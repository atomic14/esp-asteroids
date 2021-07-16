#ifndef _rotary_encoder_h_
#define _rotary_encoder_h_

#include <math.h>
#include <driver/gpio.h>
#include "RotaryEncoder.hpp"

class MechanicalRotaryEncoder : public RotaryEncoder
{
private:
  gpio_num_t _a_pin;
  gpio_num_t _b_pin;
  volatile int _a_pin_state;
  volatile int _count;
  volatile int _b_value;
  volatile int64_t _last_triggered;

public:
  MechanicalRotaryEncoder(gpio_num_t a_pin, gpio_num_t b_pin);
  ~MechanicalRotaryEncoder();
  float get_angle()
  {
    return -3.0 * (float(_count) / 24.0f) * M_PI;
  }
  friend void _a_interrupt_handler(void *param);
};

#endif