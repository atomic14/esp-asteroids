#ifndef _dac_renderer_h_
#define _dac_renderer_h_

#include "Renderer.h"

class RenderBuffer;

class DACRenderer : public Renderer
{
public:
  DACRenderer(RenderBuffer *render_buffer) : Renderer(render_buffer)
  {
  }
  void start();
  void stop();
  virtual void draw(const DrawInstruction_t &instruction);
};

#endif