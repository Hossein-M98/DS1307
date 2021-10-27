/**
 **********************************************************************************
 * @file   DS1307_platform.c
 * @author Hossein.M (https://github.com/Hossein-M98)
 * @brief  DS1307 chip driver platform dependent part
 *         Functionalities of the this file:
 *          + Initialization the platform-dependent part of handler
 **********************************************************************************
 *
 * Copyright (c) 2021 Hossein.M (MIT License)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 **********************************************************************************
 */

/* Includes ---------------------------------------------------------------------*/
#include "DS1307_platform.h"

#if defined(DS1307_PLATFORM_AVR)
#include <avr/io.h>
#elif defined(DS1307_PLATFORM_ESP32_IDF)
#include "sdkconfig.h"
#include "esp_system.h"
#include "driver/i2c.h"
#elif defined(DS1307_PLATFORM_STM32_HAL)
#include "main.h"
#endif


/* Private Macro ----------------------------------------------------------------*/
#ifndef _BV
#define _BV(bit) (1<<(bit))
#endif

#ifndef cbi
#define cbi(reg,bit) reg &= ~(_BV(bit))
#endif

#ifndef sbi
#define sbi(reg,bit) reg |= (_BV(bit))
#endif

#ifndef CHECKBIT
#define CHECKBIT(reg,bit) ((reg & _BV(bit)) ? 1 : 0)
#endif



/**
 ==================================================================================
                           ##### Private Functions #####                           
 ==================================================================================
 */

static void
Platform_Init(void)
{
#if defined(DS1307_PLATFORM_AVR)
  TWBR = (uint8_t)(DS1307_CPU_CLK - 1600000) / (2 * 100000);
#elif defined(DS1307_PLATFORM_ESP32_IDF)
  i2c_config_t conf = {0};
  conf.mode = I2C_MODE_MASTER;
  conf.sda_io_num = DS1307_SDA_GPIO;
  conf.sda_pullup_en = GPIO_PULLUP_DISABLE;
  conf.scl_io_num = DS1307_SCL_GPIO;
  conf.scl_pullup_en = GPIO_PULLUP_DISABLE;
  conf.master.clk_speed = DS1307_I2C_RATE;
  if (i2c_param_config(DS1307_I2C_NUM, &conf) != ESP_OK)
    return;
  i2c_driver_install(DS1307_I2C_NUM, conf.mode, 0, 0, 0);
#elif defined(DS1307_PLATFORM_STM32_HAL)
#endif
}


static void
Platform_DeInit(void)
{
#if defined(DS1307_PLATFORM_AVR)
#elif defined(DS1307_PLATFORM_ESP32_IDF)
  i2c_driver_delete(DS1307_I2C_NUM);
  gpio_reset_pin(DS1307_SDA_GPIO);
  gpio_reset_pin(DS1307_SCL_GPIO);
#elif defined(DS1307_PLATFORM_STM32_HAL)
#endif
}


static void
Platform_Send(uint8_t Address, uint8_t *Data, uint8_t Len)
{
#if defined(DS1307_PLATFORM_AVR)
  uint8_t DataCounter = 0;

  TWCR = _BV(TWEN) | _BV(TWSTA) | _BV(TWEA) | _BV(TWINT); // TWI enable *** acknowledge enable
  while (!CHECKBIT(TWCR, TWINT)); // wait until the process ends

  TWDR = Address<<1;                  // set data in data register to sending
  TWCR = _BV(TWEN) | _BV(TWEA) | _BV(TWINT); // TWI enable *** acknowledge enable
  while (!CHECKBIT(TWCR, TWINT));

  for (DataCounter = 0; DataCounter < Len; DataCounter++)
  {
    TWDR = Data[DataCounter];                  // set data in data register to sending
    TWCR = _BV(TWEN) | _BV(TWEA) | _BV(TWINT); // TWI enable *** acknowledge enable
    while (!CHECKBIT(TWCR, TWINT));
  }
  
  TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTO); // send the STOP mode bit

#elif defined(DS1307_PLATFORM_ESP32_IDF)
  i2c_cmd_handle_t DS1307_i2c_cmd_handle = 0;
  Address <<= 1;
  Address &= 0xFE;

  DS1307_i2c_cmd_handle = i2c_cmd_link_create();
  i2c_master_start(DS1307_i2c_cmd_handle);
  i2c_master_write(DS1307_i2c_cmd_handle, &Address, 1, 1);
  i2c_master_write(DS1307_i2c_cmd_handle, Data, Len, 1);
  i2c_master_stop(DS1307_i2c_cmd_handle);
  i2c_master_cmd_begin(DS1307_I2C_NUM, DS1307_i2c_cmd_handle, 1000 / portTICK_RATE_MS);
  i2c_cmd_link_delete(DS1307_i2c_cmd_handle);
#elif defined(DS1307_PLATFORM_STM32_HAL)
  extern I2C_HandleTypeDef DS1307_HI2C;
  Address <<= 1;
  HAL_I2C_Master_Transmit(&DS1307_HI2C, Address, Data, Len, DS1307_TIMEOUT);
#endif
}

static void
PlatformReceive(uint8_t Address, uint8_t *Data, uint8_t Len)
{
#if defined(DS1307_PLATFORM_AVR)
  uint8_t DataCounter = 0;

  TWCR = _BV(TWEN) | _BV(TWSTA) | _BV(TWEA) | _BV(TWINT); // TWI enable *** acknowledge enable
  while (!CHECKBIT(TWCR, TWINT)); // wait until the process ends

  TWDR = (Address<<1) | 0x01;                  // set data in data register to sending
  TWCR = _BV(TWEN) | _BV(TWEA) | _BV(TWINT); // TWI enable *** acknowledge enable
  while (!CHECKBIT(TWCR, TWINT)); // wait until the process ends

  for (DataCounter = 0; DataCounter < Len - 1; DataCounter++)
  {
    TWCR = _BV(TWEN) | _BV(TWEA) | _BV(TWINT); // TWI enable *** acknowledge enable
    while (!CHECKBIT(TWCR, TWINT)); // wait until the process ends
    Data[DataCounter] = TWDR;
  }
  TWCR = _BV(TWEN) | _BV(TWINT); // TWI enable
  while (!CHECKBIT(TWCR, TWINT)); // wait until the process ends
  Data[DataCounter] = TWDR;

  TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTO); // send the STOP mode bit
#elif defined(DS1307_PLATFORM_ESP32_IDF)
  i2c_cmd_handle_t DS1307_i2c_cmd_handle = 0;
  Address <<= 1;
  Address |= 0x01;

  DS1307_i2c_cmd_handle = i2c_cmd_link_create();
  i2c_master_start(DS1307_i2c_cmd_handle);
  i2c_master_write(DS1307_i2c_cmd_handle, &Address, 1, 1);
  i2c_master_read(DS1307_i2c_cmd_handle, Data, Len, I2C_MASTER_LAST_NACK);
  i2c_master_stop(DS1307_i2c_cmd_handle);
  i2c_master_cmd_begin(DS1307_I2C_NUM, DS1307_i2c_cmd_handle, 1000 / portTICK_RATE_MS);
  i2c_cmd_link_delete(DS1307_i2c_cmd_handle);
#elif defined(DS1307_PLATFORM_STM32_HAL)
  extern I2C_HandleTypeDef DS1307_HI2C;
  Address <<= 1;
  HAL_I2C_Master_Receive(&DS1307_HI2C, Address, Data, Len, DS1307_TIMEOUT);
#endif
}



/**
 ==================================================================================
                            ##### Public Functions #####                           
 ==================================================================================
 */

void
DS1307_Platform_Init(DS1307_Handler_t *Handler)
{
  Handler->PlatformInit = Platform_Init;
  Handler->PlatformDeInit = Platform_DeInit;
  Handler->PlatformSend = Platform_Send;
  Handler->PlatformReceive = PlatformReceive;
}
