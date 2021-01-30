#include "WAVFile.h"
#include <stdio.h>
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"

static const char *TAG = "WAV";

// basic wav file header
#pragma pack(push, 1)
typedef struct
{
    // RIFF Header
    char riff_header[4]; // Contains "RIFF"
    int wav_size;        // Size of the wav portion of the file, which follows the first 8 bytes. File size - 8
    char wave_header[4]; // Contains "WAVE"

    // Format Header
    char fmt_header[4]; // Contains "fmt " (includes trailing space)
    int fmt_chunk_size; // Should be 16 for PCM
    short audio_format; // Should be 1 for PCM. 3 for IEEE Float
    short num_channels;
    int sample_rate;
    int byte_rate;          // Number of bytes per second. sample_rate * num_channels * Bytes Per Sample
    short sample_alignment; // num_channels * Bytes Per Sample
    short bit_depth;        // Number of bits per sample

    // Data
    char data_header[4]; // Contains "data"
    int data_bytes;      // Number of bytes in data. Number of samples * num_channels * sample byte size
    // uint8_t bytes[]; // Remainder of wave file is bytes
} wav_header_t;
#pragma pack(pop)

WAVFile::WAVFile(const char *file_name)
{
    number_samples = 0;
    samples = NULL;
    FILE *fp = fopen(file_name, "rb");
    if (!fp)
    {
        ESP_LOGE(TAG, "Cannot open file %s", file_name);
    }
    else
    {
        // read the WAV header
        wav_header_t wav_header;
        fread(&wav_header, sizeof(wav_header_t), 1, fp);
        // sanity check the bit depth
        if (wav_header.bit_depth != 8)
        {
            ESP_LOGE(TAG, "ERROR: Expecting 8 bits depth, got %d", wav_header.bit_depth);
        }
        else
        {
            if (wav_header.num_channels != 1)
            {
                ESP_LOGE(TAG, "ERROR: Expecting 1 channel, got %d", wav_header.num_channels);
            }
            else
            {
                if (wav_header.sample_rate != 11025)
                {
                    ESP_LOGE(TAG, "ERROR: Expecting 11KHz sample rate, got %d", wav_header.sample_rate);
                }
                samples = (uint8_t *)malloc(wav_header.data_bytes);
                number_samples = wav_header.data_bytes;
                fread(samples, 1, wav_header.data_bytes, fp);
                printf("Read %d samples from %s\n", number_samples, file_name);
            }
        }
        fclose(fp);
    }
}

int16_t WAVFile::get_number_samples() { return number_samples; }
uint8_t WAVFile::get_sample(int16_t position) { return samples[position]; }

WAVFile::~WAVFile()
{
    free(samples);
}
