#include "rotary.h"
#include "mqtt.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>

#define ROTARY_PIN_A 2
#define ROTARY_PIN_B 4

int rotary_r = 0;
int rotary_g = 0;
int rotary_b = 0;

void control_rotary_decoder() {
  gpio_config_t io_conf;
  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pin_bit_mask = (1ULL<<ROTARY_PIN_A) | (1ULL<<ROTARY_PIN_B);
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
  gpio_config(&io_conf);

  int lastA = 0;

  while (true) {
    int levelA = gpio_get_level(ROTARY_PIN_A);
    int levelB = gpio_get_level(ROTARY_PIN_B);

    if(activeMax == 't'){
      rotary_r = rotary_g = rotary_b = 255;
    }
    else if (activeMin == 't'){
      rotary_r = rotary_g = rotary_b = 0;
    }
    else {
      if(levelA != lastA){
        if(levelA == levelB){
          if(activeR == 't' && rotary_r < 255){
            rotary_r++;
          }
          if(activeG == 't' && rotary_g < 255){
            rotary_g++;
          }
          if(activeB == 't' && rotary_b < 255){
            rotary_b++;
          }
        }
        else {
          if(activeR == 't' && rotary_r  > 0){
            rotary_r--;
          }
          if(activeG == 't' && rotary_g  > 0){
            rotary_g--;
          }
          if(activeB == 't' && rotary_b  > 0){
            rotary_b--;
          }
        }
      }
    }

    lastA = levelA;
    vTaskDelay(40 / portTICK_PERIOD_MS);
  }
    
}