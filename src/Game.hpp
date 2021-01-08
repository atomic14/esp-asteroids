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
#include "Drawing.hpp"

class b2World;
class GameObject;
class Game : public b2ContactListener
{
private:
    b2World *world;
    GameObject *ship;
    std::list<GameObject *> objects;
    std::list<GameObject *> bullets;
    std::set<GameObject *> hitAsteroids;
    std::set<GameObject *> deadBullets;

public:
    volatile bool needs_render = false;
    std::vector<DrawInstruction_t> *render_buffer = NULL;

    void createWorld();
    void stepWorld(float elapsedTime);
    std::list<GameObject *> &getObjects() { return objects; }

    void spinLeft();
    void spinRight();
    void thrust();
    void fire();

    void BeginContact(b2Contact *contact);
    void EndContact(b2Contact *contact);

    void render();
    void renderSegment(bool laser, float maxLineLength, b2Vec2 start, const b2Vec2 &end);
};

#endif /* Game_hpp */
