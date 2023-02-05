#include "led.h"
#include <driver/gpio.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "rotary.h"


#define LED_R 18
#define LED_G 19
#define LED_B 21

void controla_led() {
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
        muxed to GPIO on reset already, but some default to other
        functions and need to be switched to GPIO. Consult the
        Technical Reference for a list of pads and their default
        functions.)
    */
  esp_rom_gpio_pad_select_gpio(LED_R);
  gpio_set_direction(LED_R, GPIO_MODE_OUTPUT);

  esp_rom_gpio_pad_select_gpio(LED_G);
  gpio_set_direction(LED_G, GPIO_MODE_OUTPUT);

  esp_rom_gpio_pad_select_gpio(LED_B);
  gpio_set_direction(LED_B, GPIO_MODE_OUTPUT);

  while (1) {
    /* Blink off (output low) */
    printf("Turning off the LED\n");
    gpio_set_level(LED_R, 0);
    gpio_set_level(LED_G, 0);
    gpio_set_level(LED_B, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    /* Blink on (output high) */
    printf("Turning on the LED\n");
    gpio_set_level(LED_R, 1);
    gpio_set_level(LED_G, 1);
    gpio_set_level(LED_B, 1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void pwm() {

// Configuração do Timer
  ledc_timer_config_t timer_config = {
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .duty_resolution = LEDC_TIMER_8_BIT,
    .timer_num = LEDC_TIMER_0,
    .freq_hz = 1000,
    .clk_cfg = LEDC_AUTO_CLK
  };
  ledc_timer_config(&timer_config);

  // Configuração do Canal
  ledc_channel_config_t channel_config = {
    .gpio_num = LED_R,
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel = LEDC_CHANNEL_0,
    .timer_sel = LEDC_TIMER_0,
    .duty = 0,
    .hpoint = 0
  };
  ledc_channel_config(&channel_config);

  ledc_fade_func_install(0);
  while(true) {
    ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, rotary_counter, 1000, LEDC_FADE_WAIT_DONE);
  }
}