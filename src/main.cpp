#include <stdio.h>
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"

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
#include "Audio/I2SOutput.h"
#include "Audio/WAVFile.h"
#include "Audio/SoundFX.h"

#define WORLD_SIZE 30
#define ROTARY_ENCODER_CLK_GPIO GPIO_NUM_23
#define ROTARY_ENCODER_DI_GPIO GPIO_NUM_22
#define FIRE_GPIO GPIO_NUM_21

extern "C"
{
  void app_main(void);
}

static const char *TAG = "APP";

void app_main()
{
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  ESP_LOGI(TAG, "Started UP");

  esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = true};

  esp_err_t ret = esp_vfs_spiffs_register(&conf);

  if (ret != ESP_OK)
  {
    if (ret == ESP_FAIL)
    {
      ESP_LOGE(TAG, "Failed to mount or format filesystem");
    }
    else if (ret == ESP_ERR_NOT_FOUND)
    {
      ESP_LOGE(TAG, "Failed to find SPIFFS partition");
    }
    else
    {
      ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
    }
  }

  uint32_t free_ram = esp_get_free_heap_size();
  printf("Free ram before setup %d\n", free_ram);

  printf("Starting audio");
  I2SOutput *i2sOutput = new I2SOutput();
  i2sOutput->start();

  free_ram = esp_get_free_heap_size();
  printf("Free ram after I2SOutput %d\n", free_ram);

  SoundFX *sound_fx = new SoundFX(i2sOutput);
  free_ram = esp_get_free_heap_size();
  printf("Free ram after SoundFX %d\n", free_ram);

  printf("Creating controls\n");
  RotaryEncoder *rotary_encoder = new RotaryEncoder(ROTARY_ENCODER_CLK_GPIO, ROTARY_ENCODER_DI_GPIO);

  free_ram = esp_get_free_heap_size();
  printf("Free ram after RotaryEncoder %d\n", free_ram);

  Button *fire_button = new Button(FIRE_GPIO);

  free_ram = esp_get_free_heap_size();
  printf("Free ram after Button %d\n", free_ram);

  ESP32Controls *controls = new ESP32Controls(rotary_encoder, fire_button);
  printf("Starting world\n");
  Game *game = new Game(NULL, NULL);
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
