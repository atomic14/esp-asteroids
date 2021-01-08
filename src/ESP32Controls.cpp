#include "ESP32Controls.hpp"
#include "RotaryEncoder.hpp"
#include <math.h>

ESP32Controls::ESP32Controls(RotaryEncoder *rotary_encoder) : rotary_encoder(rotary_encoder)
{
}

bool ESP32Controls::is_firing()
{
  return false;
}

bool ESP32Controls::is_thrusting()
{
  return false;
}

float ESP32Controls::get_direction()
{
  return 2 * (float(rotary_encoder->get_count()) / 24.0f) * M_PI;
}
