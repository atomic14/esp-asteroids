#ifndef _rotary_encoder_h_
#define _rotary_encoder_h_

#include <driver/gpio.h>

typedef void *TaskHandle_t;

class RotaryEncoder
{
private:
  gpio_num_t _clck_pin;
  gpio_num_t _di_pin;
  gpio_num_t _button_pin;

  TaskHandle_t _rotary_decoder_task_handle;

  int _count;
  bool was_button_pushed;

public:
  RotaryEncoder(gpio_num_t clck_pin, gpio_num_t di_pin, gpio_num_t button_pin);
  ~RotaryEncoder();
  int get_count();
  bool get_button_pushed();

  friend void _rotary_interrupt_handler(void *param);
  friend void _rotary_decoder_task(void *param);
  friend void _push_button_interrupt_handler(void *param);
};

#endif