#ifndef _dac_renderer_h_
#define _dac_renderer_h_

#include "Renderer.h"

class RenderBuffer;

class DACRenderer : public Renderer
{
private:
  int draw_position;
  void IRAM_ATTR draw();

public:
  DACRenderer(float world_size);
  void start();
  friend void draw_timer(void *para);
};

#endif