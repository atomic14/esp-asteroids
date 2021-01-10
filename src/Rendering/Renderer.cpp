#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Renderer.h"
#include "driver/timer.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "../Game/Game.hpp"

void IRAM_ATTR draw_timer(void *para)
{
  timer_spinlock_take(TIMER_GROUP_0);
  Renderer *renderer = static_cast<Renderer *>(para);
  renderer->requested_sends++;
  // Clear the interrupt
  timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);
  // After the alarm has been triggered we need enable it again, so it is triggered the next time
  timer_group_enable_alarm_in_isr(TIMER_GROUP_0, TIMER_0);
  // tell the drawing task to do something
  renderer->trigger_draw();
  timer_spinlock_give(TIMER_GROUP_0);
}

void IRAM_ATTR Renderer::draw()
{
  // do we still have things to draw?
  if (draw_position < render_buffer->display_frame->size())
  {
    // draw the next sample
    draw_sample((*render_buffer->display_frame)[draw_position]);
    draw_position++;
  }
  else
  {
    // trigger a re-render
    render_buffer->swapBuffers();
    draw_position = 0;
  }
}

Renderer::Renderer() : render_buffer(NULL)
{
  // stats
  requested_sends = 0;
  send_success = 0;
  output_calls = 0;
  send_fail = 0;
  // current drawing position
  draw_position = 0;
}

void Renderer::start()
{
  // setup the LDAC output
  gpio_set_direction(PIN_NUM_LDAC, GPIO_MODE_OUTPUT);
  // setup the laser output
  gpio_set_direction(PIN_NUM_LASER, GPIO_MODE_OUTPUT);

  // set up the renderer timer
  timer_config_t config = {
      .alarm_en = TIMER_ALARM_EN,
      .counter_en = TIMER_PAUSE,
      .intr_type = TIMER_INTR_LEVEL,
      .counter_dir = TIMER_COUNT_UP,
      .auto_reload = TIMER_AUTORELOAD_EN,
      .divider = 4000}; // default clock source is APB
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
