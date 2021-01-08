#ifndef _renderer_h_
#define _renderer_h_

#include <vector>
#include "Drawing.hpp"

typedef void *TaskHandle_t;

#define PIN_NUM_LDAC GPIO_NUM_2
#define PIN_NUM_LASER GPIO_NUM_4

class Game;

class Renderer
{
private:
  int draw_position;
  std::vector<DrawInstruction_t> *display_buffer1 = NULL;
  std::vector<DrawInstruction_t> *display_buffer2 = NULL;
  Game *game;

  void _draw();

public:
  TaskHandle_t draw_task_handle;
  int renders = 0;
  int sample_send_success = 0;
  int ldac_calls = 0;
  int sample_send_fail = 0;

  Renderer(Game *game);

  virtual void start();
  virtual void stop();
  virtual void draw(const DrawInstruction_t &instruction) = 0;
  friend void draw_timer(void *para);
  friend void draw_task(void *param);
};

#endif