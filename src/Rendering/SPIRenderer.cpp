#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "SPIRenderer.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "driver/timer.h"

#define PIN_NUM_MISO -1
#define PIN_NUM_MOSI 25
#define PIN_NUM_CLK 26
#define PIN_NUM_CS 27
#define PIN_NUM_LDAC GPIO_NUM_33

void IRAM_ATTR spi_draw_timer(void *para)
{
  timer_spinlock_take(TIMER_GROUP_0);
  SPIRenderer *renderer = static_cast<SPIRenderer *>(para);
  renderer->draw();
}

void IRAM_ATTR SPIRenderer::draw()
{
  // Clear the interrupt
  timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);
  // After the alarm has been triggered we need enable it again, so it is triggered the next time
  timer_group_enable_alarm_in_isr(TIMER_GROUP_0, TIMER_0);
  // do the actual drawing
  if (hold > 0)
  {
    delays++;
    hold--;
  }
  else
  {
    // do we still have things to draw?
    if (draw_position < render_buffer->display_frame->size())
    {
      const DrawInstruction_t &instruction = render_buffer->display_frame->at(draw_position);
      hold = instruction.hold;
      // channel A
      int x = 4095 - instruction.x;
      int y = 4095 - instruction.y;
      spi_transaction_t t1;
      memset(&t1, 0, sizeof(t1)); //Zero out the transaction
      t1.length = 16;
      t1.flags = SPI_TRANS_USE_TXDATA;
      t1.tx_data[0] = 0b01010000 | ((x >> 8) & 0xF);
      t1.tx_data[1] = x & 255;
      spi_device_polling_transmit(spi, &t1);
      // channel B
      spi_transaction_t t2;
      memset(&t2, 0, sizeof(t2)); //Zero out the transaction
      t2.length = 16;
      t2.flags = SPI_TRANS_USE_TXDATA;
      t2.tx_data[0] = 0b11010000 | ((y >> 8) & 0xF);
      t2.tx_data[1] = y & 255;
      spi_device_polling_transmit(spi, &t2);

      // set the laser state
      set_laser(instruction.laser);

      // load the DAC
      gpio_set_level(PIN_NUM_LDAC, 0);
      gpio_set_level(PIN_NUM_LDAC, 1);

      draw_position++;
      transactions++;
    }
    else
    {
      // trigger a re-render
      rendered_frames++;
      render_buffer->swapBuffers();
      draw_position = 0;
    }
  }
  timer_spinlock_give(TIMER_GROUP_0);
}

SPIRenderer::SPIRenderer(float world_size, Font *font)
{
  draw_position = 0;
  hold = 0;
  render_buffer = new RenderBuffer(
      1024, 3072,
      1024, 3072,
      2048,
      2048,
      1024.0f / world_size,
      font);
  // 0, 4095,
  // 0, 4095,
  // 2048,
  // 2048,
  // 2048.0f / world_size);
}

void spi_timer_setup(void *param)
{
  // set up the renderer timer
  timer_config_t config = {
      .alarm_en = TIMER_ALARM_EN,
      .counter_en = TIMER_PAUSE,
      .intr_type = TIMER_INTR_LEVEL,
      .counter_dir = TIMER_COUNT_UP,
      .auto_reload = TIMER_AUTORELOAD_EN,
      .divider = 4000}; // default clock source is APB
  timer_init(TIMER_GROUP_0, TIMER_0, &config);

  // Timer's counter will initially start from value below.
  //   Also, if auto_reload is set, this value will be automatically reload on alarm
  timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0x00000000ULL);

  // Configure the alarm value and the interrupt on alarm.
  timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 0x00000001ULL);
  timer_enable_intr(TIMER_GROUP_0, TIMER_0);
  timer_isr_register(TIMER_GROUP_0, TIMER_0, spi_draw_timer,
                     param, ESP_INTR_FLAG_IRAM, NULL);

  timer_start(TIMER_GROUP_0, TIMER_0);
  while (true)
  {
    vTaskDelay(10000000);
  }
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
      .command_bits = 0, ///< Default amount of bits in command phase (0-16), used when ``SPI_TRANS_VARIABLE_CMD`` is not used, otherwise ignored.
      .address_bits = 0, ///< Default amount of bits in address phase (0-64), used when ``SPI_TRANS_VARIABLE_ADDR`` is not used, otherwise ignored.
      .dummy_bits = 0,   ///< Amount of dummy bits to insert between address and data phase
      .mode = 0,         //SPI mode 0
      .clock_speed_hz = 80000000,
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

  // make sure to start the task on CPU 1
  TaskHandle_t timer_setup_handle;
  xTaskCreatePinnedToCore(spi_timer_setup, "Draw Task", 4096, this, 0, &timer_setup_handle, 1);

  Renderer::start();
}
