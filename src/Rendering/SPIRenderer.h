#ifndef _spi_renderer_h_
#define _spi_renderer_h_

#include <esp_attr.h>
#include "Renderer.h"

class RenderBuffer;

typedef struct spi_device_t *spi_device_handle_t; ///< Handle for a device on a SPI bus

class SPIRenderer : public Renderer
{
private:
  TaskHandle_t spi_task_handle;
  spi_device_handle_t spi;
  void IRAM_ATTR draw();
  volatile int draw_position;
  volatile int hold;

public:
  SPIRenderer(float world_size);
  void start();
  friend void spi_draw_timer(void *para);
};

#endif
