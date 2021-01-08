#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "DACRenderer.h"
#include "driver/dac.h"

void DACRenderer::start()
{
  printf("Enabled dac output");
  dac_output_enable(DAC_CHANNEL_1);
  dac_output_enable(DAC_CHANNEL_2);
  Renderer::start();
}

void DACRenderer::stop()
{
  Renderer::stop();
  printf("Disabling dac output");
  dac_output_disable(DAC_CHANNEL_1);
  dac_output_disable(DAC_CHANNEL_2);
}

void IRAM_ATTR DACRenderer::draw(const DrawInstruction_t &instruction)
{
  gpio_set_level(PIN_NUM_LASER, instruction.laser ? 1 : 0);
  uint8_t output_x = instruction.x;
  uint8_t output_y = instruction.y;
  dac_output_voltage(DAC_CHANNEL_1, output_x);
  dac_output_voltage(DAC_CHANNEL_2, output_y);
  sample_send_success++;
}
