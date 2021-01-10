#ifndef _rotary_encoder_h_
#define _rotary_encoder_h_

#include <driver/gpio.h>

class RotaryEncoder
{
private:
  gpio_num_t _a_pin;
  gpio_num_t _b_pin;
  volatile int _a_pin_state;
  volatile int _count;
  volatile int _b_value;
  volatile int64_t _last_triggered;

public:
  RotaryEncoder(gpio_num_t a_pin, gpio_num_t b_pin);
  ~RotaryEncoder();
  int get_count() { return _count; }
  friend void _a_interrupt_handler(void *param);
};

#endif