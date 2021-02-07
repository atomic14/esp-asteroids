#pragma once

#include "box2d/box2d.h"
#include "GameObject.hpp"

/**
 * An object that is backed by a box 2d body and can move around in the game world
 **/
class DynamicObject : public GameObject
{
protected:
  b2Body *body;
  b2World *world;

public:
  DynamicObject(b2World *world, GAME_OBJECTS objectType, const b2Vec2 *points, int numPoints, b2Vec2 position, float angle, float scale, b2Vec2 velocity, float angularVelocity);
  ~DynamicObject();
  b2Vec2 getPosition();
  float getAngle();
  void setPosition(const b2Vec2 position);
  void setAngle(float angle);
  void thrust(float value);
  b2Vec2 getLinearVelocity();
};