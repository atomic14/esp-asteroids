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

void SoundFX::fire(float volume_adjust)
{
  output->add_wav_file(fire_wav, volume_adjust * 0.5);
}
void SoundFX::thrust(float volume_adjust)
{
  output->add_wav_file(thrust_wav, volume_adjust * 0.5);
}
void SoundFX::bang_large(float volume_adjust)
{
  output->add_wav_file(bang_large_wav, volume_adjust * 0.5);
}
void SoundFX::bang_medium(float volume_adjust)
{
  output->add_wav_file(bang_medium_wav, volume_adjust * 0.5);
}
void SoundFX::bang_small(float volume_adjust)
{
  output->add_wav_file(bang_small_wav, volume_adjust * 0.5);
}