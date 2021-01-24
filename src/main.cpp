#include <stdio.h>
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "WiFi.h"
#include "Rendering/I2CRenderer.h"
#include "Rendering/SPIRenderer.h"
#include "Rendering/DACRenderer.h"
#include "Rendering/HeltecOLEDRenderer.hpp"
#include "Game/GameLoop.h"
#include "Game/Game.hpp"
#include "Controls/RotaryEncoder.hpp"
#include "Controls/Button.hpp"
#include "Controls/ESP32Controls.hpp"

#define WORLD_SIZE 30
#define ROTARY_ENCODER_CLK GPIO_NUM_19
#define ROTARY_ENCODER_DI GPIO_NUM_18
#define ROTARY_ENCODER_BUTTON GPIO_NUM_5

extern "C"
{
  void app_main(void);
}

void app_main()
{
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  printf("Started Up!\n");

  uint32_t free_ram = esp_get_free_heap_size();
  printf("Free ram before setup %d\n", free_ram);

  // free_ram = esp_get_free_heap_size();
  printf("Creating controls\n");
  RotaryEncoder *rotary_encoder = new RotaryEncoder(ROTARY_ENCODER_CLK, ROTARY_ENCODER_DI);
  Button *fire_button = new Button(ROTARY_ENCODER_BUTTON);
  ESP32Controls *controls = new ESP32Controls(rotary_encoder, fire_button);
  printf("Starting world\n");
  Game *game = new Game(controls);
  game->createWorld(WORLD_SIZE);

  printf("Starting renderer\n");
  // Renderer *renderer = new DACRenderer(WORLD_SIZE);
  // Renderer *renderer = new HeltecOLEDRenderer(WORLD_SIZE);
  Renderer *renderer = new SPIRenderer(WORLD_SIZE);
  // Renderer *renderer = new I2CRenderer(WORLD_SIZE);
  renderer->start();

  GameLoop *game_loop = new GameLoop(game, renderer->get_render_buffer());
  game_loop->start();

  free_ram = esp_get_free_heap_size();
  printf("Free ram after world %d\n", free_ram);

  free_ram = esp_get_free_heap_size();
  printf("Free ram after renderer %d\n", free_ram);

  int64_t start = esp_timer_get_time();
  for (int32_t i = 0; i < 60; ++i)
  {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    int64_t end = esp_timer_get_time();
    printf("%d, World steps %d, FPS %lld, Transactions %d, Free RAM %d\n",
           i,
           game_loop->steps,
           1000 * 1000 * int16_t(renderer->rendered_frames) / (end - start),
           renderer->transactions,
           esp_get_free_heap_size());
  }
  vTaskDelay(5000 / portTICK_PERIOD_MS);
  esp_restart();
}
