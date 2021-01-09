//
//  Game.hpp
//  AsteroidsTestBed
//
//  Created by Chris Greening on 04/01/2021.
//

#ifndef Game_hpp
#define Game_hpp

#include <list>
#include <set>
#include <vector>
#include "box2d/box2d.h"

class b2World;
class GameObject;
class RenderBuffer;
class Controls;
class Game : public b2ContactListener
{
private:
    b2World *world;
    GameObject *ship;
    std::list<GameObject *> objects;
    std::list<GameObject *> bullets;
    std::set<GameObject *> hitAsteroids;
    std::set<GameObject *> deadBullets;
    int _size;
    float firing_cooldown;

public:
    Controls *controls;

    Game(Controls *controls) : controls(controls){};
    void createWorld(int size);
    void stepWorld(float elapsedTime);
    std::list<GameObject *> &getObjects() { return objects; }

    void BeginContact(b2Contact *contact);
    void EndContact(b2Contact *contact);
};

#endif /* Game_hpp */
