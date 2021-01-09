//
//  GameObject.hpp
//  AsteroidsTestBed
//
//  Created by Chris Greening on 04/01/2021.
//

#ifndef GameObject_hpp
#define GameObject_hpp

#include "box2d/box2d.h"

typedef enum {
    SHIP, ASTEROID, BULLET
} GAME_OBJECTS;

class GameObject {
    b2Body *body;
    b2Vec2 *points;
    b2World *world;
    GAME_OBJECTS objectType;
    int numPoints;
    float scale;
    float age;
public:
    GameObject(GAME_OBJECTS objectType, b2World *world, const b2Vec2 *points, int numPoints, b2Vec2 position, float angle, b2Vec2 velocity, float angularVelocity, float scale);
    ~GameObject();
    GAME_OBJECTS getObjectType() {
        return objectType;
    }
    b2Vec2 getPosition() {
        return body->GetPosition();
    }
    float getAngle() {
        return body->GetAngle();
    }
    float getScale() {
        return scale;
    };
    void setPosition(b2Vec2 position) {
        body->SetTransform(position, getAngle());
    }
    void setAngle(float angle) {
        body->SetAngularVelocity(0); // won't need this in the final version
        body->SetTransform(getPosition(), angle);
    }
    void thrust(float value) {
        body->ApplyLinearImpulseToCenter(-value * b2Vec2(cos(M_PI_2 + body->GetAngle()), sin(M_PI_2 + body->GetAngle())), true);
    }
    b2Vec2 getLinearVelocity() {
        return body->GetLinearVelocity();
    }
    void increaseAge(float delta) {
        age+=delta;
    }
    void setAge(float age) {
        this->age = age;
    }
    float getAge() {
        return age;
    }
    b2Vec2 *getPoints() {
        return points;
    }
    int getNumPoints() {
        return numPoints;
    }
    void destroy() {
        
    }
};

#endif /* GameObject_hpp */
