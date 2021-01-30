#pragma once

#include "driver/i2s.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <list>

class WAVFile;

typedef struct
{
    WAVFile *src;
    volatile int16_t play_position;
    float volume;
} Voice_t;

class I2SOutput
{
private:
    // I2S write task
    TaskHandle_t m_i2sWriterTaskHandle;
    // i2s writer queue
    QueueHandle_t m_i2sQueue;
    // voices
    std::list<Voice_t> voices;

public:
    void start();
    void add_wav_file(WAVFile *file, float volume);

    friend void i2sWriterTask(void *param);
};
