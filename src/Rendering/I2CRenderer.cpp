#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "I2CRenderer.h"
#include "driver/i2c.h"
#include "driver/gpio.h"

#define MCP4725_ADDR_X 0x60
#define MCP4725_ADDR_Y 0x61

#define MCP4725_WRITE_FAST 0x00
#define MCP4725_WRITE_DAC 0x40
#define MCP4725_WRITE_DAC_EEPROM 0x60
#define MCP4725_MASK 0xFF

#define GPIO_SCL 27
#define GPIO_SDA 26

void IRAM_ATTR i2c_draw_task(void *param)
{
  I2CRenderer *renderer = static_cast<I2CRenderer *>(param);
  renderer->draw();
}

void IRAM_ATTR I2CRenderer::draw()
{
  while (true)
  {
    uint8_t buffer[3];
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    for (const auto &instruction : (*render_buffer->display_frame))
    {
      set_laser(instruction.laser);
      // channel A
      buffer[0] = (MCP4725_ADDR_X << 1) | I2C_MASTER_WRITE;
      buffer[1] = (instruction.x >> 8) | MCP4725_WRITE_FAST;
      buffer[2] = instruction.x & MCP4725_MASK;
      {
        i2c_master_start(cmd);
        i2c_master_write(cmd, buffer, 3, false);
        i2c_master_stop(cmd);
        i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
      }
      // channel B
      buffer[0] = (MCP4725_ADDR_Y << 1) | I2C_MASTER_WRITE;
      buffer[1] = (instruction.y >> 8) | MCP4725_WRITE_FAST;
      buffer[2] = instruction.y & MCP4725_MASK;
      {
        i2c_master_start(cmd);
        i2c_master_write(cmd, buffer, 3, false);
        i2c_master_stop(cmd);
        i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
      }
      transactions += 1;
    }
    i2c_cmd_link_delete(cmd);
    // trigger a re-render
    render_buffer->swapBuffers();
    rendered_frames++;
  }
}

I2CRenderer::I2CRenderer(float world_size)
{
  render_buffer = new RenderBuffer(
      0, 4096,
      0, 4096,
      2048,
      2048,
      2048.0f / world_size);
}

void I2CRenderer::start()
{
  // setup I2C output
  // setup i2c
  i2c_config_t conf;
  conf.mode = I2C_MODE_MASTER;
  conf.sda_io_num = GPIO_SDA;
  conf.sda_pullup_en = false;
  conf.scl_io_num = GPIO_SCL;
  conf.scl_pullup_en = false;
  conf.master.clk_speed = 1000000;
  i2c_param_config(I2C_NUM_0, &conf);
  i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);
  // setup the task for sending samples
  xTaskCreatePinnedToCore(i2c_draw_task, "Draw Task", 4096, this, 10, &i2c_task_handle, 1);

  Renderer::start();
}
