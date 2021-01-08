//
//  RenderBuffer.cpp
//  AsteroidsTestBed
//
//  Created by Chris Greening on 08/01/2021.
//

#include "RenderBuffer.hpp"
#include "box2d/box2d.h"
#include "GameObject.hpp"
#include "Game.hpp"
#include <list>

#define MAX_MOVE_LENGTH 2
#define MAX_LINE_LENGTH 1

RenderBuffer::RenderBuffer(int minX, int maxX, int minY, int maxY, int centerX, int centerY, float scale) {
    display_frame = new std::vector<DrawInstruction_t>(500);
    drawing_frame = new std::vector<DrawInstruction_t>(500);
    _minX=minX;
    _minY=minY;
    _maxX=maxX;
    _maxY=maxY;
    _centerX=centerX;
    _centerY=centerY;
    _scale=scale;
}

void RenderBuffer::renderSegment(bool laser, float maxLineLength, b2Vec2 start, const b2Vec2 &end)
{
    auto vector = end - start;
    auto length = vector.Length();
    auto invLength = 1.0f / length;
    auto step = invLength * vector;
    // start of the segment
    drawing_frame->push_back({.x = calc_x(start.x),
                              .y = calc_y(start.y),
                              .laser = laser});
    // move in steps
    while (length > maxLineLength)
    {
        length -= maxLineLength;
        start += maxLineLength * step;
        drawing_frame->push_back({.x = calc_x(start.x),
            .y = calc_y(start.y),
                                  .laser = laser});
    }
    // do the remaining line
    if (length > 0)
    {
        start += length * step;
        drawing_frame->push_back({.x = calc_x(start.x),
                                 .y = calc_y(start.y),
                                  .laser = laser});
    }
}

GameObject *removeNearest(b2Vec2 search_point, std::list<GameObject *> &objects)
{
    GameObject *nearest_object = NULL;
    float nearest_distance = FLT_MAX;
    for (auto object : objects)
    {
        auto distance = (object->getPosition() - search_point).LengthSquared();
        if (distance < nearest_distance)
        {
            nearest_distance = distance;
            nearest_object = object;
        }
    }
    objects.remove(nearest_object);
    return nearest_object;
}

void RenderBuffer::render_if_needed(Game *game)
{
    if (needs_render) {
        drawing_frame->clear();
        // assume we start from 0,0 - we might optimise this later to start from the last drawn location
        b2Vec2 cur(0, 0);
        std::list<GameObject *> objects_to_draw(game->getObjects());
        // while we still have objects to draw
        while (objects_to_draw.size() > 0)
        {
            // get the nearest object to the current search_point
            auto object = removeNearest(cur, objects_to_draw);
            auto numPoints = object->getNumPoints();
            auto points = object->getPoints();
            auto position = object->getPosition();
            auto angle = object->getAngle();
            auto c = cos(angle);
            auto s = sin(angle);
            // move to the start of the object
            renderSegment(false, MAX_MOVE_LENGTH, cur, points[0] + position);
            auto start = b2Vec2(points[0].x * c - points[0].y * s, points[0].x * s + points[0].y * c) + position;
            cur = start;
            for (int i = 0; i < numPoints; i++)
            {
                // draw each line segment
                auto p = b2Vec2(points[i].x * c - points[i].y * s, points[i].x * s + points[i].y * c) + position;
                renderSegment(true, MAX_LINE_LENGTH, cur, p);
                cur = p;
            }
            // close the line
            renderSegment(true, MAX_LINE_LENGTH, cur, start);
            cur = start;
        }
        // move back to 0,0 ready for the next draw pass
        renderSegment(false, MAX_MOVE_LENGTH, cur, b2Vec2(0, 0));
        // finished rendering
        needs_render = false;
    }
}


bool RenderBuffer::swapBuffers() {
    // check to see if the other buffer is ready
    if (needs_render) {
        // if it's not don't swap as there's nothing to swap to!
        return false;
    }
    std::swap(display_frame, drawing_frame);
    needs_render = true;
    return true;
}
