/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "driver/spi_master.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//SPI测试程序

#define PIN_NUM_MISO 25
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK 19
#define PIN_NUM_CS 22

static void esp32_info(void);
static void spi_master_send(spi_device_handle_t spi, const uint32_t cmd);
static uint32_t spi_master_recv(spi_device_handle_t spi);

void app_main() {
  esp_err_t ret;
  spi_device_handle_t handle;
  uint32_t recv_buf = 0;

  spi_host_device_t spi_master = HSPI_HOST;

  spi_bus_config_t bus_config = {
      .miso_io_num = PIN_NUM_MISO,
      .mosi_io_num = PIN_NUM_MOSI,
      .sclk_io_num = PIN_NUM_CLK,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
      .max_transfer_sz = 32,
  };

  spi_device_interface_config_t dev_config = {
      .clock_speed_hz = 1 * 1000 * 1000, // Clock out at 1 MHz
      .mode = 0,                         // SPI mode 0
      .spics_io_num = PIN_NUM_CS,        // CS pin
      .queue_size = 32, // We want to be able to queue 32 transactions at a time
  };

  esp32_info();

  ret = spi_bus_initialize(spi_master, &bus_config, 1); // 1 open DMA
  ESP_ERROR_CHECK(ret);

  ret = spi_bus_add_device(spi_master, &dev_config, &handle);
  ESP_ERROR_CHECK(ret);

  for (int i = 10; i >= 0; i--) {
    printf("spi_send 0x5a5a5a5a %d\n", i);

    spi_master_send(handle, 0x12345678);
    recv_buf = spi_master_recv(handle);
    printf("spi_recv recv_buf = %x \n", recv_buf);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

  printf("Restarting now.\n");
  fflush(stdout);
  esp_restart();
}

static void esp32_info(void) {
  printf("Hello world!\n");

  /* Print chip information */
  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);
  printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ", chip_info.cores,
         (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
         (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

  printf("silicon revision %d, ", chip_info.revision);

  printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
         (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded"
                                                       : "external");
}

static void spi_master_send(spi_device_handle_t spi, const uint32_t cmd) {
  esp_err_t ret;
  spi_transaction_t t;

  memset(&t, 0, sizeof(t)); // Zero out the transaction
  t.length = 32;            // Command is 8 bits
  t.tx_buffer = &cmd;       // The data is the cmd itself

  ret = spi_device_polling_transmit(spi, &t); // Transmit!
  assert(ret == ESP_OK);                      // Should have had no issues.
}

static uint32_t spi_master_recv(spi_device_handle_t spi) {
  esp_err_t ret;
  spi_transaction_t t;

  memset(&t, 0, sizeof(t));
  t.length = 32;

  ret = spi_device_polling_transmit(spi, &t);
  assert(ret == ESP_OK);

  return *(uint32_t *)t.rx_data;
}
