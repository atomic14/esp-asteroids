#pragma once

#include "GameState.hpp"

class StartState: public GameState
{
public:
  void enter(Game *game);
  GAME_STATE handle(Game *game, float elapsed_time);
  void exit(Game *game);
  virtual const char *get_text()
  {
    return "Press Fire!";
  }
};