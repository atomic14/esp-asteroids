//
//  GameObject.cpp
//  AsteroidsTestBed
//
//  Created by Chris Greening on 04/01/2021.
//

#include "GameObject.hpp"

GameObject::GameObject(GAME_OBJECTS objectType, b2World *world, const b2Vec2 *points, int numPoints, b2Vec2 position, float angle, b2Vec2 linearVelocity, float angularVelocity, float scale)
{
    age = 0;
    this->objectType = objectType;
    this->world = world;
    // create the body
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = position;
    bodyDef.angle = angle;
    bodyDef.angularVelocity = angularVelocity;
    bodyDef.linearVelocity = linearVelocity;
    bodyDef.userData.pointer = (uintptr_t)this;
    bodyDef.bullet = objectType == BULLET;
    body = world->CreateBody(&bodyDef);
    // set the shape of the body taking into account the scaling
    this->points = (b2Vec2 *)malloc(sizeof(b2Vec2) * numPoints);
    for (int i = 0; i < numPoints; i++)
    {
        this->points[i].Set(points[i].x * scale, points[i].y * scale);
    }
    this->numPoints = numPoints;
    // decompose the polygon into smaller ones to allow for box2d limits and concave polygons
    b2Vec2 subPoly[4];
    subPoly[0] = this->points[0];
    subPoly[3] = this->points[0];
    for (int i = 1; i < numPoints - 1; i++)
    {
        subPoly[1] = this->points[i];
        subPoly[2] = this->points[i + 1];
        b2PolygonShape shape;
        shape.Set(subPoly, 4);
        // create the fixture
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shape;
        fixtureDef.density = objectType == ASTEROID ? 10.0f : 1.0f;
        fixtureDef.friction = 1.0f;
        fixtureDef.restitution = 0.8f;
        fixtureDef.filter.groupIndex = objectType == ASTEROID ? 0 : -1;
        body->CreateFixture(&fixtureDef);
    }
}

GameObject::~GameObject()
{
    world->DestroyBody(body);
}
