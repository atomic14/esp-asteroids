#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Renderer.h"
#include "driver/timer.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "Game.hpp"

void IRAM_ATTR draw_timer(void *para)
{
  timer_spinlock_take(TIMER_GROUP_0);
  Renderer *renderer = static_cast<Renderer *>(para);

  // uint32_t timer_intr = timer_group_get_intr_status_in_isr(TIMER_GROUP_0);
  // uint64_t timer_counter_value = timer_group_get_counter_value_in_isr(TIMER_GROUP_0, TIMER_0);

  renderer->renders++;

  // Clear the interrupt
  timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);

  // After the alarm has been triggered
  //     we need enable it again, so it is triggered the next time
  timer_group_enable_alarm_in_isr(TIMER_GROUP_0, TIMER_0);

  xTaskNotifyFromISR(renderer->draw_task_handle, 1, eIncrement, NULL);

  timer_spinlock_give(TIMER_GROUP_0);
}

void IRAM_ATTR draw_task(void *param)
{
  Renderer *renderer = static_cast<Renderer *>(param);

  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(100);
  while (true)
  {
    uint32_t ulNotificationValue = ulTaskNotifyTake(pdTRUE, xMaxBlockTime);
    if (ulNotificationValue > 0)
    {
      renderer->_draw();
    }
  }
}

void IRAM_ATTR Renderer::_draw()
{
  if (draw_position < display_buffer1->size())
  {
    draw((*display_buffer1)[draw_position]);
    draw_position++;
  }
  if (draw_position >= display_buffer1->size())
  {
    if (!game->needs_render)
    {
      std::swap(display_buffer1, display_buffer2);
      game->render_buffer = display_buffer2;
      game->needs_render = true;
    }
    draw_position = 0;
  }
}

Renderer::Renderer(Game *game) : game(game)
{
  // stats
  renders = 0;
  sample_send_success = 0;
  ldac_calls = 0;
  sample_send_fail = 0;
  // render instructions
  display_buffer1 = new std::vector<DrawInstruction_t>();
  display_buffer2 = new std::vector<DrawInstruction_t>();
  game->render_buffer = display_buffer2;
  game->needs_render = true;
  // current drawing position
  draw_position = 0;
}

void Renderer::start()
{

  // setup the LDAC output
  gpio_set_direction(PIN_NUM_LDAC, GPIO_MODE_OUTPUT);
  // setup the laser output
  gpio_set_direction(PIN_NUM_LASER, GPIO_MODE_OUTPUT);

  // setup the task for sending samples
  xTaskCreatePinnedToCore(draw_task, "Draw Task", 1024, this, 10, &draw_task_handle, 1);

  // set up the renderer timer
  timer_config_t config = {
      .alarm_en = TIMER_ALARM_EN,
      .counter_en = TIMER_PAUSE,
      .intr_type = TIMER_INTR_LEVEL,
      .counter_dir = TIMER_COUNT_UP,
      .auto_reload = TIMER_AUTORELOAD_EN,
      .divider = 8000}; // default clock source is APB
  timer_init(TIMER_GROUP_0, TIMER_0, &config);

  // Timer's counter will initially start from value below.
  //   Also, if auto_reload is set, this value will be automatically reload on alarm
  timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0x00000000ULL);

  // Configure the alarm value and the interrupt on alarm.
  timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 0x00000001ULL);
  timer_enable_intr(TIMER_GROUP_0, TIMER_0);
  timer_isr_register(TIMER_GROUP_0, TIMER_0, draw_timer,
                     (void *)this, ESP_INTR_FLAG_IRAM, NULL);

  timer_start(TIMER_GROUP_0, TIMER_0);
}

void Renderer::stop()
{
  timer_disable_intr(TIMER_GROUP_0, TIMER_0);
  timer_deinit(TIMER_GROUP_0, TIMER_0);
}
