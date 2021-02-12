#include "esp_log.h"
#include "PlayingState.hpp"
#include "../Game.hpp"
#include "../../Controls/Controls.hpp"
#include "../../Audio/SoundFX.h"
#include "../GameObjects/ShipObject.hpp"

#define FIRE_COOLDOWN 0.1f
#define RESPAWN_COOLDOWN 1.0f

static const char *TAG = "GAME";

void PlayingState::enter(Game *game)
{
  game->start_new_game();
  // clear our own state down
  firing_cooldown = 0;
  respawn_cooldown = 0;
}

GAME_STATE PlayingState::handle(Game *game, float elapsed_time)
{
  // handle respawn
  if (is_respawning)
  {
    respawn_cooldown -= elapsed_time;
    if (respawn_cooldown < 0 && game->get_controls()->is_firing())
    {
      ESP_LOGI(TAG, "Respawn");
      is_respawning = false;
      game->add_player_ship();
    }
  }
  // check to see if the ship has been hit
  if (game->is_ship_hit())
  {
    game->destroy_player_ship();
    game->set_lives(game->get_lives() - 1);
    game->get_sound_fx()->bang_large();
    if (game->get_lives() == 0)
    {
      ESP_LOGI(TAG, "Switching to GAME over state");
      return GAME_STATE_GAME_OVER;
    }
    else
    {
      ESP_LOGI(TAG, "Allow respawn ship in 2 seconds");
      respawn_cooldown = 2;
      is_respawning = true;
    }
  }
  auto ship = game->get_ship();
  if (ship)
  {
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
    if (firing_cooldown > 0)
    {
      firing_cooldown -= elapsed_time;
    }
    if (game->get_controls()->is_firing() && firing_cooldown <= 0 && game->can_add_bullet())
    {
      game->add_bullet();
      // prevent firing for some time period
      firing_cooldown = FIRE_COOLDOWN;
      game->get_sound_fx()->fire();
    }
  }
  if (!game->has_asteroids())
  {
    game->increase_difficulty();
    // need to create new asteroids
    game->reset_player_ship();
    game->add_asteroids();
  }
  return GAME_STATE_PLAYING;
}

void PlayingState::exit(Game *game)
{
  // nothing to do
}