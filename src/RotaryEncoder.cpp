#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_timer.h>
#include "RotaryEncoder.hpp"

IRAM_ATTR void _interrupt_handler(void *param)
{
  auto rotary_encoder = static_cast<RotaryEncoder *>(param);
  gpio_intr_disable(rotary_encoder->_clck_pin);
  // capture the value of the di pin now and send it to the task
  int di = gpio_get_level(rotary_encoder->_di_pin);
  xTaskNotifyFromISR(rotary_encoder->_rotary_decoder_task_handle, 2 | di, eSetBits, NULL);
}

void _rotary_decoder_task(void *param)
{
  printf("In task routing");
  auto rotary_encoder = static_cast<RotaryEncoder *>(param);
  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(10000);
  while (true)
  {
    uint32_t ulNotificationValue = ulTaskNotifyTake(pdTRUE, xMaxBlockTime);
    if (ulNotificationValue > 0)
    {
      // check to see if the clock pin is low after a short delay to avoid any bounce issues
      int clk = gpio_get_level(rotary_encoder->_clck_pin);
      if (!clk)
      {
        // check the value of di to determine direction
        // int di = gpio_get_level(rotary_encoder->_di_pin);
        if (ulNotificationValue & 1)
        {
          rotary_encoder->_count++;
        }
        else
        {
          rotary_encoder->_count--;
        }
      }
      // re-enable the interrupts
      vTaskDelay(pdMS_TO_TICKS(10));
      gpio_intr_enable(rotary_encoder->_clck_pin);
    }
  }
}

RotaryEncoder::RotaryEncoder(gpio_num_t clck_pin, gpio_num_t di_pin)
{
  xTaskCreate(_rotary_decoder_task, "Rotary Decode", 4096, this, 0, &_rotary_decoder_task_handle);
  _count = 0;
  _clck_pin = clck_pin;
  _di_pin = di_pin;
  gpio_install_isr_service(0);
  gpio_set_direction(clck_pin, GPIO_MODE_INPUT);
  gpio_set_direction(di_pin, GPIO_MODE_INPUT);
  gpio_set_intr_type(clck_pin, GPIO_INTR_ANYEDGE);
  gpio_isr_handler_add(clck_pin, _interrupt_handler, this);
  gpio_intr_enable(clck_pin);
}

int RotaryEncoder::get_count()
{
  return _count;
}

RotaryEncoder::~RotaryEncoder()
{
  gpio_intr_disable(_clck_pin);
  gpio_isr_handler_remove(_clck_pin);
}
