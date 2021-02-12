
#include "driver/i2s.h"
#include <math.h>
#include "I2SOutput.h"
#include "WAVFile.h"

// number of frames to try and send at once (a frame is a left and right sample)
#define NUM_FRAMES_TO_SEND 64

typedef struct
{
    int16_t left;
    int16_t right;
} Frame_t;

void i2sWriterTask(void *param)
{
    I2SOutput *output = (I2SOutput *)param;
    volatile int availableBytes = 0;
    volatile int buffer_position = 0;
    Frame_t *frames = (Frame_t *)malloc(sizeof(Frame_t) * NUM_FRAMES_TO_SEND);
    while (true)
    {
        // wait for some data to be requested
        i2s_event_t evt;
        if (xQueueReceive(output->m_i2sQueue, &evt, portMAX_DELAY) == pdPASS)
        {
            if (evt.type == I2S_EVENT_TX_DONE)
            {
                size_t bytesWritten = 0;
                do
                {
                    // we've finished sending our buffer - time to create a new one
                    if (availableBytes == 0)
                    {
                        // go through each voice and add samples until we have filled up our buffer
                        for (int i = 0; i < NUM_FRAMES_TO_SEND; i++)
                        {
                            float sample = 0;
                            for (auto &voice : output->voices)
                            {
                                if (voice.play_position < voice.src->get_number_samples())
                                {
                                    sample += voice.volume * (voice.src->get_sample(voice.play_position) - 128.0f) / 128.0f;
                                    voice.play_position++;
                                }
                            }
                            // apply clipping
                            sample = tanhf(sample);
                            // output it
                            frames[i].left = sample * 16383;
                            frames[i].right = sample * 16383;
                            // frames[i].left = (i % 100 < 50) ? 1000 : -1000;
                            // frames[i].right = (i % 100 < 50) ? 1000 : -1000;
                        }
                        // how many bytes do we now have to send
                        availableBytes = NUM_FRAMES_TO_SEND * sizeof(Frame_t);
                        // reset the buffer position back to the start
                        buffer_position = 0;
                    }
                    // do we have something to write?
                    if (availableBytes > 0)
                    {
                        // write data to the i2s peripheral
                        i2s_write(I2S_NUM_1, buffer_position + (uint8_t *)frames,
                                  availableBytes, &bytesWritten, portMAX_DELAY);
                        availableBytes -= bytesWritten;

                        buffer_position += bytesWritten;
                    }
                } while (bytesWritten > 0);
            }
        }
    }
}

void I2SOutput::start()
{
    i2s_pin_config_t i2sPins = {
        .bck_io_num = GPIO_NUM_22,
        .ws_io_num = GPIO_NUM_23,
        .data_out_num = GPIO_NUM_21,
        .data_in_num = -1};
    // i2s config for writing both channels of I2S
    i2s_config_t i2sConfig = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = 11025,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S),
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = 64,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0};

    //install and start i2s driver
    i2s_driver_install(I2S_NUM_1, &i2sConfig, 4, &m_i2sQueue);
    // set up the i2s pins
    i2s_set_pin(I2S_NUM_1, &i2sPins);
    // clear the DMA buffers
    i2s_zero_dma_buffer(I2S_NUM_1);
    // start a task to write samples to the i2s peripheral
    TaskHandle_t writerTaskHandle;
    xTaskCreate(i2sWriterTask, "i2s Writer Task", 4096, this, 2, &writerTaskHandle);
}

void I2SOutput::add_wav_file(WAVFile *wav_file, float volume)
{
    // try and find a free slot in existing voices
    for (auto &voice : voices)
    {
        if (voice.play_position == voice.src->get_number_samples())
        {
            voice.src = wav_file;
            voice.play_position = 0;
            voice.volume = volume;
            return;
        }
    }
    voices.push_back({.src = wav_file,
                      .play_position = 0,
                      .volume = volume});
}
