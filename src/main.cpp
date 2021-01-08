#include <stdio.h>
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "WiFi.h"
#include "RenderBuffer.hpp"
#include "SPIRenderer.h"
#include "DACRenderer.h"
#include "GameLoop.h"
#include "Game.hpp"

#define WORLD_SIZE 30
#define ROTARY_ENCODER_CLK
#define ROTARY_ENCODER_DI

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

  // suitable values for the DAC Renderer
  RenderBuffer *render_buffer = new RenderBuffer(
      0, 255,
      0, 255,
      128,
      128,
      128 / WORLD_SIZE);

  // free_ram = esp_get_free_heap_size();
  printf("Starting world\n");
  Game *game = new Game();
  game->createWorld(WORLD_SIZE);

  GameLoop *game_loop = new GameLoop(game, render_buffer);
  game_loop->start();

  free_ram = esp_get_free_heap_size();
  printf("Free ram after world %d\n", free_ram);

  printf("Starting renderer\n");
  Renderer *renderer = new DACRenderer(render_buffer);
  renderer->start();

  free_ram = esp_get_free_heap_size();
  printf("Free ram after renderer %d\n", free_ram);

  for (int32_t i = 0; i < 60; ++i)
  {
    int64_t start = esp_timer_get_time();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("%d, World steps %d, Rendered %d frames, %d SPI packets, %d LDAC toggles, %d failures\n", i, game_loop->steps, renderer->renders, renderer->sample_send_success, renderer->ldac_calls, renderer->sample_send_fail);
    int64_t end = esp_timer_get_time();
    printf("Elapsed time = %lld\n", end - start);
    free_ram = esp_get_free_heap_size();
    printf("Free ram %d\n", free_ram);
  }
  renderer->stop();
  printf("Stopped renderer timer - reboot in 5 seconds\n");
  vTaskDelay(5000 / portTICK_PERIOD_MS);
  esp_restart();
}
