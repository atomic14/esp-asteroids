#ifndef _renderer_h_
#define _renderer_h_

#include "driver/gpio.h"
#include <vector>
#include "RenderBuffer.hpp"

#define PIN_NUM_LDAC GPIO_NUM_4
#define PIN_NUM_LASER GPIO_NUM_2

class Game;

class Renderer
{
protected:
  RenderBuffer *render_buffer;

public:
  Renderer();

  virtual void start() = 0;
  void IRAM_ATTR set_laser(bool on);
  RenderBuffer *get_render_buffer() { return render_buffer; }

  volatile int rendered_frames;
  volatile int transactions;
};

#endif