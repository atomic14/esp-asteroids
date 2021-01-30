#include "SoundFX.h"
#include "WAVFile.h"
#include "I2SOutput.h"

SoundFX::SoundFX(I2SOutput *output) : output(output)
{
  bang_large_wav = new WAVFile("/spiffs/bang_large.wav");
  bang_medium_wav = new WAVFile("/spiffs/bang_medium.wav");
  bang_small_wav = new WAVFile("/spiffs/bang_small.wav");
  fire_wav = new WAVFile("/spiffs/fire.wav");
  thrust_wav = new WAVFile("/spiffs/thrust.wav");
}

void SoundFX::fire()
{
  output->add_wav_file(fire_wav, 0.5);
}
void SoundFX::bang_large()
{
  output->add_wav_file(bang_large_wav, 4.0);
}
void SoundFX::bang_medium()
{
  output->add_wav_file(bang_medium_wav, 2.0);
}
void SoundFX::bang_small()
{
  output->add_wav_file(bang_small_wav, 1.0);
}