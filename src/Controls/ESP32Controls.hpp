#ifndef ESP32_Controls_hpp
#define ESP32_Controls_hpp

#include "Controls.hpp"

class RotaryEncoder;
class Button;

class ESP32Controls : public Controls
{
private:
  RotaryEncoder *rotary_encoder;
  Button *fire_button;

public:
  ESP32Controls(RotaryEncoder *rotary_encoder, Button *fire_button);
  bool is_firing();
  bool is_thrusting();
  float get_direction();
};

#endif