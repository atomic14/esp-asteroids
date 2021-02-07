#pragma once

typedef enum
{
  GAME_STATE_START,    // ready to start a new game
  GAME_STATE_PLAYING,  // a game is in progress
  GAME_STATE_GAME_OVER // game over!
} GAME_STATE;

class Game;
class GameState
{
public:
  virtual void enter(Game *game) = 0;
  virtual GAME_STATE handle(Game *game, float elapsed_time) = 0;
  virtual void exit(Game *game) = 0;
  virtual const char *get_text();
};