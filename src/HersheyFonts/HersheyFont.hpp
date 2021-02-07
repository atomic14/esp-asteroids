//
//  HersheyFont.hpp
//  HiershyFontTest
//
//  Created by Chris Greening on 05/02/2021.
//

// https://emergent.unpythonic.net/software/hershey

#ifndef HersheyFont_hpp
#define HersheyFont_hpp

#include <stdio.h>
#include <vector>

typedef struct
{
    int x;
    int y;
    bool draw;
} FontCoord_t;

typedef struct
{
    std::vector<FontCoord_t> commands;
    int left_pos;
    int right_pos;
} HersheyCharacter_t;

class HersheyFont
{
private:
    std::vector<HersheyCharacter_t> chars;

public:
    void read_from_file(const char *fname);
    void parse(const char *data);
    const std::vector<HersheyCharacter_t> &get_chars()
    {
        return chars;
    }
};

#endif /* HersheyFont_hpp */
