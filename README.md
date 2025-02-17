# DS1307 Library
Library for handling DS1307 Real Time Clock chip.

## Library Features
- Time and date management
- non-volatile internal RAM management
- Output square wave management

## Hardware Support
It is easy to port this library to any platform. But now it is ready for use in:
- AVR (ATmega32)
- ESP32 (esp-idf)
- STM32 (HAL)
- Zynq PS side

## How To Use
1. Add `DS1307.h` and `DS1307.c` files to your project.  It is optional to use `DS1307_platform.h` and `DS1307_platform.c` files (open and config `DS1307_platform.h` file).
2. Initialize platform-dependent part of handler.
4. Call `DS1307_Init()`.
5. Call other functions and enjoy.

## Example
<details>
<summary>Using DS1307_platform files</summary>

```c
#include <stdio.h>
#include "DS1307.h"
#include "DS1307_platform.h"

int main(void)
{
  DS1307_Handler_t Handler;
  DS1307_DateTime_t DateTime =
  {
    .Second   = 0,
    .Minute   = 18,
    .Hour     = 0,
    .WeekDay  = 6,
    .Day      = 6,
    .Month    = 2,
    .Year     = 21
  };

  DS1307_Platform_Init(&Handler);
  DS1307_Init(&Handler);
  DS1307_SetDateTime(&Handler, &DateTime);
  DS1307_SetOutWave(&Handler, DS1307_OutWave_1Hz);

  while (1)
  {
    DS1307_GetDateTime(&Handler, &DateTime);
    printf("Date: 20%02u/%02u/%02u\r\n", DateTime.Year, DateTime.Month, DateTime.Day);
    printf("Time: %02u:%02u:%02u\r\n", DateTime.Hour, DateTime.Minute, DateTime.Second);
    printf("WeekDay: %u\r\n", DateTime.WeekDay);
  }

  DS1307_DeInit(&Handler);
  return 0;
}
```
</details>


<details>
<summary>Without using DS1307_platform files (esp-idf)</summary>

```c
#include <stdio.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_err.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "DS1307.h"

#define DS1307_I2C_NUM   I2C_NUM_1
#define DS1307_I2C_RATE  100000
#define DS1307_SCL_GPIO  GPIO_NUM_13
#define DS1307_SDA_GPIO  GPIO_NUM_14

int8_t
DS1307_Platform_Init(void)
{
  i2c_config_t conf;
  conf.mode = I2C_MODE_MASTER;
  conf.sda_io_num = DS1307_SDA_GPIO;
  conf.sda_pullup_en = GPIO_PULLUP_DISABLE;
  conf.scl_io_num = DS1307_SCL_GPIO;
  conf.scl_pullup_en = GPIO_PULLUP_DISABLE;
  conf.master.clk_speed = DS1307_I2C_RATE;
  if (i2c_param_config(DS1307_I2C_NUM, &conf) != ESP_OK)
    return -1;
  if (i2c_driver_install(DS1307_I2C_NUM, conf.mode, 0, 0, 0) != ESP_OK)
    return -1;
  return 0;
}

int8_t
DS1307_Platform_DeInit(void)
{
  i2c_driver_delete(DS1307_I2C_NUM);
  gpio_reset_pin(DS1307_SDA_GPIO);
  gpio_reset_pin(DS1307_SCL_GPIO);
  return 0;
}

int8_t
DS1307_Platform_Send(uint8_t Address, uint8_t *Data, uint8_t DataLen)
{
  i2c_cmd_handle_t DS1307_i2c_cmd_handle = 0;
  Address <<= 1;
  Address &= 0xFE;

  DS1307_i2c_cmd_handle = i2c_cmd_link_create();
  i2c_master_start(DS1307_i2c_cmd_handle);
  i2c_master_write(DS1307_i2c_cmd_handle, &Address, 1, 1);
  i2c_master_write(DS1307_i2c_cmd_handle, Data, DataLen, 1);
  i2c_master_stop(DS1307_i2c_cmd_handle);
  if (i2c_master_cmd_begin(DS1307_I2C_NUM, DS1307_i2c_cmd_handle, 1000 / portTICK_RATE_MS) != ESP_OK)
  {
    i2c_cmd_link_delete(DS1307_i2c_cmd_handle);
    return -1;
  }
  i2c_cmd_link_delete(DS1307_i2c_cmd_handle);
  return 0;
}

int8_t
DS1307_Platform_Receive(uint8_t Address, uint8_t *Data, uint8_t DataLen)
{
  i2c_cmd_handle_t DS1307_i2c_cmd_handle = 0;
  Address <<= 1;
  Address |= 0x01;

  DS1307_i2c_cmd_handle = i2c_cmd_link_create();
  i2c_master_start(DS1307_i2c_cmd_handle);
  i2c_master_write(DS1307_i2c_cmd_handle, &Address, 1, 1);
  i2c_master_read(DS1307_i2c_cmd_handle, Data, DataLen, I2C_MASTER_LAST_NACK);
  i2c_master_stop(DS1307_i2c_cmd_handle);
  if (i2c_master_cmd_begin(DS1307_I2C_NUM, DS1307_i2c_cmd_handle, 1000 / portTICK_RATE_MS) != ESP_OK)
  {
    i2c_cmd_link_delete(DS1307_i2c_cmd_handle);
    return -1;
  }
  i2c_cmd_link_delete(DS1307_i2c_cmd_handle);
  return 0;
}

int main(void)
{
  DS1307_Handler_t Handler;
  DS1307_DateTime_t DateTime =
  {
    .Second   = 0,
    .Minute   = 18,
    .Hour     = 0,
    .WeekDay  = 6,
    .Day      = 6,
    .Month    = 2,
    .Year     = 21
  };

  Handler.PlatformInit    = DS1307_Platform_Init;
  Handler.PlatformDeInit  = DS1307_Platform_DeInit;
  Handler.PlatformSend    = DS1307_Platform_Send;
  Handler.PlatformReceive = DS1307_Platform_Receive;

  DS1307_Init(&Handler);
  DS1307_SetDateTime(&Handler, &DateTime);
  DS1307_SetOutWave(&Handler, DS1307_OutWave_1Hz);

  while (1)
  {
    DS1307_GetDateTime(&Handler, &DateTime);
    printf("Date: 20%02u/%02u/%02u\r\n", DateTime.Year, DateTime.Month, DateTime.Day);
    printf("Time: %02u:%02u:%02u\r\n", DateTime.Hour, DateTime.Minute, DateTime.Second);
    printf("WeekDay: %u\r\n", DateTime.WeekDay);
  }

  DS1307_DeInit(&Handler);
  return 0;
}
```
</details>