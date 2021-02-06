/**
 **********************************************************************************
 * @file   DS1307.h
 * @author Hossein.M (https://github.com/Hossein-M98)
 * @brief  DS1307 chip driver
 *         Functionalities of the this file:
 *          + Get and Set date and time
 *          + Read and Write non-volatile embedded RAM of DS1307 chip
 *          + Control squarewave output signal
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
#ifndef _DS1307_H_
#define _DS1307_H_

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ---------------------------------------------------------------------*/
#include <stdint.h>


/* Exported Data Types ----------------------------------------------------------*/
/**
 * @brief  Date and time data type
 */
typedef struct
{
  uint8_t   Second;
  uint8_t   Minute;
  uint8_t   Hour;
  uint8_t   WeekDay;
  uint8_t   Day;
  uint8_t   Month;
  uint8_t   Year;
} DS1307_DateTime_t;

/**
 * @brief  squarewave output signal options
 */
typedef enum
{
  DS1307_OutWave_Low    = 0,  // Logic level 0 on the SQW/OUT pin
  DS1307_OutWave_High   = 1,  // Logic level 1 on the SQW/OUT pin
  DS1307_OutWave_1Hz    = 2,  // Output wave frequency = 1Hz
  DS1307_OutWave_4KHz   = 3,  // Output wave frequency = 4.096KHz
  DS1307_OutWave_8KHz   = 4,  // Output wave frequency = 8.192KHz
  DS1307_OutWave_32KHz  = 5   // Output wave frequency = 32.768KHz
} DS1307_OutWave_t;



/**
 ==================================================================================
                           ##### Common Functions #####                           
 ==================================================================================
 */

/**
 * @brief  Initialize DS1307  
 * @retval None
 */
void
DS1307_Init(void);



/**
 ==================================================================================
                             ##### RTC Functions #####                             
 ==================================================================================
 */

/**
 * @brief  Set date and time on DS1307 real time chip
 * @param  DateTime: pointer to date and time value structure
 * @retval None
 */
void
DS1307_SetDateTime(DS1307_DateTime_t *DateTime);


/**
 * @brief  Get date and time from DS1307 real time chip
 * @param  DateTime: pointer to date and time value structure
 * @retval None
 */
void
DS1307_GetDateTime(DS1307_DateTime_t *DateTime);



/**
 ==================================================================================
                           ##### Memory Functions #####                            
 ==================================================================================
 */

/**
 * @brief  Write data on DS1307 data Non-volatile RAM
 * @param  Address: address of block beginning (0 to 55)
 * @param  Data: pointer to data array
 * @param  Size: data size (1 to 56)
 * @retval None
 */
void
DS1307_WriteRAM(uint8_t Address, uint8_t *Data, uint8_t Size);


/**
 * @brief  Read data from DS1307 data Non-volatile RAM
 * @param  Address: address of block beginning (0 to 55)
 * @param  Data: pointer to data array
 * @param  Size: data size (1 to 56)
 * @retval None
 */
void
DS1307_ReadRAM(uint8_t Address, uint8_t *Data, uint8_t Size);



/**
 ==================================================================================
                          ##### Out Wave Functions #####                           
 ==================================================================================
 */

/**
 * @brief  Set output Wave on SQW/Out pin of DS1307  
 * @param  OutWave: where OutWave Shows different output wave states
 *         - DS1307_OutWave_Low:    Logic level 0 on the SQW/OUT pin
 *         - DS1307_OutWave_High:   Logic level 1 on the SQW/OUT pin
 *         - DS1307_OutWave_1Hz:    Output wave frequency = 1Hz
 *         - DS1307_OutWave_4KHz:   Output wave frequency = 4.096KHz
 *         - DS1307_OutWave_8KHz:   Output wave frequency = 8.192KHz
 *         - DS1307_OutWave_32KHz:  Output wave frequency = 32.768KHz
 * @retval None
 */
void
DS1307_SetOutWave(DS1307_OutWave_t OutWave);



#ifdef __cplusplus
}
#endif

#endif
