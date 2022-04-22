#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "driver/adc.h"
#include "string.h"
#include "esp_sleep.h"
#include "driver/touch_pad.h"
#include "esp_timer.h"

/**
 *
 * - Port: configured UART
 * - Receive (Rx) buffer: on
 * - Transmit (Tx) buffer: off
 * - Flow control: off
 * - Event queue: off
 * - Pin assignment: see defines below (See Kconfig)
 */



#define TEST_TXD (UART_PIN_NO_CHANGE)
#define TEST_RXD (UART_PIN_NO_CHANGE)
#define TEST_RTS (UART_PIN_NO_CHANGE)
#define TEST_CTS (UART_PIN_NO_CHANGE)

#define UART_PORT_NUM (UART_NUM_2)
#define UART_BAUD_RATE (115200)

static const char *TAG = "UART";

#define BUF_SIZE (1024) 
#define SEC_MULTIPLIER 1000000l

/* Configure adc channel */
static const adc1_channel_t adc_channel = ADC_CHANNEL_6; // GPIO34

static void init_hw(void)
{
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, TEST_TXD, TEST_RXD, TEST_RTS, TEST_CTS));

    /* Configure the touchpad sensor */
    touch_pad_init();
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
    touch_pad_config(TOUCH_PAD_NUM8, 0);
    touch_pad_filter_start(10);
    uint16_t val;
    touch_pad_read_filtered(TOUCH_PAD_NUM8, &val);
    touch_pad_set_thresh(TOUCH_PAD_NUM8, val * 0.2);
}

static void uart_task(void *arg)
{

    init_hw();

    uint32_t adc_val = 0;

    // Configure a temporary buffer for the incoming data
    char *data = (char *)malloc(BUF_SIZE);

    while (1)
    {
        /* Configure the timer and the touchpad as wakeup sources */
        esp_sleep_enable_timer_wakeup(5 * SEC_MULTIPLIER);
        esp_sleep_enable_touchpad_wakeup();

        /* Enter in ligh sleep mode*/
        esp_light_sleep_start();

        printf("Active at timer value: %lli by %s \n", esp_timer_get_time() / SEC_MULTIPLIER, esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER ? "Wakeup Timer" : "Touchpad");

        /* Read the value from adc */
        adc_val = adc1_get_raw(adc_channel);
        
        data = (char) adc_val;

        // Read data from the UART
        const int len = uart_read_bytes(UART_PORT_NUM, data, (BUF_SIZE - 1), 20 / portTICK_PERIOD_MS);

        // Write data back to the UART
        uart_write_bytes(UART_PORT_NUM, (char) adc_val, len);

        ESP_LOGI(TAG, "Recv: %d", (int)data);
    }
}

void app_main(void)
{
    xTaskCreate(uart_task, "uart_task", 1024 * 2, NULL, 10, NULL);
}