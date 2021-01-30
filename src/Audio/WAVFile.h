#pragma once

#include <stdint.h>
// extremely basic wav file parsing, only handles 11KHz, 8bit samples

class WAVFile
{
public:
    uint8_t *samples;
    int16_t number_samples;

public:
    WAVFile(const char *file_name);
    ~WAVFile();
    int16_t get_number_samples();
    uint8_t get_sample(int16_t position);
};
