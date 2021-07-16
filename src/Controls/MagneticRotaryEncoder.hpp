#pragma once

#include <math.h>
#include <driver/gpio.h>
#include "RotaryEncoder.hpp"

typedef struct spi_device_t *spi_device_handle_t; ///< Handle for a device on a SPI bus
typedef void *TaskHandle_t;

class MagneticRotaryEncoder : public RotaryEncoder
{
private:
  TaskHandle_t spi_task_handle;
  spi_device_handle_t spi;

  bool error_flag;

  uint16_t zero_position;

  uint16_t read(uint16_t registerAddress);
  void update_current_angle();
  float current_angle;
  gpio_num_t cs;

public:
  MagneticRotaryEncoder(gpio_num_t cs, gpio_num_t clk, gpio_num_t miso, gpio_num_t mosi);
  ~MagneticRotaryEncoder();
  float get_angle()
  {
    return -current_angle;
  }
  float read_angle();
  const char *get_and_clear_errors();
  friend void rotary_encoder_reader_task(void *param);
};
