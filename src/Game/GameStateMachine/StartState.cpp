#include "StartState.hpp"
#include "../Game.hpp"
#include "../../Controls/Controls.hpp"

GAME_STATE StartState::handle(Game *game, float elapsed_time)
{
  // wait for fire to be pushed
  if (game->get_controls()->is_firing())
  {
    return GAME_STATE_PLAYING;
  }
  return GAME_STATE_START;
}

void StartState::enter(Game *game)
{
}
void StartState::exit(Game *game)
{
}
