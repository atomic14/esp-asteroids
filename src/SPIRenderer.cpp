#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "SPIRenderer.h"
#include "driver/timer.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#define PIN_NUM_MISO 25
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK 19
#define PIN_NUM_CS 22

void IRAM_ATTR spi_post_callback(spi_transaction_t *t)
{
  SPIRenderer *renderer = static_cast<SPIRenderer *>(t->user);
  renderer->ldac_calls++;
  gpio_set_level(PIN_NUM_LDAC, 0);
  gpio_set_level(PIN_NUM_LDAC, 1);
}

void SPIRenderer::start()
{
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
      .mode = 0,                          //SPI mode 0
      .clock_speed_hz = 10 * 1000 * 1000, //Clock out at 10 MHz
      .spics_io_num = PIN_NUM_CS,         //CS pin
      .queue_size = 2,                    //We want to be able to queue 2 transactions at a time
      .post_cb = spi_post_callback        // We'll use this for the LDAC line
  };
  //Initialize the SPI bus
  ret = spi_bus_initialize(HSPI_HOST, &buscfg, 1);
  assert(ret == ESP_OK);
  //Attach the SPI device
  ret = spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
  assert(ret == ESP_OK);

  Renderer::start();
}

void SPIRenderer::stop()
{
  Renderer::stop();
  spi_bus_remove_device(spi);
  spi_bus_free(HSPI_HOST);
}

void IRAM_ATTR SPIRenderer::draw(const DrawInstruction_t &instruction)
{
  esp_err_t ret;
  spi_transaction_t t;
  memset(&t, 0, sizeof(t)); //Zero out the transaction
  t.length = 32;
  t.flags = SPI_TRANS_USE_TXDATA;
  t.tx_data[0] = 1;
  t.tx_data[0] = 2;
  t.tx_data[0] = 3;
  t.tx_data[0] = 4;
  t.user = (void *)this;
  ret = spi_device_transmit(spi, &t); //Transmit!
  assert(ret == ESP_OK);              //Should have had no issues.
  if (ret == ESP_OK)
  {
    sample_send_success++;
  }
  else
  {
    sample_send_fail++;
  }
}