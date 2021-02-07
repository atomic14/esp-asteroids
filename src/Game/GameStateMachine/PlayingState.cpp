#include "PlayingState.hpp"
#include "../Game.hpp"
#include "../../Controls/Controls.hpp"
#include "../../Audio/SoundFX.h"
#include "../ShipObject.hpp"

#define FIRE_COOLDOWN 0.1

void PlayingState::enter(Game *game)
{
  // clear down the current game state
  game->reset();
  game->add_asteroids();
  game->add_player_ship();
  game->add_lives();
  game->set_score(0);
  // clear our own state down
  firing_cooldown = 0;
}

GAME_STATE PlayingState::handle(Game *game, float elapsed_time)
{
  if (firing_cooldown > 0)
  {
    firing_cooldown -= elapsed_time;
  }
  // upate the angle of the ship
  game->get_ship()->setAngle(game->get_controls()->get_direction());
  // is the user pushing the thrust button?
  if (game->get_controls()->is_thrusting())
  {
    game->get_ship()->thrust(500 * elapsed_time);
    game->get_sound_fx()->thrust();
    game->get_ship()->set_is_thrusting(true);
  }
  else
  {
    game->get_ship()->set_is_thrusting(false);
  }
  // handle the fire button
  if (game->get_controls()->is_firing() && firing_cooldown <= 0 && game->can_add_bullet())
  {
    game->add_bullet();
    // prevent firing for some time period
    firing_cooldown = FIRE_COOLDOWN;
    game->get_sound_fx()->fire();
  }
  if (!game->has_asteroids())
  {
    game->increase_difficulty();
    // need to create new asteroids
    game->add_asteroids();
  }
  return GAME_STATE_PLAYING;
}

void PlayingState::exit(Game *game)
{
  // nothing to do
}