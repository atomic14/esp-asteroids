#include "GameLoop.h"
#include "Game.hpp"
#include "driver/timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "../Rendering/RenderBuffer.hpp"
#include "../Controls/Controls.hpp"

void IRAM_ATTR timer_group1_isr(void *param)
{
  timer_spinlock_take(TIMER_GROUP_1);
  GameLoop *game = static_cast<GameLoop *>(param);
  // Clear the interrupt
  timer_group_clr_intr_status_in_isr(TIMER_GROUP_1, TIMER_0);
  timer_group_enable_alarm_in_isr(TIMER_GROUP_1, TIMER_0);
  xTaskNotifyFromISR(game->game_task_handle, 1, eIncrement, NULL);
  timer_spinlock_give(TIMER_GROUP_1);
}

void game_task(void *param)
{
  GameLoop *game_loop = static_cast<GameLoop *>(param);

  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(100);
  float prev_time = esp_timer_get_time() / 1E6f;
  while (true)
  {
    uint32_t ulNotificationValue = ulTaskNotifyTake(pdTRUE, xMaxBlockTime);
    if (ulNotificationValue > 0)
    {
      // work out the elapsed time - it's good to keep this accurate so the physics simulation and any timers are correct
      float cur_time = esp_timer_get_time() / 1E6f;
      float elapsed_time = cur_time - prev_time;
      game_loop->game->stepWorld(elapsed_time);
      prev_time = cur_time;
      // re-render if we need to
      game_loop->render_buffer->render_if_needed(game_loop->game);
      game_loop->steps++;
    }
  }
}

void GameLoop::start()
{
  // start the world
  xTaskCreatePinnedToCore(game_task, "Game Loop", 4096, this, 1, &game_task_handle, 0);
  // set up the renderer timer
  timer_config_t config = {
      .alarm_en = TIMER_ALARM_EN,
      .counter_en = TIMER_PAUSE,
      .intr_type = TIMER_INTR_LEVEL,
      .counter_dir = TIMER_COUNT_UP,
      .auto_reload = TIMER_AUTORELOAD_EN,
      .divider = 4}; // default clock source is APB
  timer_init(TIMER_GROUP_1, TIMER_0, &config);

  // Timer's counter will initially start from value below.
  //   Also, if auto_reload is set, this value will be automatically reload on alarm
  timer_set_counter_value(TIMER_GROUP_1, TIMER_0, 0x00000000ULL);

  // Configure the alarm value and the interrupt on alarm.
  timer_set_alarm_value(TIMER_GROUP_1, TIMER_0, 333333ULL);
  timer_enable_intr(TIMER_GROUP_1, TIMER_0);
  timer_isr_register(TIMER_GROUP_1, TIMER_0, timer_group1_isr,
                     (void *)this, ESP_INTR_FLAG_IRAM, NULL);

  timer_start(TIMER_GROUP_1, TIMER_0);
}
