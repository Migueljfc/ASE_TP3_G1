#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "esp_sleep.h"
#include "driver/touch_pad.h"
#include "esp_timer.h"

static const char *TAG = "TC74-I2C:";

#define I2C_MASTER_SCL_IO           22                         /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           21                         /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000
#define SAMPLE_RATE_MS              500

#define SENSOR_ADDR                 0x4D                       /*!< Slave address of the sensor */

#define SEC_MULTIPLIER 1000000l

/**
 * @brief Initialize touchpad 
 */
static void init_hw(void)
{

}

/**
 * @brief Read a sequence of bytes from sensor
 */
static esp_err_t sensor_read(uint8_t reg_addr, uint8_t *data, size_t len)
{
    return i2c_master_read_from_device(I2C_MASTER_NUM, SENSOR_ADDR, data, \
    len, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
}

/**
 * @brief i2c master initialization
 */
static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, \
     I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}


void app_main(void)
{
    uint8_t data[2];
    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(TAG, "I2C initialized successfully");
    init_hw();
    
    while (1)
    {
        
        ESP_ERROR_CHECK(sensor_read(SENSOR_ADDR, data, 1));
        ESP_LOGI(TAG, "Val = %d", data[0]);    

    }

    ESP_ERROR_CHECK(i2c_driver_delete(I2C_MASTER_NUM));
    ESP_LOGI(TAG, "I2C unitialized successfully");
}