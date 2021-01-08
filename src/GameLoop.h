#ifndef _esp32_game_h
#define _esp32_game_h

typedef void *TaskHandle_t;

class RenderBuffer;
class Game;

class GameLoop
{
private:
  TaskHandle_t game_task_handle;
  Game *game;
  RenderBuffer *render_buffer;

public:
  GameLoop(Game *game, RenderBuffer *render_buffer) : game(game), render_buffer(render_buffer), steps(0) {}
  int steps;
  void start();
  friend void game_task(void *param);
  friend void timer_group1_isr(void *param);
};

#endif