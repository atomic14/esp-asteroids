//
//  GameObject.hpp
//  AsteroidsTestBed
//
//  Created by Chris Greening on 04/01/2021.
//
#pragma once

#include "box2d/box2d.h"

typedef enum
{
    SHIP,
    ASTEROID,
    BULLET,
    HUD
} GAME_OBJECTS;

class GameObject
{
protected:
    b2Vec2 *points;
    GAME_OBJECTS objectType;
    int numPoints;
    float scale;
    float age;
    b2Vec2 position;
    float angle;

public:
    GameObject(GAME_OBJECTS objectType, const b2Vec2 *points, int numPoints, b2Vec2 position, float angle, float scale);
    virtual ~GameObject();
    GAME_OBJECTS getObjectType()
    {
        return objectType;
    }
    float getScale()
    {
        return scale;
    };
    void increaseAge(float delta)
    {
        age += delta;
    }
    void setAge(float age)
    {
        this->age = age;
    }
    float getAge()
    {
        return age;
    }
    virtual b2Vec2 getPosition();
    virtual float getAngle();
    virtual void setPosition(b2Vec2 position);
    virtual void setAngle(float angle);
    virtual void thrust(float value);
    virtual b2Vec2 getLinearVelocity();

    virtual b2Vec2 *getPoints();
    virtual int getNumPoints();
    void destroy()
    {
    }
};
