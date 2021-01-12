#include <u8g2.h>
#include "HeltecOLEDRenderer.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "RenderBuffer.hpp"

#define I2C_MASTER_NUM I2C_NUM_1    //  I2C port number for master dev
#define I2C_MASTER_TX_BUF_DISABLE 0 //  I2C master do not need buffer
#define I2C_MASTER_RX_BUF_DISABLE 0 //  I2C master do not need buffer
#define I2C_MASTER_FREQ_HZ 1000000  //  I2C master clock frequency
#define ACK_CHECK_EN 0x0            //  I2C master will check ack from slave
#define ACK_CHECK_DIS 0x1           //  I2C master will not check ack from slave

u8g2_t u8g2;

static const unsigned int I2C_TIMEOUT_MS = 1000;
static i2c_cmd_handle_t handle_i2c; // I2C handle.

#define GPIO_SDA GPIO_NUM_4
#define GPIO_SCL GPIO_NUM_15
#define GPIO_RESET GPIO_NUM_16

// U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/15, /* data=*/4, /* reset=*/16);
uint8_t u8g2_esp32_i2c_byte_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  switch (msg)
  {
  case U8X8_MSG_BYTE_INIT:
  {
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = GPIO_SDA;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = GPIO_SCL;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0));
    break;
  }

  case U8X8_MSG_BYTE_SEND:
  {
    uint8_t *data_ptr = (uint8_t *)arg_ptr;
    while (arg_int > 0)
    {
      ESP_ERROR_CHECK(i2c_master_write_byte(handle_i2c, *data_ptr, ACK_CHECK_EN));
      data_ptr++;
      arg_int--;
    }
    break;
  }

  case U8X8_MSG_BYTE_START_TRANSFER:
  {
    uint8_t i2c_address = u8x8_GetI2CAddress(u8x8);
    handle_i2c = i2c_cmd_link_create();
    ESP_ERROR_CHECK(i2c_master_start(handle_i2c));
    ESP_ERROR_CHECK(i2c_master_write_byte(handle_i2c, i2c_address | I2C_MASTER_WRITE, ACK_CHECK_EN));
    break;
  }

  case U8X8_MSG_BYTE_END_TRANSFER:
  {
    ESP_ERROR_CHECK(i2c_master_stop(handle_i2c));
    ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_MASTER_NUM, handle_i2c, I2C_TIMEOUT_MS / portTICK_RATE_MS));
    i2c_cmd_link_delete(handle_i2c);
    break;
  }
  }
  return 0;
}
uint8_t u8g2_esp32_gpio_and_delay_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  switch (msg)
  {
    // Initialize the GPIO and DELAY HAL functions.  If the pins for DC and RESET have been
    // specified then we define those pins as GPIO outputs.
  case U8X8_MSG_GPIO_AND_DELAY_INIT:
  {
    uint64_t bitmask = 0;
    bitmask = bitmask | (1ull << GPIO_RESET);
    if (bitmask == 0)
    {
      break;
    }
    gpio_config_t gpioConfig;
    gpioConfig.pin_bit_mask = bitmask;
    gpioConfig.mode = GPIO_MODE_OUTPUT;
    gpioConfig.pull_up_en = GPIO_PULLUP_DISABLE;
    gpioConfig.pull_down_en = GPIO_PULLDOWN_ENABLE;
    gpioConfig.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&gpioConfig);
    break;
  }

    // Set the GPIO reset pin to the value passed in through arg_int.
  case U8X8_MSG_GPIO_RESET:
  {
    gpio_set_level(GPIO_RESET, arg_int);
  }
  break;
    // Set the Software I²C pin to the value passed in through arg_int.
  case U8X8_MSG_GPIO_I2C_CLOCK:
  {
    gpio_set_level(GPIO_SCL, arg_int);
  }
  break;
  case U8X8_MSG_GPIO_I2C_DATA:
  {
    gpio_set_level(GPIO_SDA, arg_int);
  }
  break;
  case U8X8_MSG_DELAY_MILLI:
    vTaskDelay(arg_int / portTICK_PERIOD_MS);
    break;
  }
  return 0;
}

void IRAM_ATTR oled_draw_task(void *param)
{
  HeltecOLEDRenderer *renderer = static_cast<HeltecOLEDRenderer *>(param);

  while (true)
  {
    u8g2_ClearBuffer(&u8g2);
    std::vector<DrawInstruction_t> *instructions = renderer->render_buffer->display_frame;
    for (int i = 0; i < instructions->size() - 1; i++)
    {
      if ((*instructions)[i].laser)
      {
        u8g2_DrawLine(&u8g2, (*instructions)[i].x, (*instructions)[i].y, (*instructions)[i + 1].x, (*instructions)[i + 1].y);
      }
      renderer->transactions++;
    }
    vTaskDelay(pdMS_TO_TICKS(33));
    u8g2_SendBuffer(&u8g2);
    // trigger a re-render
    renderer->render_buffer->swapBuffers();
    renderer->rendered_frames++;
  }
}

HeltecOLEDRenderer::HeltecOLEDRenderer(float world_size)
{
  render_buffer = new RenderBuffer(
      0, 64,
      0, 64,
      32,
      32,
      32.0f / world_size);
}

void HeltecOLEDRenderer::start()
{
  // Init Driver
  u8g2_Setup_ssd1306_i2c_128x64_noname_f(
      &u8g2,
      U8G2_R0,
      u8g2_esp32_i2c_byte_cb,
      u8g2_esp32_gpio_and_delay_cb);
  u8x8_SetI2CAddress(&u8g2.u8x8, 0x78);

  // Blank Screen
  u8g2_InitDisplay(&u8g2);
  u8g2_SetPowerSave(&u8g2, 0);
  u8g2_ClearBuffer(&u8g2);
  u8g2_SendBuffer(&u8g2);

  xTaskCreatePinnedToCore(oled_draw_task, "OLED Draw Task", 4096, this, 10, &_draw_task_handle, 1);
}
