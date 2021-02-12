#pragma once

#include "box2d/box2d.h"

typedef struct
{
  b2Vec2 coords;
  bool draw;
} FontCoord_t;

class Character
{
public:
  Character()
  {
    left_pos = 0.0f;
    right_pos = 0.0f;
  }
  std::vector<FontCoord_t> commands;
  float left_pos;
  float right_pos;
};

class Font
{
public:
  virtual const Character *get_character(char c) = 0;
  virtual float get_x_scale() = 0;
  virtual float get_y_scale() = 0;
  virtual float get_default_width() = 0;
};
