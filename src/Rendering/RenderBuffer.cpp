//
//  RenderBuffer.cpp
//  AsteroidsTestBed
//
//  Created by Chris Greening on 08/01/2021.
//

#include <stdint.h>
#include "RenderBuffer.hpp"
#include "box2d/box2d.h"
#include "../Game/GameObjects/GameObject.hpp"
#include "../Game/Game.hpp"
#include "../Fonts/HersheyFont.hpp"

#include <list>

RenderBuffer::RenderBuffer(int minX, int maxX, int minY, int maxY, int centerX, int centerY, float scale, Font *font)
{
    display_frame = new std::vector<DrawInstruction_t>(500);
    drawing_frame = new std::vector<DrawInstruction_t>(500);
    _minX = minX;
    _minY = minY;
    _maxX = maxX;
    _maxY = maxY;
    _centerX = centerX;
    _centerY = centerY;
    _scale = scale;
    _font = font;
}

void RenderBuffer::renderSegment(bool laser, b2Vec2 start, const b2Vec2 &end, int min_hold)
{
    auto vector = end - start;
    auto length = vector.Length();
    int16_t hold = min_hold + (int16_t)((std::min(20.0f, 2.5f * length))); // this is a bit of finger in the air fudge
    drawing_frame->push_back({.x = calc_x(start.x + vector.x),
                              .y = calc_y(start.y + vector.y),
                              .hold = hold,
                              .laser = laser});
}

b2Vec2 RenderBuffer::draw_text(b2Vec2 start, float x, float y, const char *text, bool measure)
{
    float left = FLT_MAX, right = -FLT_MAX, top = FLT_MAX, bottom = -FLT_MAX;

    float default_width = _font->get_default_width();
    float x_scale = _font->get_x_scale();
    float y_scale = _font->get_y_scale();
    while (*text != '\0')
    {
        const Character *character = _font->get_character(*text);
        if (character)
        {
            const std::vector<FontCoord_t> &commands = character->commands;
            // draw the character
            if (commands.size() > 0)
            {
                // move to the start of the character
                b2Vec2 next = b2Vec2(x + (commands[0].coords.x - character->left_pos) * x_scale, y + commands[0].coords.y * y_scale);
                left = std::min(left, next.x);
                right = std::max(right, next.x);
                top = std::min(top, next.y);
                bottom = std::max(bottom, next.y);
                if (!measure)
                {
                    renderSegment(false, start, next);
                }

                start = next;
                // draw the character
                for (int j = 0; j < commands.size(); j++)
                {
                    next = b2Vec2(x + (commands[j].coords.x - character->left_pos) * x_scale, y + commands[j].coords.y * y_scale);
                    left = std::min(left, next.x);
                    right = std::max(right, next.x);
                    top = std::min(top, next.y);
                    bottom = std::max(bottom, next.y);
                    if (!measure)
                    {
                        renderSegment(commands[j].draw, start, next);
                    }
                    start = next;
                }
            }
            x += (character->right_pos - character->left_pos) * x_scale;
        }
        else
        {
            x += default_width * x_scale;
        }
        left = std::min(left, x);
        right = std::max(right, x);
        text++;
    }
    if (measure)
    {
        return b2Vec2(right - left, bottom - top);
    }
    return start;
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
    if (needs_render)
    {
        drawing_frame->clear();
        // start from the top left corner
        b2Vec2 cur(-30, -30);
        // score
        if (game->show_score())
        {
            char tmp[100];
            sprintf(tmp, "%03d", game->get_score());
            cur = draw_text(cur, -30, -28, tmp, false);
        }
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
            auto start = b2Vec2(points[0].x * c - points[0].y * s, points[0].x * s + points[0].y * c) + position;
            renderSegment(false, cur, start);
            cur = start;
            for (int i = 0; i < numPoints; i++)
            {
                // draw each line segment
                auto p = b2Vec2(points[i].x * c - points[i].y * s, points[i].x * s + points[i].y * c) + position;
                renderSegment(true, cur, p);
                cur = p;
            }
        }
        // main text
        auto main_text = game->get_main_text();
        if (main_text)
        {
            b2Vec2 text_size = draw_text(cur, 0, 0, main_text, true);
            cur = draw_text(cur, 0.0f - text_size.x / 2.0f, 0.0f - text_size.y / 2.0f, main_text, false);
        }
        // move back to the top left corner ready for the next draw pass
        renderSegment(false, cur, b2Vec2(-30, -30));
        // finished rendering
        needs_render = false;
    }
}

bool RenderBuffer::swapBuffers()
{
    // check to see if the other buffer is ready
    if (needs_render)
    {
        // if it's not don't swap as there's nothing to swap to!
        return false;
    }
    std::swap(display_frame, drawing_frame);
    needs_render = true;
    return true;
}
