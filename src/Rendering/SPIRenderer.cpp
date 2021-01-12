#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "SPIRenderer.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#define PIN_NUM_MISO 25
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK 19
#define PIN_NUM_CS 22

void IRAM_ATTR spi_draw_task(void *param)
{
  SPIRenderer *renderer = static_cast<SPIRenderer *>(param);
  renderer->draw();
}

void IRAM_ATTR SPIRenderer::draw()
{
  while (true)
  {
    for (const auto &instruction : (*render_buffer->display_frame))
    {
      set_laser(instruction.laser);
      // channel A
      spi_transaction_t t1;
      memset(&t1, 0, sizeof(t1)); //Zero out the transaction
      t1.length = 16;
      t1.flags = SPI_TRANS_USE_TXDATA;
      t1.tx_data[0] = 0b0010000 || ((instruction.x >> 4) & 15);
      t1.tx_data[1] = instruction.x;
      spi_device_polling_transmit(spi, &t1);
      // channel B
      spi_transaction_t t2;
      memset(&t2, 0, sizeof(t2)); //Zero out the transaction
      t2.length = 16;
      t2.flags = SPI_TRANS_USE_TXDATA;
      t2.tx_data[0] = 0b1010000 || ((instruction.y >> 4) & 15);
      t2.tx_data[1] = instruction.y;
      spi_device_polling_transmit(spi, &t2);

      transactions += 1;
      // load the DAC
      gpio_set_level(PIN_NUM_LDAC, 0);
      gpio_set_level(PIN_NUM_LDAC, 1);
    }
    // vTaskDelay(1);
    // trigger a re-render
    render_buffer->swapBuffers();
    rendered_frames++;
  }
}

SPIRenderer::SPIRenderer(float world_size)
{
  render_buffer = new RenderBuffer(
      0, 4096,
      0, 4096,
      2048,
      2048,
      2048.0f / world_size);
}

void SPIRenderer::start()
{
  // setup the LDAC output
  gpio_set_direction(PIN_NUM_LDAC, GPIO_MODE_OUTPUT);

  // setup SPI output
  esp_err_t ret;
  spi_bus_config_t buscfg = {
      .mosi_io_num = PIN_NUM_MOSI,
      .miso_io_num = PIN_NUM_MISO,
      .sclk_io_num = PIN_NUM_CLK,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
      .max_transfer_sz = 0};
  spi_device_interface_config_t devcfg = {
      .command_bits = 0,          ///< Default amount of bits in command phase (0-16), used when ``SPI_TRANS_VARIABLE_CMD`` is not used, otherwise ignored.
      .address_bits = 0,          ///< Default amount of bits in address phase (0-64), used when ``SPI_TRANS_VARIABLE_ADDR`` is not used, otherwise ignored.
      .dummy_bits = 0,            ///< Amount of dummy bits to insert between address and data phase
      .mode = 0,                  //SPI mode 0
      .clock_speed_hz = 3600000,  //Clock out at approx 32 bits * 20KHz - TODO - investigate what clock speed we should use...
      .spics_io_num = PIN_NUM_CS, //CS pin
      .flags = SPI_DEVICE_NO_DUMMY,
      .queue_size = 2, //We want to be able to queue 2 transactions at a time
                       // .post_cb = spi_post_callback // We'll use this for the LDAC line
  };
  //Initialize the SPI bus
  ret = spi_bus_initialize(HSPI_HOST, &buscfg, 1);
  assert(ret == ESP_OK);
  //Attach the SPI device
  ret = spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
  assert(ret == ESP_OK);

  // setup the task for sending samples
  xTaskCreatePinnedToCore(spi_draw_task, "Draw Task", 4096, this, 10, &spi_task_handle, 1);

  Renderer::start();
}
