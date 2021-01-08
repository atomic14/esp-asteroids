#ifndef _esp32_game_h
#define _esp32_game_h

#include "Game.hpp"

typedef void *TaskHandle_t;

class ESP32Game : public Game
{
private:
  TaskHandle_t game_task_handle;

public:
  int steps;
  void start();
  friend void game_task(void *param);
  friend void timer_group1_isr(void *param);
};

#endif