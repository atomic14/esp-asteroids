#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_timer.h>
#include "RotaryEncoder.hpp"

#define ON 0
#define OFF 1
#define MIN_CLK_PULSE_US 10000

IRAM_ATTR void _a_interrupt_handler(void *param)
{
  auto rotary_encoder = static_cast<RotaryEncoder *>(param);
  int64_t now = esp_timer_get_time();
  if (rotary_encoder->_a_pin_state)
  {
    // we were OFF, so now we must be ON
    rotary_encoder->_a_pin_state = false;
    // start waiting for an OFF value
    gpio_set_intr_type(rotary_encoder->_a_pin, OFF ? GPIO_INTR_HIGH_LEVEL : GPIO_INTR_LOW_LEVEL);
    // capture the time when we went ON
    rotary_encoder->_last_triggered = now;
    // sample and store the value of the direction pin for use later
    rotary_encoder->_b_value = gpio_get_level(rotary_encoder->_b_pin);
  }
  else
  {
    // we were low, so now we must be high
    rotary_encoder->_a_pin_state = true;
    // switch to waiting for an ON value
    gpio_set_intr_type(rotary_encoder->_a_pin, ON ? GPIO_INTR_HIGH_LEVEL : GPIO_INTR_LOW_LEVEL);
    // was the low pulse long enough to be valid?
    if (now - rotary_encoder->_last_triggered >= MIN_CLK_PULSE_US)
    {
      if (rotary_encoder->_b_value == ON)
      {
        rotary_encoder->_count++;
      }
      else
      {
        rotary_encoder->_count--;
      }
    }
  }
}

RotaryEncoder::RotaryEncoder(gpio_num_t clck_pin, gpio_num_t di_pin)
{
  _count = 0;
  _a_pin = clck_pin;
  _b_pin = di_pin;
  _last_triggered = 0;
  gpio_install_isr_service(0);
  gpio_set_direction(clck_pin, GPIO_MODE_INPUT);
  gpio_set_direction(di_pin, GPIO_MODE_INPUT);
  // start off with the clk pin in the OFF state waiting for an ON level
  _a_pin_state = OFF;
  gpio_isr_handler_add(clck_pin, _a_interrupt_handler, this);
  gpio_set_intr_type(clck_pin, ON ? GPIO_INTR_HIGH_LEVEL : GPIO_INTR_LOW_LEVEL);
  gpio_intr_enable(clck_pin);
}

RotaryEncoder::~RotaryEncoder()
{
  gpio_intr_disable(_a_pin);
  gpio_isr_handler_remove(_a_pin);
}
