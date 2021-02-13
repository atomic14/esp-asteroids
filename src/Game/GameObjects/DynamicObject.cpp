#include "DynamicObject.hpp"

DynamicObject::DynamicObject(b2World *world, GAME_OBJECTS objectType, const b2Vec2 *points, int numPoints, b2Vec2 position, float angle, float scale, b2Vec2 velocity, float angularVelocity)
    : GameObject(objectType, points, numPoints, position, angle, scale)
{
  // create the body
  this->world = world;
  b2BodyDef bodyDef;
  bodyDef.type = b2_dynamicBody;
  bodyDef.position = position;
  bodyDef.angle = angle;
  bodyDef.angularVelocity = angularVelocity;
  bodyDef.linearVelocity = velocity;
  bodyDef.userData.pointer = (uintptr_t)this;
  bodyDef.bullet = objectType == BULLET;
  body = world->CreateBody(&bodyDef);
  // decompose the polygon into smaller ones to allow for box2d limits and concave polygons
  b2Vec2 subPoly[4];
  subPoly[0] = this->points[0];
  subPoly[3] = this->points[0];
  for (int i = 1; i < numPoints - 2; i++)
  {
    subPoly[1] = this->points[i];
    subPoly[2] = this->points[i + 1];
    b2PolygonShape shape;
    shape.Set(subPoly, 4);
    // create the fixture
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = objectType == ASTEROID ? 20.0f : 1.0f;
    fixtureDef.friction = 0.1f;
    fixtureDef.restitution = 1.0f;
    fixtureDef.filter.groupIndex = objectType == ASTEROID ? 0 : -1;
    body->CreateFixture(&fixtureDef);
  }
}

DynamicObject::~DynamicObject()
{
  world->DestroyBody(body);
}

b2Vec2 DynamicObject::getPosition()
{
  return body->GetPosition();
}
float DynamicObject::getAngle()
{
  return body->GetAngle();
}
void DynamicObject::setPosition(b2Vec2 position)
{
  body->SetTransform(position, getAngle());
}
void DynamicObject::setAngle(float angle)
{
  body->SetAngularVelocity(0); // won't need this in the final version
  body->SetTransform(getPosition(), angle);
}
void DynamicObject::thrust(float value)
{
  body->ApplyLinearImpulseToCenter(-value * b2Vec2(cos(M_PI_2 + body->GetAngle()), sin(M_PI_2 + body->GetAngle())), true);
}
b2Vec2 DynamicObject::getLinearVelocity()
{
  return body->GetLinearVelocity();
}
