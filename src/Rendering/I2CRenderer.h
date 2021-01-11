#ifndef _i2c_renderer_h_
#define _i2c_renderer_h_

#include <esp_attr.h>
#include "Renderer.h"

class RenderBuffer;

class I2CRenderer : public Renderer
{
private:
  TaskHandle_t i2c_task_handle;
  void IRAM_ATTR draw();

public:
  I2CRenderer(float world_size);
  void start();
  friend void i2c_draw_task(void *param);
};

#endif