#include <stdio.h>
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "WiFi.h"
#include "SPIRenderer.h"
#include "DACRenderer.h"
#include "ESP32Game.h"

extern "C"
{
  void app_main(void);
}

void app_main()
{
  //Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  printf("Started Up!\n");
  vTaskDelay(5000 / portTICK_PERIOD_MS);

  uint32_t free_ram = esp_get_free_heap_size();
  printf("Free ram before wifi %d\n", free_ram);

  // wifi_init_sta();

  // free_ram = esp_get_free_heap_size();
  printf("Free ram after wifi %d\n", free_ram);

  printf("Starting world\n");
  ESP32Game *game = new ESP32Game();
  game->createWorld();
  game->start();

  free_ram = esp_get_free_heap_size();
  printf("Free ram after world %d\n", free_ram);

  printf("Starting renderer\n");
  Renderer *renderer = new DACRenderer(game);
  renderer->start();

  free_ram = esp_get_free_heap_size();
  printf("Free ram after renderer %d\n", free_ram);

  for (int32_t i = 0; i < 60; ++i)
  {
    int64_t start = esp_timer_get_time();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("%d, World steps %d, Rendered %d frames, %d SPI packets, %d LDAC toggles, %d failures\n", i, game->steps, renderer->renders, renderer->sample_send_success, renderer->ldac_calls, renderer->sample_send_fail);
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
