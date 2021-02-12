//
//  Game.hpp
//  AsteroidsTestBed
//
//  Created by Chris Greening on 04/01/2021.
//

#pragma once

#include <list>
#include <set>
#include <vector>
#include "GameStateMachine/GameState.hpp"
#include "box2d/box2d.h"

class b2World;
class GameObject;
class ShipObject;
class RenderBuffer;
class Controls;
class SoundFX;

class Game : public b2ContactListener
{
private:
    int score;
    int lives;
    bool _is_ship_hit;
    float asteroid_speed;
    b2World *world;
    ShipObject *ship;

    std::list<GameObject *> objects;
    std::list<GameObject *> bullets;
    std::list<GameObject *> lives_indicators;
    std::set<GameObject *> hitAsteroids;
    std::set<GameObject *> deadBullets;

    float size;
    SoundFX *sound_fx;
    Controls *controls;

    GAME_STATE current_game_state;
    GameState *current_game_state_handler;

    GameState *game_state_start_handler;
    GameState *game_over_state_handler;
    GameState *game_playing_state_handler;

    // world processing functions
    void process_bullets(float elapsed_time);
    void process_asteroids();
    void wrap_objects();

public:
    Game(float size, Controls *controls, SoundFX *sound_fx);
    void start_new_game();
    std::list<GameObject *> &getObjects()
    {
        return objects;
    }
    Controls *get_controls()
    {
        return this->controls;
    }
    SoundFX *get_sound_fx()
    {
        return this->sound_fx;
    }
    ShipObject *get_ship()
    {
        return this->ship;
    }
    void stepWorld(float elapsedTime);
    void reset();

    void add_bullet();
    bool has_asteroids();
    bool can_add_bullet();
    void increase_difficulty()
    {
        asteroid_speed += 2;
    }
    void add_asteroids();
    void add_player_ship();
    void destroy_player_ship();
    void add_lives();
    void reset_player_ship();

    int get_lives()
    {
        return this->lives;
    }
    void set_lives(int new_value);
    void set_score(int new_score);
    int get_score()
    {
        return score;
    }
    bool is_ship_hit()
    {
        return _is_ship_hit;
    }
    void clear_is_ship_hit()
    {
        _is_ship_hit = false;
    }
    bool show_score()
    {
        return current_game_state != GAME_STATE_START;
    }
    const char *get_main_text()
    {
        return current_game_state_handler->get_text();
    }

    void BeginContact(b2Contact *contact);
    void EndContact(b2Contact *contact);
};
