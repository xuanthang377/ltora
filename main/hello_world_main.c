#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_system.h"

#define M0 12
#define M1 13

#define MY_UART_NUM_1 UART_NUM_0 // UART0
#define TXD_PIN_1 (GPIO_NUM_1)
#define RXD_PIN_1 (GPIO_NUM_3)

#define MY_UART_NUM_2 UART_NUM_2 // UART2
#define TXD_PIN_2 (GPIO_NUM_17) // Thay XX bằng GPIO phù hợp
#define RXD_PIN_2 (GPIO_NUM_16) // Thay YY bằng GPIO phù hợp

static const char *TAG = "UART Example";

void init_uart()
{
    uart_config_t uart_config_1 = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    uart_config_t uart_config_2 = {
        .baud_rate = 9600, // Thay đổi baud rate cho UART2 nếu cần
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    uart_param_config(MY_UART_NUM_1, &uart_config_1);
    uart_set_pin(MY_UART_NUM_1, TXD_PIN_1, RXD_PIN_1, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(MY_UART_NUM_1, 256, 0, 0, NULL, 0);

    uart_param_config(MY_UART_NUM_2, &uart_config_2);
    uart_set_pin(MY_UART_NUM_2, TXD_PIN_2, RXD_PIN_2, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(MY_UART_NUM_2, 256, 0, 0, NULL, 0);
}

void app_main()
{
    init_uart();
    gpio_set_direction(M0, GPIO_MODE_OUTPUT);
    gpio_set_direction(M1, GPIO_MODE_OUTPUT);
    gpio_set_level(M0, 0);
    gpio_set_level(M1, 0);


    while (1)
    {
        uint8_t data[128];
        int length_1 = uart_read_bytes(MY_UART_NUM_1, data, sizeof(data), 20 / portTICK_PERIOD_MS);
        if (length_1 > 0)
        {
            ESP_LOGI(TAG, "Received data from UART0: %.*s", length_1, data);
            uart_write_bytes(MY_UART_NUM_2, (const char *)data, length_1); // Ghi vào UART2

        }

        int length_2 = uart_read_bytes(MY_UART_NUM_2, data, sizeof(data), 20 / portTICK_PERIOD_MS);
        if (length_2 > 0)
        {
            ESP_LOGI(TAG, "Received data from UART2: %.*s", length_2, data);
            uart_write_bytes(MY_UART_NUM_1, (const char *)data, length_2); // Ghi vào UART0
        }

        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}
