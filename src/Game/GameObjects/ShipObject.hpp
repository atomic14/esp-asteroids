#pragma once

#include "DynamicObject.hpp"

class ShipObject : public DynamicObject
{
private:
  b2Vec2 *thrust_points;
  int thrust_num_points;
  bool is_thrusting;

public:
  ShipObject(b2World *world, const b2Vec2 *shipPoints, int numShipPoints, const b2Vec2 *shipThrustPoints, int numShipThrustPoints, b2Vec2 position, float angle, float scale, b2Vec2 velocity, float angularVelocity);
  b2Vec2 *getPoints();
  int getNumPoints();
  void set_is_thrusting(bool is_thrusting)
  {
    this->is_thrusting = is_thrusting;
  }
};