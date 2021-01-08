#ifndef _dac_renderer_h_
#define _dac_renderer_h_

#include "Renderer.h"

class DACRenderer : public Renderer
{
public:
  DACRenderer(Game *game) : Renderer(game)
  {
  }
  void start();
  void stop();
  virtual void draw(const DrawInstruction_t &instruction);
};

#endif