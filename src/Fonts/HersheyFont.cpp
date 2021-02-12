//
//  HersheyFont.cpp
//  HiershyFontTest
//
//  Created by Chris Greening on 05/02/2021.
//

#include "HersheyFont.hpp"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int str2int(const char *str, int len)
{
  int i;
  int ret = 0;
  for (i = 0; i < len; ++i)
  {
    if (str[i] >= '0' && str[i] <= '9')
    {
      ret = ret * 10 + (str[i] - '0');
    }
  }
  return ret;
}

int hersheyCharToNumber(char c)
{
  return c - 'R';
}

const char *skip_newlines(const char *data)
{
  while (*data == '\n')
  {
    data++;
  }
  return data;
}

void HersheyFont::read_from_file(const char *fname)
{
  FILE *fp = fopen(fname, "rb");
  fseek(fp, 0, SEEK_END);
  int length = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  char *data = (char *)malloc(length);
  fread(data, 1, length, fp);
  parse(data);
  free(data);
  fclose(fp);
}

void HersheyFont::parse(const char *data)
{
  // Load Hershey .jhf file. See https://emergent.unpythonic.net/software/hershey for source files
  // futural.jhf is a good candidate as it is a single stroke sans-serif font
  while (*data != '\0')
  {
    data = skip_newlines(data);
    Character character;
    // 0-4 number (not used)
    // 5-7 number of vertices
    // 8   left pos
    // 9   right pos
    // 10- vertices
    // newline
    data += 5;
    int vertex_count = str2int(data, 3);
    data += 3;
    character.left_pos = hersheyCharToNumber(*(data++));
    character.right_pos = hersheyCharToNumber(*(data++));
    for (int i = 0; i < vertex_count - 1; i++)
    {
      data = skip_newlines(data);
      bool draw = true;
      char x = *(data++);
      char y = *(data++);
      if (x == ' ' && y == 'R')
      {
        draw = false;
        data = skip_newlines(data);
        x = *(data++);
        y = *(data++);
        i++;
      }
      character.commands.push_back({b2Vec2(hersheyCharToNumber(x), hersheyCharToNumber(y)), .draw = draw});
    }
    chars.push_back(character);
  }
}

const Character *HersheyFont::get_character(char c)
{
  int char_index = c - ' ';
  if (char_index >= 0 && char_index < chars.size())
  {
    return &chars[char_index];
  }
  return nullptr;
}
