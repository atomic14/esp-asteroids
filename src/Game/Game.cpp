//
//  Game.cpp
//  AsteroidsTestBed
//
//  Created by Chris Greening on 04/01/2021.
//

#include <stdio.h>
#include "Game.hpp"
#include "box2d/box2d.h"
#include "DynamicObject.hpp"
#include "ShipObject.hpp"
#include "../Controls/Controls.hpp"
#include "../Audio/SoundFX.h"
#include "Shapes.hpp"
#include "GameStateMachine/PlayingState.hpp"
#include "GameStateMachine/GameOverState.hpp"
#include "GameStateMachine/StartState.hpp"
#include <set>

#define MAX_BULLETS_INFLIGHT 10
#define BULLET_MAX_AGE 1

Game::Game(float size, Controls *controls, SoundFX *sound_fx)
{
    this->size = size;
    this->sound_fx = sound_fx;
    this->controls = controls;
    this->current_game_state = GAME_STATE_START;
    // no gravity in our world
    b2Vec2 gravity(0.0f, 0.0f);
    this->world = new b2World(gravity);
    this->world->SetContactListener(this);
    this->asteroid_speed = 10.0;

    game_state_start_handler = new StartState();
    game_over_state_handler = new GameOverState();
    game_playing_state_handler = new PlayingState();

    current_game_state_handler = game_state_start_handler;
    this->score = 0;
}

void Game::reset()
{
    // clean up any old objects that we might have
    for (auto object : objects)
    {
        object->destroy();
        delete object;
    }
    objects.clear();
    hitAsteroids.clear();
    bullets.clear();
    deadBullets.clear();
}

void Game::add_player_ship()
{
    // create the objects for our game
    ship = new ShipObject(world, shipPoints, shipPointsCount, shipThrustPoints, shipThrustPointsCount, b2Vec2(0, 0), 0, 5, b2Vec2(0, 0), 0);
    objects.push_back(ship);
}

void Game::add_lives()
{
    // the three lives
    objects.push_back(new GameObject(HUD, shipPoints, shipPointsCount, b2Vec2(size - 1.5, -size + 1.5), 0, 3));
    objects.push_back(new GameObject(HUD, shipPoints, shipPointsCount, b2Vec2(size - 4, -size + 1.5), 0, 3));
    objects.push_back(new GameObject(HUD, shipPoints, shipPointsCount, b2Vec2(size - 6.5, -size + 1.5), 0, 3));
}

void Game::set_score(int new_score)
{
    this->score = new_score;
}

bool Game::can_add_bullet()
{
    return bullets.size() < MAX_BULLETS_INFLIGHT;
}

bool Game::has_asteroids()
{
    // check if there are any asteroids left
    for (auto obj : objects)
    {
        if (obj->getObjectType() == ASTEROID)
        {
            return true;
        }
    }
    return false;
}

void Game::add_asteroids()
{
    objects.push_back(new DynamicObject(world, ASTEROID, asteroid1Points, asteroid1PointsCount, b2Vec2(25, 25), 0, 10, b2Vec2(asteroid_speed, asteroid_speed), 0));
    printf("Created asteroid1\n");
    objects.push_back(new DynamicObject(world, ASTEROID, asteroid2Points, asteroid2PointsCount, b2Vec2(-25, -25), 0, 10, b2Vec2(-asteroid_speed, asteroid_speed), 0));
    printf("Created asteroid2\n");
    objects.push_back(new DynamicObject(world, ASTEROID, asteroid3Points, asteroid3PointsCount, b2Vec2(-25, 0), 0, 10, b2Vec2(-asteroid_speed, -asteroid_speed), 0));
    printf("Created asteroid3\n");
}

void Game::add_bullet()
{
    // create a new bullet and add it to the game
    DynamicObject *bullet = new DynamicObject(world, BULLET, bulletPoints, bulletPointsCount, ship->getPosition(), M_PI + ship->getAngle(), 1.5, -80 * b2Vec2(cos(M_PI_2 + ship->getAngle()), sin(M_PI_2 + ship->getAngle())), 0);

    objects.push_back(bullet);
    bullets.push_back(bullet);
}

void Game::wrap_objects()
{
    for (auto object : this->objects)
    {
        auto position = object->getPosition();
        if (position.x < -size)
        {
            position.x = size;
        }
        else if (position.x > size)
        {
            position.x = -size;
        }
        if (position.y < -size)
        {
            position.y = size;
        }
        else if (position.y > size)
        {
            position.y = -size;
        }
        object->setPosition(position);
    }
}

void Game::process_bullets(float elapsed_time)
{
    // age bullets so that they dissappear
    for (auto bullet : bullets)
    {
        bullet->increaseAge(elapsed_time);
        if (bullet->getAge() > BULLET_MAX_AGE)
        {
            deadBullets.insert(bullet);
        }
    }
    // remove any dead bullets
    for (auto bullet : deadBullets)
    {
        objects.remove(bullet);
        bullets.remove(bullet);
        delete bullet;
    }
    deadBullets.clear();
}

void Game::process_asteroids()
{
    // remove any hit asteroids
    for (auto asteroid : hitAsteroids)
    {
        if (asteroid->getAge() < 1)
        {
            sound_fx->bang_large();
        }
        else if (asteroid->getAge() < 2)
        {
            sound_fx->bang_medium();
        }
        else
        {
            sound_fx->bang_small();
        }
        // add any child asteroids
        if (asteroid->getAge() < 2)
        {
            auto position = asteroid->getPosition();
            auto scale = 10.0 / (asteroid->getAge() + 2);
            auto linearVelocity = asteroid->getLinearVelocity();
            {
                auto angle = atan2(linearVelocity.y, linearVelocity.x) + M_PI_2;
                // printf("Angle %f %f = %f\n", linearVelocity.y, linearVelocity.x, angle);
                auto newAsteroid = new DynamicObject(
                    world,
                    ASTEROID,
                    asteroid1Points,
                    asteroid1PointsCount,
                    b2Vec2(position.x + scale * cos(angle),
                           position.y + scale * sin(angle)),
                    0,
                    scale,
                    b2Vec2(asteroid_speed * cos(angle), 10 * sin(angle)),
                    0);
                newAsteroid->setAge(asteroid->getAge() + 1);
                objects.push_back(newAsteroid);
            }
            {
                auto angle = atan2(linearVelocity.y, linearVelocity.x) - M_PI_2;
                // printf("Angle %f %f = %f\n", linearVelocity.y, linearVelocity.x, angle);
                auto newAsteroid = new DynamicObject(
                    world,
                    ASTEROID,
                    asteroid2Points,
                    asteroid2PointsCount,
                    b2Vec2(position.x + scale * cos(angle),
                           position.y + scale * sin(angle)),
                    0,
                    scale,
                    b2Vec2(10 * cos(angle), 10 * sin(angle)),
                    0);
                newAsteroid->setAge(asteroid->getAge() + 1);
                objects.push_back(newAsteroid);
            }
        }
        // and remove the dead asteroid
        objects.remove(asteroid);
        delete asteroid;
    }
    hitAsteroids.clear();
}

void Game::stepWorld(float elapsedTime)
{
    // step the world forward
    world->Step(elapsedTime, 6, 2);
    // wrap objects around the screen
    wrap_objects();
    // handle any bullets
    process_bullets(elapsedTime);
    // handle any asteroids
    process_asteroids();
    // handle the current game state
    GAME_STATE next_state = current_game_state_handler->handle(this, elapsedTime);
    // are we transitioning to a new state?
    if (next_state != current_game_state)
    {
        // yes we are so tell the current state to clean up
        current_game_state_handler->exit(this);
        // update to the new state handler
        switch (next_state)
        {
        case GAME_STATE_START:
            current_game_state_handler = game_state_start_handler;
            break;
        case GAME_STATE_GAME_OVER:
            current_game_state_handler = game_over_state_handler;
            break;
        case GAME_STATE_PLAYING:
            current_game_state_handler = game_playing_state_handler;
            break;
        }
        current_game_state = next_state;
        // tell the new handler to start up
        current_game_state_handler->enter(this);
    }
}

// Processs contact events from the world
// We're only interested in collisions between asteroids and bullets, and between asteroids and the ship
void Game::BeginContact(b2Contact *contact)
{
    GameObject *objA = reinterpret_cast<GameObject *>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
    GameObject *objB = reinterpret_cast<GameObject *>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);
    // Asteroid and bullet collisions - need to check both sides as the order is random
    if (objA->getObjectType() == BULLET && objB->getObjectType() == ASTEROID)
    {
        deadBullets.insert(objA);
        hitAsteroids.insert(objB);
    }
    if (objA->getObjectType() == ASTEROID && objB->getObjectType() == BULLET)
    {
        deadBullets.insert(objB);
        hitAsteroids.insert(objA);
    }
    // Asteroid and ship collision
    if ((objA->getObjectType() == SHIP && objB->getObjectType() == ASTEROID) ||
        (objA->getObjectType() == ASTEROID && objB->getObjectType() == SHIP))
    {
        // TODO
    }
}

void Game::EndContact(b2Contact *contact)
{
    // nothing to do here
}
