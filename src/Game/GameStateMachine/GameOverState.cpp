#include "GameOverState.hpp"
#include "../../Controls/Controls.hpp"
#include "../Game.hpp"

GAME_STATE GameOverState::handle(Game *game, float elapsed_time)
{
  // wait for fire to be pushed
  if (game->get_controls()->is_firing())
  {
    return GAME_STATE_PLAYING;
  }
  return GAME_STATE_GAME_OVER;
}

void GameOverState::enter(Game *game)
{
}
void GameOverState::exit(Game *game)
{
}