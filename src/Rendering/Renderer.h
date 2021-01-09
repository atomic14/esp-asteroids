#ifndef _renderer_h_
#define _renderer_h_

#include <vector>
#include "RenderBuffer.hpp"

typedef void *TaskHandle_t;

#define PIN_NUM_LDAC GPIO_NUM_4
#define PIN_NUM_LASER GPIO_NUM_2

class Game;

class Renderer
{
private:
  int draw_position;
  RenderBuffer *render_buffer;

public:
  int requested_sends = 0;
  int send_success = 0;
  int output_calls = 0;
  int send_fail = 0;

  Renderer(RenderBuffer *render_buffer);

  virtual void start();
  virtual void stop();

  // override this in derived classes with logic to tigger the draw method
  // For example
  // - the DAC renderer just calls the _draw function directly
  // - the SPI renderer triggers a task that calls the _draw function as it's got to a lot more work
  virtual void trigger_draw() = 0;
  // draw and move to the next sample
  void draw();
  // do the actual drawing of a sample - override in derived classes to do the actual work
  virtual void draw_sample(const DrawInstruction_t &instruction) = 0;

  friend void draw_timer(void *para);
};

#endif