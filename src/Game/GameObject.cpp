//
//  GameObject.cpp
//  AsteroidsTestBed
//
//  Created by Chris Greening on 04/01/2021.
//

#include "GameObject.hpp"

GameObject::GameObject(GAME_OBJECTS objectType, const b2Vec2 *points, int numPoints, b2Vec2 position, float angle, float scale)
{
    age = 0;
    this->objectType = objectType;
    this->position = position;
    this->angle = angle;
    // set the shape of the body taking into account the scaling
    this->points = (b2Vec2 *)malloc(sizeof(b2Vec2) * numPoints);
    for (int i = 0; i < numPoints; i++)
    {
        this->points[i].Set(points[i].x * scale, points[i].y * scale);
    }
    this->numPoints = numPoints;
}

GameObject::~GameObject()
{
    free(this->points);
}

b2Vec2 *GameObject::getPoints()
{
    return points;
}

int GameObject::getNumPoints()
{
    return numPoints;
}

b2Vec2 GameObject::getPosition()
{
    return this->position;
}
float GameObject::getAngle()
{
    return this->angle;
}
void GameObject::setPosition(b2Vec2 position)
{
    this->position = position;
}
void GameObject::setAngle(float angle)
{
    this->angle = angle;
}
void GameObject::thrust(float value)
{
    // nothing to do
}
b2Vec2 GameObject::getLinearVelocity()
{
    return b2Vec2(0, 0);
}
