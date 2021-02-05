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

/* Define to prevent recursive inclusion ----------------------------------------*/
#ifndef _DS1307_PLATFORM_H_
#define _DS1307_PLATFORM_H_

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ---------------------------------------------------------------------*/
#include <stdint.h>


/* Functionality Options --------------------------------------------------------*/
/**
 * @brief  Specify the target platform
 * @note   Uncomment the line below according to the target platform
 */ 
// #define DS1307_PLATFORM_AVR
// #define DS1307_PLATFORM_ESP32_IDF

#if defined(DS1307_PLATFORM_ESP32_IDF)
/**
 * @brief  Specify I2C options of ESP32
 */
#define DS1307_I2C_Num      1
#define DS1307_I2C_Rate     100000
#define DS1307_SCL_GPIO     22
#define DS1307_SDA_GPIO     21
#endif

#if defined(DS1307_PLATFORM_AVR)
/**
 * @brief  Specify I2C options of AVR
 */
#define DS1307_CPU_CLK      8000000UL
#endif



/* Exported Data Types ----------------------------------------------------------*/
/**
 * @brief  Data type of data transform options
 */
typedef enum
{
  DS1307_ACK        = 0,
  DS1307_NACK       = 1,
  DS1307_LAST_NACK  = 2
} DS1307_ACK_t;


/**
 ==================================================================================
                               ##### Functions #####                               
 ==================================================================================
 */

/**
 * @brief  Initialize I2C communication with DS1307  
 * @retval None
 */
void
DS1307_Platform_Init(void);


/**
 * @brief  Send START message 
 * @retval None
 */
void
DS1307_Platform_Start(void);


/**
 * @brief  Send STOP message
 * @retval None
 */
void
DS1307_Platform_Stop(void);


/**
 * @brief  Send Data to DS1307
 * @param  Data: pointer to data array
 * @param  BytesCount: data size
 * @param  ACK: acknowledgement type
 *              - DS1307_ACK: Check acknowledgement for all bytes
 *              - DS1307_NACK: Don't check acknowledgement for all bytes
 *              - DS1307_LAST_NACK: Don't check acknowledgement just for last byte
 * @retval None
 */
void
DS1307_Platform_WriteBytes(uint8_t *Data, uint8_t BytesCount, DS1307_ACK_t ACK);


/**
 * @brief  Receive data from DS1307 
 * @param  Data: pointer to data array
 * @param  BytesCount: data size
 * @param  ACK: acknowledgement type
 *              - DS1307_ACK: Check acknowledgement for all bytes
 *              - DS1307_NACK: Don't check acknowledgement for all bytes
 *              - DS1307_LAST_NACK: Don't check acknowledgement just for last byte
 * @retval None
 */
void
DS1307_Platform_ReadBytes(uint8_t *Data, uint8_t BytesCount, DS1307_ACK_t ACK);



#ifdef __cplusplus
}
#endif

#endif
