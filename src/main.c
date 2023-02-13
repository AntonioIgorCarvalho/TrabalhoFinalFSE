#include <stdio.h>
#include <string.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/semphr.h"

#include "wifi.h"
#include "mqtt.h"
#include "led.h"
#include "rotary.h"

SemaphoreHandle_t conexaoWifiSemaphore;
SemaphoreHandle_t conexaoMQTTSemaphore;

void conectadoWifi(void * params)
{
  while(true)
  {
    if(xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY))
    {
      // Processamento Internet
      mqtt_start();
    }
  }
}

void trataComunicacaoComServidor(void * params)
{
  char mensagem[50];
  if(xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY)) {
    while(true) {
      sprintf(mensagem, "{\"potencia_vermelho\": %d}", rotary_r);
      mqtt_envia_mensagem("v1/devices/me/attributes", mensagem);

      sprintf(mensagem, "{\"potencia_verde\": %d}", rotary_g);
      mqtt_envia_mensagem("v1/devices/me/attributes", mensagem);

      sprintf(mensagem, "{\"potencia_azul\": %d}", rotary_b);
      mqtt_envia_mensagem("v1/devices/me/attributes", mensagem);
      vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
  }
}

int32_t le_valor_nvs()
{
    // Inicia o acesso à partição padrão nvs
    ESP_ERROR_CHECK(nvs_flash_init());

    // Inicia o acesso à partição personalizada
    // ESP_ERROR_CHECK(nvs_flash_init_partition("DadosNVS"));

    int32_t valor = 0;
    nvs_handle particao_padrao_handle;
    
    // Abre o acesso à partição nvs
    esp_err_t res_nvs = nvs_open("armazenamento", NVS_READONLY, &particao_padrao_handle);
    
    // Abre o acesso à partição DadosNVS
    // esp_err_t res_nvs = nvs_open_from_partition("DadosNVS", "armazenamento", NVS_READONLY, &particao_padrao_handle);
    


    if(res_nvs == ESP_ERR_NVS_NOT_FOUND)
    {
        ESP_LOGE("NVS", "Namespace: armazenamento, não encontrado");
    }
    else
    {
        esp_err_t res = nvs_get_i32(particao_padrao_handle, "contador", &valor);

        switch (res)
        {
        case ESP_OK:
            printf("Valor armazenado: %d\n", (int) valor);
            break;
        case ESP_ERR_NOT_FOUND:
            ESP_LOGE("NVS", "Valor não encontrado");
            return -1;
        default:
            ESP_LOGE("NVS", "Erro ao acessar o NVS (%s)", esp_err_to_name(res));
            return -1;
            break;
        }

        nvs_close(particao_padrao_handle);
    }
    return valor;
}

void grava_valor_nvs(int32_t valor)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    // ESP_ERROR_CHECK(nvs_flash_init_partition("DadosNVS"));

    nvs_handle particao_padrao_handle;

    esp_err_t res_nvs = nvs_open("armazenamento", NVS_READWRITE, &particao_padrao_handle);
    // esp_err_t res_nvs = nvs_open_from_partition("DadosNVS", "armazenamento", NVS_READWRITE, &particao_padrao_handle);
    
    if(res_nvs == ESP_ERR_NVS_NOT_FOUND)
    {
        ESP_LOGE("NVS", "Namespace: armazenamento, não encontrado");
    }
    esp_err_t res = nvs_set_i32(particao_padrao_handle, "contador", valor + 1);
    if(res != ESP_OK)
    {
        ESP_LOGE("NVS", "Não foi possível escrever no NVS (%s)", esp_err_to_name(res));
    }
    nvs_commit(particao_padrao_handle);
    nvs_close(particao_padrao_handle);
}


void app_main(void)
{
    int32_t valor_lido = 0;
    valor_lido = le_valor_nvs();
    if(valor_lido == -1){
        valor_lido = 0;
    }
    grava_valor_nvs(valor_lido);

    
    conexaoWifiSemaphore = xSemaphoreCreateBinary();
    conexaoMQTTSemaphore = xSemaphoreCreateBinary();
    wifi_start();

    xTaskCreate(control_rotary_decoder, "Controle do Rotary", 4096, NULL, 1, NULL);
    xTaskCreate(&conectadoWifi,  "Conexão ao MQTT", 4096, NULL, 1, NULL);
    xTaskCreate(&trataComunicacaoComServidor, "Comunicação com Broker", 4096, NULL, 1, NULL);
    xTaskCreate(pwm_r, "Controle LED Vermelho", 4096, NULL, 1, NULL);
    xTaskCreate(pwm_g, "Controle LED Verde", 4096, NULL, 1, NULL);
    xTaskCreate(pwm_b, "Controle LED Azul", 4096, NULL, 1, NULL);
}
