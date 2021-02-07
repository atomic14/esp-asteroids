#pragma once

class I2SOutput;
class WAVFile;

class SoundFX
{
  WAVFile *bang_large_wav;
  WAVFile *bang_medium_wav;
  WAVFile *bang_small_wav;
  WAVFile *fire_wav;
  WAVFile *thrust_wav;
  I2SOutput *output;

public:
  SoundFX(I2SOutput *output);
  void fire();
  void thrust();
  void bang_large();
  void bang_medium();
  void bang_small();
};