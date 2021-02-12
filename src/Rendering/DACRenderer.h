#ifndef _dac_renderer_h_
#define _dac_renderer_h_

#include "Renderer.h"

class RenderBuffer;
class Font;

class DACRenderer : public Renderer
{
private:
  volatile int draw_position;
  void IRAM_ATTR draw();

public:
  DACRenderer(float world_size, Font *font);
  void start();
  friend void draw_timer(void *para);
};

#endif