#include "Renderer.h"
#include "../Game/Game.hpp"

Renderer::Renderer() : render_buffer(NULL)
{
  rendered_frames = 0;
}

void Renderer::start()
{
  // setup the laser output
  gpio_set_direction(PIN_NUM_LASER, GPIO_MODE_OUTPUT);
}

void IRAM_ATTR Renderer::set_laser(bool on)
{
  gpio_set_level(PIN_NUM_LASER, on ? 1 : 0);
}
