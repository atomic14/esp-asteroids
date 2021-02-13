#pragma once

#include "GameState.hpp"

class PlayingState : public GameState
{
private:
  float firing_cooldown;
  bool is_respawning;
  float respawn_cooldown;
  float thrust_sound_cooldown;

public:
  void enter(Game *game);
  GAME_STATE handle(Game *game, float elapsed_time);
  void exit(Game *game);
  const char *get_text()
  {
    if (is_respawning)
    {
      return "PRESS FIRE";
    }
    return nullptr;
  }
};