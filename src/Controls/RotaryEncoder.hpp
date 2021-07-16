#pragma once

class RotaryEncoder
{
public:
  virtual ~RotaryEncoder(){};
  virtual float get_angle() = 0;
};
