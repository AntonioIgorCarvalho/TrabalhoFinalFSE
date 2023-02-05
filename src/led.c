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

void pwm_r(){
    pwm('r');
}
void pwm_g(){
    pwm('g');
}
void pwm_b(){
    pwm('b');
}

void pwm(char led) {

int led_channel;

if(led == 'r'){
    led_channel = LED_R;
}
else if(led == 'g'){
    led_channel = LED_G;
}
else if(led == 'b'){
    led_channel = LED_B;
}
else {
    return;
}

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
  ledc_channel_config_t channel_config_r = {
    .gpio_num = led_channel,
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel = LEDC_CHANNEL_0,
    .timer_sel = LEDC_TIMER_0,
    .duty = 0,
    .hpoint = 0
  };
  ledc_channel_config(&channel_config_r);

  ledc_fade_func_install(0);
  while(true) {
    ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, rotary_counter, 1000, LEDC_FADE_WAIT_DONE);
  }
}