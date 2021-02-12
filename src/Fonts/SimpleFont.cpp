#include "SimpleFont.hpp"

SimpleFont::SimpleFont()
{
  characters['0'].left_pos = 0;
  characters['0'].right_pos = 1.5;

  characters['0'].commands.push_back({.coords = b2Vec2(0, 1), .draw = false});
  characters['0'].commands.push_back({.coords = b2Vec2(0, 0), .draw = true});
  characters['0'].commands.push_back({.coords = b2Vec2(1, 0), .draw = true});
  characters['0'].commands.push_back({.coords = b2Vec2(1, 1), .draw = true});
  characters['0'].commands.push_back({.coords = b2Vec2(0, 1), .draw = true});

  characters['1'].left_pos = 0;
  characters['1'].right_pos = 0.5;

  characters['1'].commands.push_back({.coords = b2Vec2(0, 1), .draw = false});
  characters['1'].commands.push_back({.coords = b2Vec2(0, 0), .draw = true});

  characters['2'].left_pos = 0;
  characters['2'].right_pos = 1.5;

  characters['2'].commands.push_back({.coords = b2Vec2(0, 0), .draw = false});
  characters['2'].commands.push_back({.coords = b2Vec2(1, 0), .draw = true});
  characters['2'].commands.push_back({.coords = b2Vec2(1, 0.5), .draw = true});
  characters['2'].commands.push_back({.coords = b2Vec2(0, 0.5), .draw = true});
  characters['2'].commands.push_back({.coords = b2Vec2(0, 1), .draw = true});
  characters['2'].commands.push_back({.coords = b2Vec2(1, 1), .draw = true});

  characters['3'].left_pos = 0;
  characters['3'].right_pos = 1.5;

  characters['3'].commands.push_back({.coords = b2Vec2(0, 1), .draw = false});
  characters['3'].commands.push_back({.coords = b2Vec2(1, 1), .draw = true});
  characters['3'].commands.push_back({.coords = b2Vec2(1, 0.5), .draw = true});
  characters['3'].commands.push_back({.coords = b2Vec2(0, 0.5), .draw = true});
  characters['3'].commands.push_back({.coords = b2Vec2(1, 0.5), .draw = false});
  characters['3'].commands.push_back({.coords = b2Vec2(1, 0), .draw = true});
  characters['3'].commands.push_back({.coords = b2Vec2(0, 0), .draw = true});

  characters['4'].left_pos = 0;
  characters['4'].right_pos = 1.5;

  characters['4'].commands.push_back({.coords = b2Vec2(1, 1), .draw = false});
  characters['4'].commands.push_back({.coords = b2Vec2(1, 0), .draw = true});
  characters['4'].commands.push_back({.coords = b2Vec2(0, 0), .draw = false});
  characters['4'].commands.push_back({.coords = b2Vec2(0, 0.5), .draw = true});
  characters['4'].commands.push_back({.coords = b2Vec2(1, 0.5), .draw = true});

  characters['5'].left_pos = 0;
  characters['5'].right_pos = 1.5;

  characters['5'].commands.push_back({.coords = b2Vec2(1, 0), .draw = false});
  characters['5'].commands.push_back({.coords = b2Vec2(0, 0), .draw = true});
  characters['5'].commands.push_back({.coords = b2Vec2(0, 0.5), .draw = true});
  characters['5'].commands.push_back({.coords = b2Vec2(1, 0.5), .draw = true});
  characters['5'].commands.push_back({.coords = b2Vec2(1, 1), .draw = true});
  characters['5'].commands.push_back({.coords = b2Vec2(0, 1), .draw = true});

  characters['6'].left_pos = 0;
  characters['6'].right_pos = 1.5;

  characters['6'].commands.push_back({.coords = b2Vec2(0, 0), .draw = false});
  characters['6'].commands.push_back({.coords = b2Vec2(0, 1), .draw = true});
  characters['6'].commands.push_back({.coords = b2Vec2(1, 1), .draw = true});
  characters['6'].commands.push_back({.coords = b2Vec2(1, 0.5), .draw = true});
  characters['6'].commands.push_back({.coords = b2Vec2(0, 0.5), .draw = true});

  characters['7'].left_pos = 0;
  characters['7'].right_pos = 1.5;

  characters['7'].commands.push_back({.coords = b2Vec2(0, 0), .draw = false});
  characters['7'].commands.push_back({.coords = b2Vec2(1, 0), .draw = true});
  characters['7'].commands.push_back({.coords = b2Vec2(0, 1), .draw = true});

  characters['8'].left_pos = 0;
  characters['8'].right_pos = 1.5;

  characters['8'].commands.push_back({.coords = b2Vec2(0, 0.5), .draw = false});
  characters['8'].commands.push_back({.coords = b2Vec2(1, 0.5), .draw = true});
  characters['8'].commands.push_back({.coords = b2Vec2(1, 0), .draw = true});
  characters['8'].commands.push_back({.coords = b2Vec2(0, 0), .draw = true});
  characters['8'].commands.push_back({.coords = b2Vec2(0, 1), .draw = true});
  characters['8'].commands.push_back({.coords = b2Vec2(1, 1), .draw = true});
  characters['8'].commands.push_back({.coords = b2Vec2(1, 0.5), .draw = true});

  characters['9'].left_pos = 0;
  characters['9'].right_pos = 1.5;

  characters['9'].commands.push_back({.coords = b2Vec2(1, 1), .draw = false});
  characters['9'].commands.push_back({.coords = b2Vec2(1, 0), .draw = true});
  characters['9'].commands.push_back({.coords = b2Vec2(0, 0), .draw = true});
  characters['9'].commands.push_back({.coords = b2Vec2(0, 0.5), .draw = true});
  characters['9'].commands.push_back({.coords = b2Vec2(1, 0.5), .draw = true});

  characters['F'].left_pos = 0;
  characters['F'].right_pos = 1.5;

  characters['F'].commands.push_back({.coords = b2Vec2(1, 0), .draw = false});
  characters['F'].commands.push_back({.coords = b2Vec2(0, 0), .draw = true});
  characters['F'].commands.push_back({.coords = b2Vec2(0, 1), .draw = true});
  characters['F'].commands.push_back({.coords = b2Vec2(0, 0.5), .draw = false});
  characters['F'].commands.push_back({.coords = b2Vec2(1, 0.5), .draw = true});

  characters['I'] = characters['1'];

  characters['R'].left_pos = 0;
  characters['R'].right_pos = 1.5;

  characters['R'].commands.push_back({.coords = b2Vec2(0, 1), .draw = false});
  characters['R'].commands.push_back({.coords = b2Vec2(0, 0), .draw = true});
  characters['R'].commands.push_back({.coords = b2Vec2(1, 0), .draw = true});
  characters['R'].commands.push_back({.coords = b2Vec2(1, 0.5), .draw = true});
  characters['R'].commands.push_back({.coords = b2Vec2(0, 0.5), .draw = true});
  characters['R'].commands.push_back({.coords = b2Vec2(1, 1), .draw = true});

  characters['E'].left_pos = 0;
  characters['E'].right_pos = 1.5;

  characters['E'].commands.push_back({.coords = b2Vec2(1, 1), .draw = false});
  characters['E'].commands.push_back({.coords = b2Vec2(0, 1), .draw = true});
  characters['E'].commands.push_back({.coords = b2Vec2(0, 0), .draw = true});
  characters['E'].commands.push_back({.coords = b2Vec2(1, 0), .draw = true});
  characters['E'].commands.push_back({.coords = b2Vec2(1, 0.5), .draw = false});
  characters['E'].commands.push_back({.coords = b2Vec2(0, 0.5), .draw = true});

  characters['P'].left_pos = 0;
  characters['P'].right_pos = 1.5;

  characters['P'].commands.push_back({.coords = b2Vec2(0, 1), .draw = false});
  characters['P'].commands.push_back({.coords = b2Vec2(0, 0), .draw = true});
  characters['P'].commands.push_back({.coords = b2Vec2(1, 0), .draw = true});
  characters['P'].commands.push_back({.coords = b2Vec2(1, 0.5), .draw = true});
  characters['P'].commands.push_back({.coords = b2Vec2(0, 0.5), .draw = true});

  characters['S'] = characters['5'];

  characters['G'].left_pos = 0;
  characters['G'].right_pos = 1.5;

  characters['G'].commands.push_back({.coords = b2Vec2(1, 0), .draw = false});
  characters['G'].commands.push_back({.coords = b2Vec2(0, 0), .draw = true});
  characters['G'].commands.push_back({.coords = b2Vec2(0, 1), .draw = true});
  characters['G'].commands.push_back({.coords = b2Vec2(1, 1), .draw = true});
  characters['G'].commands.push_back({.coords = b2Vec2(1, 0.5), .draw = true});

  characters['A'].left_pos = 0;
  characters['A'].right_pos = 1.5;

  characters['A'].commands.push_back({.coords = b2Vec2(0, 1), .draw = false});
  characters['A'].commands.push_back({.coords = b2Vec2(0, 0), .draw = true});
  characters['A'].commands.push_back({.coords = b2Vec2(1, 0), .draw = true});
  characters['A'].commands.push_back({.coords = b2Vec2(1, 1), .draw = true});
  characters['A'].commands.push_back({.coords = b2Vec2(1, 0.5), .draw = false});
  characters['A'].commands.push_back({.coords = b2Vec2(0, 0.5), .draw = true});

  characters['M'].left_pos = 0;
  characters['M'].right_pos = 1.5;

  characters['M'].commands.push_back({.coords = b2Vec2(0, 1), .draw = false});
  characters['M'].commands.push_back({.coords = b2Vec2(0, 0), .draw = true});
  characters['M'].commands.push_back({.coords = b2Vec2(0.5, 0.5), .draw = true});
  characters['M'].commands.push_back({.coords = b2Vec2(1, 0), .draw = true});
  characters['M'].commands.push_back({.coords = b2Vec2(1, 1), .draw = true});

  characters['O'] = characters['0'];

  characters['V'].left_pos = 0;
  characters['V'].right_pos = 1.5;

  characters['V'].commands.push_back({.coords = b2Vec2(0, 0), .draw = false});
  characters['V'].commands.push_back({.coords = b2Vec2(0.5, 1), .draw = true});
  characters['V'].commands.push_back({.coords = b2Vec2(1, 0), .draw = true});
}

const Character *SimpleFont::get_character(char c)
{
  auto character = characters.find(c);
  if (character != characters.end())
  {
    return &character->second;
  }
  return nullptr;
}
