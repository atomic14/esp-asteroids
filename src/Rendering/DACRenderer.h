#ifndef _dac_renderer_h_
#define _dac_renderer_h_

#include "Renderer.h"

class RenderBuffer;

class DACRenderer : public Renderer
{

public:
  DACRenderer(float world_size);
  void start();
  void stop();

  void IRAM_ATTR trigger_draw();
  void IRAM_ATTR draw_sample(const DrawInstruction_t &instruction);
};

#endif