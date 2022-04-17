#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "driver/adc.h"
#include "string.h"

/**
 *
 * - Port: configured UART
 * - Receive (Rx) buffer: on
 * - Transmit (Tx) buffer: off
 * - Flow control: off
 * - Event queue: off
 * - Pin assignment: see defines below (See Kconfig)
 */
#define SAMPLE_CNT 32     //number of samples readed to calculate the ADC channel Average
static const adc1_channel_t adc_channel = ADC_CHANNEL_6; //GPIO34

#define TEST_TXD (UART_PIN_NO_CHANGE)
#define TEST_RXD (UART_PIN_NO_CHANGE)
#define TEST_RTS (UART_PIN_NO_CHANGE)
#define TEST_CTS (UART_PIN_NO_CHANGE)

#define UART_PORT_NUM      (UART_NUM_2)
#define UART_BAUD_RATE     (115200)


static const char *TAG = "UART";

#define BUF_SIZE (1024)

static void uart_task(void *arg)
{
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };


    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, TEST_TXD, TEST_RXD, TEST_RTS, TEST_CTS));
    uint32_t adc_val = 0;

    // Configure a temporary buffer for the incoming data
    uint32_t* data = (uint32_t*) malloc(BUF_SIZE);

    while (1) {
        for (int i = 0; i < SAMPLE_CNT; ++i)
        {
            adc_val += adc1_get_raw(adc_channel);
        }
        adc_val /= SAMPLE_CNT;
        //printf("%d\t",adc_val);
        
        // Read data from the UART
        const int len = uart_read_bytes(UART_PORT_NUM, data, (BUF_SIZE - 1), 20 / portTICK_PERIOD_MS);
        
        // Write data back to the UART
        uart_write_bytes(UART_PORT_NUM, adc_val, len);
        ESP_LOGI(TAG, "Recv: %lu", (unsigned long)data);
    }
}

void app_main(void)
{
    xTaskCreate(uart_task, "uart_task", 1024 * 2, NULL, 10, NULL);
}