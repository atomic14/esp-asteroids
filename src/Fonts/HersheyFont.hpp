//
//  HersheyFont.hpp
//  HiershyFontTest
//
//  Created by Chris Greening on 05/02/2021.
//

// https://emergent.unpythonic.net/software/hershey

#pragma once

#include <stdio.h>
#include <vector>
#include "Font.hpp"

class HersheyFont : public Font
{
private:
    std::vector<Character> chars;

public:
    void read_from_file(const char *fname);
    void parse(const char *data);
    const Character *get_character(char c);
    float get_x_scale()
    {
        return 0.15;
    }
    float get_y_scale()
    {
        return 0.15;
    }
    float get_default_width()
    {
        return 20;
    }
};
