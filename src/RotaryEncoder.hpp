#ifndef _rotary_encoder_h_
#define _rotary_encoder_h_

#include <driver/gpio.h>

typedef void *TaskHandle_t;

class RotaryEncoder
{
private:
  gpio_num_t _clck_pin;
  gpio_num_t _di_pin;

  TaskHandle_t _rotary_decoder_task_handle;

  void interrupt_handler();
  int _count;

public:
  RotaryEncoder(gpio_num_t clck_pin, gpio_num_t di_pin);
  ~RotaryEncoder();
  int get_count();
  friend void _interrupt_handler(void *param);
  friend void _rotary_decoder_task(void *param);
};

#endif