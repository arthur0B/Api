#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "cJSON.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"

#define UART_NUM UART_NUM_1
#define BUF_SIZE 256
#define STACK_SIZE 4096

typedef struct {
    uint16_t anTbl[2][6];
    uint16_t digTbl[2][1];
    uint16_t scalingFactor[6];
    uint16_t scalingOffset[6];
    uint16_t scaledValues[6];
} SensorData;

typedef struct {
    char CFG_OP_MODE[5];
    char CFG_RUN_PGM[5];
    char CFG_DHCP[5];
    char CFG_MB_MASTER_INTERFACE[5];
    char CFG_MB_MASTER_BAUDRATE[2][5];
    char CFG_MB_SLAVE_INTERFACE[5];
    char CFG_MB_SLAVE_BAUDRATE[2][5];
    char CFG_GL_TMR_INTERVAL[5];
    char CFG_IP0[2][5];
    char CFG_SUBNET_MASK[2][5];
    char CFG_IP2[2][5];
    char CFG_IP3[2][5];
    char CFG_GW_H[2][5];
    char CFG_SLAVE_IP[2][5];
    char CFG_GL_FILTER_ALPHA[5];
    char CFG_GL_PID_TMR_INTERVAL[5];
    char CFG_WIFI_MODE[5];
    char CFG_GL_PID_SP[5];
    char CFG_GL_PID_KP[5];
    char CFG_GL_PID_KI[5];
    char CFG_GL_PID_KD[5];
    char CFG_GL_PID_N[5];
    char CFG_GL_PID_CP[5];
    char CFG_GL_PID_CI[5];
    char CFG_GL_PID_CD[5];
    char CFG_REMOTA_LOG_LEVEL[5];
} ConfigData;

SensorData s3Tables;
ConfigData config;
static const char *TAG = "ESP32";

void generate_hex_value(char *buffer, int length) {
    snprintf(buffer, length, "%04X", rand() % 65536);
}

// Inicializa los valores simulados de las tablas y configuración
void init_tables() {
    for (int i = 0; i < 6; i++) {
        s3Tables.anTbl[0][i] = rand() % 1000;
        s3Tables.scalingFactor[i] = rand() % 100 + 1;
        s3Tables.scalingOffset[i] = rand() % 10;
        s3Tables.scaledValues[i] = (s3Tables.anTbl[0][i] * s3Tables.scalingFactor[i]) + s3Tables.scalingOffset[i];
    }
    s3Tables.digTbl[0][0] = 0b00011111;

    generate_hex_value(config.CFG_OP_MODE, sizeof(config.CFG_OP_MODE));
    generate_hex_value(config.CFG_RUN_PGM, sizeof(config.CFG_RUN_PGM));
    generate_hex_value(config.CFG_DHCP, sizeof(config.CFG_DHCP));
    generate_hex_value(config.CFG_MB_MASTER_INTERFACE, sizeof(config.CFG_MB_MASTER_INTERFACE));
    generate_hex_value(config.CFG_MB_MASTER_BAUDRATE[0], sizeof(config.CFG_MB_MASTER_BAUDRATE[0]));
    generate_hex_value(config.CFG_MB_MASTER_BAUDRATE[1], sizeof(config.CFG_MB_MASTER_BAUDRATE[1]));
    generate_hex_value(config.CFG_MB_SLAVE_INTERFACE, sizeof(config.CFG_MB_SLAVE_INTERFACE));
    generate_hex_value(config.CFG_MB_SLAVE_BAUDRATE[0], sizeof(config.CFG_MB_SLAVE_BAUDRATE[0]));
    generate_hex_value(config.CFG_MB_SLAVE_BAUDRATE[1], sizeof(config.CFG_MB_SLAVE_BAUDRATE[1]));
    generate_hex_value(config.CFG_GL_TMR_INTERVAL, sizeof(config.CFG_GL_TMR_INTERVAL));
    generate_hex_value(config.CFG_IP0[0], sizeof(config.CFG_IP0[0]));
    generate_hex_value(config.CFG_IP0[1], sizeof(config.CFG_IP0[1]));
    generate_hex_value(config.CFG_SUBNET_MASK[0], sizeof(config.CFG_SUBNET_MASK[0]));
    generate_hex_value(config.CFG_SUBNET_MASK[1], sizeof(config.CFG_SUBNET_MASK[1]));
    generate_hex_value(config.CFG_IP2[0], sizeof(config.CFG_IP2[0]));
    generate_hex_value(config.CFG_IP2[1], sizeof(config.CFG_IP2[1]));
    // Continúa simulando el resto de los valores de configuración
}

char *generate_json() {
    for (int i = 0; i < 6; i++) {
        s3Tables.anTbl[0][i] = rand() % 1000;
        s3Tables.scaledValues[i] = (s3Tables.anTbl[0][i] * s3Tables.scalingFactor[i]) + s3Tables.scalingOffset[i];
    }
    s3Tables.digTbl[0][0] = rand() % 2 ? 0b00011111 : 0b00000011;

    cJSON *root = cJSON_CreateObject();
    cJSON *config_json = cJSON_CreateObject();
    
    cJSON_AddStringToObject(config_json, "CFG_OP_MODE", config.CFG_OP_MODE);
    cJSON_AddStringToObject(config_json, "CFG_RUN_PGM", config.CFG_RUN_PGM);
    cJSON_AddStringToObject(config_json, "CFG_DHCP", config.CFG_DHCP);
    cJSON_AddStringToObject(config_json, "CFG_MB_MASTER_INTERFACE", config.CFG_MB_MASTER_INTERFACE);
    cJSON *baudrate_master = cJSON_CreateStringArray((const char *[]) { config.CFG_MB_MASTER_BAUDRATE[0], config.CFG_MB_MASTER_BAUDRATE[1] }, 2);
    cJSON_AddItemToObject(config_json, "CFG_MB_MASTER_BAUDRATE", baudrate_master);
    
    cJSON_AddItemToObject(root, "CONFIGURACION", config_json);

    cJSON *valores_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(valores_json, "NF_AI_PTL", s3Tables.anTbl[0][0]);
    cJSON_AddNumberToObject(valores_json, "NF_AI_TTL", s3Tables.anTbl[0][1]);
    cJSON_AddNumberToObject(valores_json, "NF_AI_PTC", s3Tables.anTbl[0][2]);
    cJSON_AddNumberToObject(valores_json, "NF_AI_PTA", s3Tables.anTbl[0][3]);
    cJSON_AddNumberToObject(valores_json, "NF_AI_TTA", s3Tables.anTbl[0][4]);
    cJSON_AddNumberToObject(valores_json, "NF_AI_FT", s3Tables.anTbl[0][5]);
    cJSON_AddItemToObject(root, "VALORES", valores_json);

    char *json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    return json_str;
}

// Procesa los comandos recibidos para actualizar valores
void handle_command(char *command) {
    cJSON *json = cJSON_Parse(command);
    if (json == NULL) {
        ESP_LOGE(TAG, "Error al parsear JSON");
        return;
    }

    cJSON *cmd = cJSON_GetObjectItem(json, "command");
    if (cJSON_IsString(cmd) && (strcmp(cmd->valuestring, "update_data") == 0)) {
        cJSON *data = cJSON_GetObjectItem(json, "data");
        if (cJSON_IsObject(data)) {
            cJSON *new_val = NULL;
            
            new_val = cJSON_GetObjectItem(data, "CFG_OP_MODE");
            if (cJSON_IsString(new_val)) strncpy(config.CFG_OP_MODE, new_val->valuestring, sizeof(config.CFG_OP_MODE));

            new_val = cJSON_GetObjectItem(data, "NF_AI_PTL");
            if (cJSON_IsNumber(new_val)) s3Tables.anTbl[0][0] = new_val->valueint;

            ESP_LOGI(TAG, "Valores actualizados desde API");
        }
    }
    cJSON_Delete(json);
}

// Envía datos periódicamente y procesa comandos entrantes
void send_periodic_data(void *pvParameter) {
    ESP_LOGI(TAG, "Iniciando tarea send_periodic_data");
    uint8_t buffer[BUF_SIZE];
    while (1) {
        int length = uart_read_bytes(UART_NUM, buffer, BUF_SIZE, 20 / portTICK_PERIOD_MS);
        if (length > 0) {
            buffer[length] = '\0';
            ESP_LOGI(TAG, "Comando recibido: %s", buffer);
            handle_command((char *)buffer);
        } else {
            char *json_str = generate_json();
            ESP_LOGI(TAG, "Enviando datos: %s", json_str);
            uart_write_bytes(UART_NUM, json_str, strlen(json_str));
            free(json_str);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main() {
    ESP_LOGI(TAG, "Iniciando ESP32");
    init_tables();

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM, &uart_config);
    uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);

    xTaskCreate(&send_periodic_data, "send_periodic_data", STACK_SIZE, NULL, 5, NULL);
}
