/**
 **********************************************************************************
 * @file   DS1307_platform.c
 * @author Hossein.M (https://github.com/Hossein-M98)
 * @brief  DS1307 chip driver platform dependent part
 *         Functionalities of the this file:
 *          + Initialization the platform device to communicate with DS1307
 *          + Send START and STOP data
 *          + Send data to DS1307
 *          + Receive data from DS1307
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


/* Private variables ------------------------------------------------------------*/
#if defined(DS1307_PLATFORM_ESP32_IDF)
static i2c_cmd_handle_t DS1307_i2c_cmd_handle = 0;
#endif



/**
 ==================================================================================
                            ##### Public Functions #####                           
 ==================================================================================
 */

/**
 * @brief  Initialize I2C communication with DS1307  
 * @retval None
 */
void
DS1307_Platform_Init(void)
{
#if defined(DS1307_PLATFORM_AVR)
  TWBR = (uint8_t)(DS1307_CPU_CLK - 1600000) / (2 * 100000);
#elif defined(DS1307_PLATFORM_ESP32_IDF)
  i2c_config_t conf;
  conf.mode = I2C_MODE_MASTER;
  conf.sda_io_num = DS1307_SDA_GPIO;
  conf.sda_pullup_en = GPIO_PULLUP_DISABLE;
  conf.scl_io_num = DS1307_SCL_GPIO;
  conf.scl_pullup_en = GPIO_PULLUP_DISABLE;
  conf.master.clk_speed = DS1307_I2C_Rate;
  i2c_param_config(DS1307_I2C_Num, &conf);
  i2c_driver_install(DS1307_I2C_Num, conf.mode, 0, 0, 0);
#endif
}


/**
 * @brief  Send START message 
 * @retval None
 */
void
DS1307_Platform_Start(void)
{
#if defined(DS1307_PLATFORM_AVR)
  TWCR = _BV(TWEN) | _BV(TWSTA) | _BV(TWEA) | _BV(TWINT); // TWI enable *** acknowledge enable
  while (!CHECKBIT(TWCR, TWINT)); // wait until the process ends
#elif defined(DS1307_PLATFORM_ESP32_IDF)
  DS1307_i2c_cmd_handle = i2c_cmd_link_create();
  i2c_master_start(DS1307_i2c_cmd_handle);
#endif
}

/**
 * @brief  Send STOP message
 * @retval None
 */
void
DS1307_Platform_Stop(void)
{
#if defined(DS1307_PLATFORM_AVR)
  TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTO); // send the STOP mode bit
#elif defined(DS1307_PLATFORM_ESP32_IDF)
  i2c_master_stop(DS1307_i2c_cmd_handle);
  i2c_master_cmd_begin(DS1307_I2C_Num, DS1307_i2c_cmd_handle, 1000 / portTICK_RATE_MS);
  i2c_cmd_link_delete(DS1307_i2c_cmd_handle);
#endif
}


/**
 * @brief  Send Data to DS1307
 * @param  Data: pointer to data array
 * @param  BytesCount: data size
 * @param  ACK: acknowledgement type
 *         - DS1307_ACK: Check acknowledgement for all bytes
 *         - DS1307_NACK: Don't check acknowledgement for all bytes
 *         - DS1307_LAST_NACK: Don't check acknowledgement just for last byte
 * @retval None
 */
void
DS1307_Platform_WriteBytes(uint8_t *Data, uint8_t BytesCount, DS1307_ACK_t ACK)
{
#if defined(DS1307_PLATFORM_AVR)
  uint8_t DataCounter;

  switch (ACK)
  {
  case DS1307_ACK:
  {
    for (DataCounter = 0; DataCounter < BytesCount; DataCounter++)
    {
      TWDR = Data[DataCounter];  // set data in data register to sending
      TWCR = _BV(TWEN) | _BV(TWEA) | _BV(TWINT); // TWI enable *** acknowledge enable
      while (!CHECKBIT(TWCR, TWINT));
    }
    break;
  }

  case DS1307_NACK:
  {
    for (DataCounter = 0; DataCounter < BytesCount; DataCounter++)
    {
      TWDR = Data[DataCounter];  // set data in data register to sending
      TWCR = _BV(TWEN) | _BV(TWINT); // TWI enable
      while (!CHECKBIT(TWCR, TWINT));
    }
    break;
  }

  case DS1307_LAST_NACK:
  {
    for (DataCounter = 0; DataCounter < BytesCount-1; DataCounter++)
    {
      TWDR = Data[DataCounter];  // set data in data register to sending
      TWCR = _BV(TWEN) | _BV(TWEA) | _BV(TWINT); // TWI enable *** acknowledge enable
      while (!CHECKBIT(TWCR, TWINT));
    }
    TWDR = Data[DataCounter];  // set data in data register to sending
    TWCR = _BV(TWEN) | _BV(TWINT); // TWI enable
    while (!CHECKBIT(TWCR, TWINT));
    break;
  }

  default:
    break;
  }
#elif defined(DS1307_PLATFORM_ESP32_IDF)
  i2c_master_write(DS1307_i2c_cmd_handle, Data, BytesCount, ACK);
#endif
}


/**
 * @brief  Receive data from DS1307 
 * @param  Data: pointer to data array
 * @param  BytesCount: data size
 * @param  ACK: acknowledgement type
 *         - DS1307_ACK: Check acknowledgement for all bytes
 *         - DS1307_NACK: Don't check acknowledgement for all bytes
 *         - DS1307_LAST_NACK: Don't check acknowledgement just for last byte
 * @retval None
 */
void
DS1307_Platform_ReadBytes(uint8_t *Data, uint8_t BytesCount, DS1307_ACK_t ACK)
{
#if defined(DS1307_PLATFORM_AVR)
  uint8_t DataCounter;

  switch (ACK)
  {
  case DS1307_ACK:
  {
    for (DataCounter = 0; DataCounter < BytesCount; DataCounter++)
    {
      TWCR = _BV(TWEN) | _BV(TWEA) | _BV(TWINT);  // TWI enable *** acknowledge enable
      while (!CHECKBIT(TWCR, TWINT)); // wait until the process ends
      Data[DataCounter] = TWDR;
    }
    break;
  }

  case DS1307_NACK:
  {
    for (DataCounter = 0; DataCounter < BytesCount; DataCounter++)
    {
      TWCR = _BV(TWEN) | _BV(TWINT);  // TWI enable
      while (!CHECKBIT(TWCR, TWINT)); // wait until the process ends
      Data[DataCounter] = TWDR;
    }
    break;
  }

  case DS1307_LAST_NACK:
  {
    for (DataCounter = 0; DataCounter < BytesCount-1; DataCounter++)
    {
      TWCR = _BV(TWEN) | _BV(TWEA) | _BV(TWINT);  // TWI enable *** acknowledge enable
      while (!CHECKBIT(TWCR, TWINT)); // wait until the process ends
      Data[DataCounter] = TWDR;
    }
    TWCR = _BV(TWEN) | _BV(TWINT);  // TWI enable
    while (!CHECKBIT(TWCR, TWINT)); // wait until the process ends
    Data[DataCounter] = TWDR;
    break;
  }

  default:
    break;
  }
#elif defined(DS1307_PLATFORM_ESP32_IDF)
  i2c_master_read(DS1307_i2c_cmd_handle, Data, BytesCount, ACK);
#endif
}
