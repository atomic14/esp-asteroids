#pragma once

#include "GameState.hpp"

class GameOverState : public GameState
{
public:
  void enter(Game *game);
  GAME_STATE handle(Game *game, float elapsed_time);
  void exit(Game *game);
  const char *get_text()
  {
    return "Game Over!";
  }
};