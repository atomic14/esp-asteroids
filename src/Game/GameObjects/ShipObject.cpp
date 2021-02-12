#include "ShipObject.hpp"

ShipObject::ShipObject(b2World *world, const b2Vec2 *shipPoints, int numShipPoints, const b2Vec2 *shipThrustPoints, int numShipThrustPoints, b2Vec2 position, float angle, float scale, b2Vec2 velocity, float angularVelocity)
    : DynamicObject(world, SHIP, shipPoints, numShipPoints, position, angle, scale, velocity, angularVelocity)
{
  // thrusting ship
  this->thrust_points = (b2Vec2 *)malloc(sizeof(b2Vec2) * numShipThrustPoints);
  for (int i = 0; i < numShipThrustPoints; i++)
  {
    this->thrust_points[i].Set(shipThrustPoints[i].x * scale, shipThrustPoints[i].y * scale);
  }
  this->thrust_num_points = numShipThrustPoints;
  is_thrusting = false;
}

b2Vec2 *ShipObject::getPoints()
{
  if (is_thrusting)
  {
    return thrust_points;
  }
  return GameObject::getPoints();
}
int ShipObject::getNumPoints()
{
  if (is_thrusting)
  {
    return thrust_num_points;
  }
  return GameObject::getNumPoints();
}