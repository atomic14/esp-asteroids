#ifndef _spi_renderer_h_
#define _spi_renderer_h_

#include "Renderer.h"

class RenderBuffer;

typedef struct spi_device_t *spi_device_handle_t; ///< Handle for a device on a SPI bus

class SPIRenderer : public Renderer
{
private:
  spi_device_handle_t spi;

public:
  SPIRenderer(RenderBuffer *render_buffer) : Renderer(render_buffer)
  {
  }
  void start();
  void stop();
  virtual void draw(const DrawInstruction_t &instruction);
};

#endif