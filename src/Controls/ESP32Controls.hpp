#ifndef ESP32_Controls_hpp
#define ESP32_Controls_hpp

#include "Controls.hpp"

class RotaryEncoder;

class ESP32Controls : public Controls
{
private:
public:
  RotaryEncoder *rotary_encoder;
  ESP32Controls(RotaryEncoder *rotary_encoder);
  bool is_firing();
  bool is_thrusting();
  float get_direction();
};

#endif