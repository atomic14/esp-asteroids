#pragma once

#include <vector>
#include <map>
#include "Font.hpp"

class SimpleFont : public Font
{
private:
  std::map<char, Character> characters;

public:
  SimpleFont();
  const Character *get_character(char c);
  float get_x_scale()
  {
    return 2;
  }
  float get_y_scale()
  {
    return 4;
  }
  float get_default_width()
  {
    return 1.5;
  }
};