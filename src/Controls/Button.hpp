#ifndef _BUTTON_HPP
#define _BUTTON_HPP

#include <driver/gpio.h>

class Button
{
private:
  gpio_num_t _button_pin;

public:
  Button(gpio_num_t button_pin);
  bool get_button_pushed();
};

#endif