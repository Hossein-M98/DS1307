/**
 **********************************************************************************
 * @file   DS1307.c
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

/* Includes ---------------------------------------------------------------------*/
#include <string.h>
#include "DS1307.h"


/* Private Constants ------------------------------------------------------------*/
/**
 * @brief  The DS1307 Address on I2C BUS
 */ 
#define DS1307_ADDRESS  0x68

/**
 * @brief  Internal Registers Address
 */ 
#define DS1307_SECOND   0x00  // the address of SECOND register in DS1307
#define DS1307_MINUTE   0x01  // the address of MINUTE register in DS1307
#define DS1307_HOUR     0x02  // the address of HOUR register in DS1307
#define DS1307_DAY      0x03  // the address of DAY register in DS1307
#define DS1307_DATE     0x04  // the address of DATE register in DS1307
#define DS1307_MONTH    0x05  // the address of MONTH register in DS1307
#define DS1307_YEAR     0x06  // the address of YEAR register in DS1307
#define DS1307_CONTROL  0x07  // the address of CONTROL register in DS1307

/**
 * @brief  Non-volatile RAM Address
 */ 
#define DS1307_RAM      0x08  // the address of first byte of Non-volatile RAM
#define DS1307_RAM_SIZE 56    // size of Non-volatile

/**
 * @brief  CONTROL register bits
 */ 
#define DS1307_OUT      7
#define DS1307_SQWE     4
#define DS1307_RS0      0
#define DS1307_RS1      1


/* Private Macro ----------------------------------------------------------------*/
#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif


/**
 ==================================================================================
                           ##### Private Functions #####                           
 ==================================================================================
 */

static uint8_t
DS1307_DECtoBCD(uint8_t DEC)
{
  uint8_t Buff2 = DEC % 10;
  uint8_t Buff1 = (DEC / 10) % 10;
  uint8_t BCD   = (Buff1 << 4) | Buff2;

  return BCD;
}

static uint8_t
DS1307_BCDtoDEC(uint8_t BCD)
{
  uint8_t Buff1 = BCD >> 4;
  uint8_t Buff2 = BCD & 0x0f;
  uint8_t DEC   = (Buff1 * 10) + Buff2;

  return DEC;
}

static int8_t
DS1307_WriteRegs(DS1307_Handler_t *Handler,
                 uint8_t StartReg, uint8_t *Data, uint8_t BytesCount)
{
  uint8_t Buffer[DS1307_SEND_BUFFER_SIZE];
  uint8_t Len = 0;

  Buffer[0] = StartReg; // send register address to set RTC pointer
  while (BytesCount)
  {
    Len = MIN(BytesCount, sizeof(Buffer)-1);
    memcpy((void*)(Buffer+1), (const void*)Data, Len);

    if (Handler->PlatformSend(DS1307_ADDRESS, Buffer, Len+1) < 0)
      return -1;

    Data += Len;
    Buffer[0] += Len;
    BytesCount -= Len;
  }

  return 0;
}

static int8_t
DS1307_ReadRegs(DS1307_Handler_t *Handler,
                uint8_t StartReg, uint8_t *Data, uint8_t BytesCount)
{
  if (Handler->PlatformSend(DS1307_ADDRESS, &StartReg, 1) < 0)
    return -1;

  if (Handler->PlatformReceive(DS1307_ADDRESS, Data, BytesCount) < 0)
    return -1;

  return 0;
}



/**
 ==================================================================================
                       ##### Public Common Functions #####                         
 ==================================================================================
 */

/**
 * @brief  Initialize DS1307 
 * @param  Handler: Pointer to handler
 * @retval DS1307_Result_t
 *         - DS1307_OK: Operation was successful.
 *         - DS1307_FAIL: Failed to send or receive data.
 *         - DS1307_INVALID_PARAM: One of parameters is invalid.
 */
DS1307_Result_t
DS1307_Init(DS1307_Handler_t *Handler)
{
  if (!Handler->PlatformSend ||
      !Handler->PlatformReceive)
    return DS1307_INVALID_PARAM;

  if (Handler->PlatformInit)
    if (Handler->PlatformInit() < 0)
      return DS1307_FAIL;

  return DS1307_OK;
}

/**
 * @brief  Uninitialize DS1307 
 * @param  Handler: Pointer to handler
 * @retval DS1307_Result_t
 *         - DS1307_OK: Operation was successful.
 *         - DS1307_FAIL: Failed to send or receive data.
 */
DS1307_Result_t
DS1307_DeInit(DS1307_Handler_t *Handler)
{
  if (Handler->PlatformDeInit)
    if (Handler->PlatformDeInit() < 0)
      return DS1307_FAIL;

  return DS1307_OK;
}



/**
 ==================================================================================
                         ##### Public RTC Functions #####                          
 ==================================================================================
 */

/**
 * @brief  Set date and time on DS1307 real time chip
 * @param  Handler: Pointer to handler
 * @param  DateTime: pointer to date and time value structure
 * @retval DS1307_Result_t
 *         - DS1307_OK: Operation was successful.
 *         - DS1307_FAIL: Failed to send or receive data.
 *         - DS1307_INVALID_PARAM: One of parameters is invalid.
 */
DS1307_Result_t
DS1307_SetDateTime(DS1307_Handler_t *Handler, DS1307_DateTime_t *DateTime)
{
  uint8_t Buffer[7] = {0};

  if (DateTime->Second > 59 ||
      DateTime->Minute > 59 ||
      DateTime->Hour > 23 ||
      DateTime->WeekDay > 7 || DateTime->WeekDay == 0 ||
      DateTime->Day > 31 || DateTime->Day == 0 ||
      DateTime->Month > 12 || DateTime->Month == 0 ||
      DateTime->Year > 99)
    return DS1307_INVALID_PARAM;

  // convert value of parameter to BCD
  Buffer[0] = DS1307_DECtoBCD(DateTime->Second) & 0x7F;
  Buffer[1] = DS1307_DECtoBCD(DateTime->Minute);
  Buffer[2] = DS1307_DECtoBCD(DateTime->Hour);
  Buffer[3] = DS1307_DECtoBCD(DateTime->WeekDay);
  Buffer[4] = DS1307_DECtoBCD(DateTime->Day);
  Buffer[5] = DS1307_DECtoBCD(DateTime->Month);
  Buffer[6] = DS1307_DECtoBCD(DateTime->Year);

  if (DS1307_WriteRegs(Handler, DS1307_SECOND, Buffer, 7) < 0)
    return DS1307_FAIL;
  
  return DS1307_OK;
}


/**
 * @brief  Get date and time from DS1307 real time chip
 * @param  Handler: Pointer to handler
 * @param  DateTime: pointer to date and time value structure
 * @retval DS1307_Result_t
 *         - DS1307_OK: Operation was successful.
 *         - DS1307_FAIL: Failed to send or receive data.
 */
DS1307_Result_t
DS1307_GetDateTime(DS1307_Handler_t *Handler, DS1307_DateTime_t *DateTime)
{
  uint8_t Buffer[7] = {0};

  if (DS1307_ReadRegs(Handler, DS1307_SECOND, Buffer, 7) < 0)
    return DS1307_FAIL;

  // convert BCD value to decimal
  DateTime->Second  = DS1307_BCDtoDEC(Buffer[0] & 0x7F);
  DateTime->Minute  = DS1307_BCDtoDEC(Buffer[1]);
  DateTime->Hour    = DS1307_BCDtoDEC(Buffer[2]);
  DateTime->WeekDay = DS1307_BCDtoDEC(Buffer[3]);
  DateTime->Day     = DS1307_BCDtoDEC(Buffer[4]);
  DateTime->Month   = DS1307_BCDtoDEC(Buffer[5]);
  DateTime->Year    = DS1307_BCDtoDEC(Buffer[6]);

  return DS1307_OK;
}



/**
 ==================================================================================
                       ##### Public Memory Functions #####                         
 ==================================================================================
 */

/**
 * @brief  Write data on DS1307 data Non-volatile RAM
 * @param  Handler: Pointer to handler
 * @param  Address: address of block beginning (0 to 55)
 * @param  Data: pointer to data array
 * @param  Size: data size (1 to 56)
 * @retval DS1307_Result_t
 *         - DS1307_OK: Operation was successful.
 *         - DS1307_FAIL: Failed to send or receive data.
 *         - DS1307_INVALID_PARAM: Requested area is out of range.
 */
DS1307_Result_t
DS1307_WriteRAM(DS1307_Handler_t *Handler,
                uint8_t Address, uint8_t *Data, uint8_t Size)
{
  Address += 8;

  if ((Address + Size) > 0x3f)
    return DS1307_INVALID_PARAM;

  if (DS1307_WriteRegs(Handler, Address, Data, Size) < 0)
    return DS1307_FAIL;

  return DS1307_OK;
}


/**
 * @brief  Read data from DS1307 data Non-volatile RAM
 * @param  Handler: Pointer to handler
 * @param  Address: address of block beginning (0 to 55)
 * @param  Data: pointer to data array
 * @param  Size: data size (1 to 56)
 * @retval DS1307_Result_t
 *         - DS1307_OK: Operation was successful.
 *         - DS1307_FAIL: Failed to send or receive data.
 *         - DS1307_INVALID_PARAM: Requested area is out of range.
 */
DS1307_Result_t
DS1307_ReadRAM(DS1307_Handler_t *Handler,
               uint8_t Address, uint8_t *Data, uint8_t Size)
{
  Address += 8;

  if ((Address + Size) > 0x3f)
    return DS1307_INVALID_PARAM;

  if (DS1307_ReadRegs(Handler, Address, Data, Size) < 0)
    return DS1307_FAIL;

  return DS1307_OK;
}



/**
 ==================================================================================
                     ##### Public Out Wave Functions #####                         
 ==================================================================================
 */

/**
 * @brief  Set output Wave on SQW/Out pin of DS1307
 * @param  Handler: Pointer to handler
 * @param  OutWave: where OutWave Shows different output wave states
 *         - DS1307_OutWave_Low:    Logic level 0 on the SQW/OUT pin
 *         - DS1307_OutWave_High:   Logic level 1 on the SQW/OUT pin
 *         - DS1307_OutWave_1Hz:    Output wave frequency = 1Hz
 *         - DS1307_OutWave_4KHz:   Output wave frequency = 4.096KHz
 *         - DS1307_OutWave_8KHz:   Output wave frequency = 8.192KHz
 *         - DS1307_OutWave_32KHz:  Output wave frequency = 32.768KHz
 * 
 * @retval DS1307_Result_t
 *         - DS1307_OK: Operation was successful.
 *         - DS1307_FAIL: Failed to send or receive data.
 *         - DS1307_INVALID_PARAM: One of parameters is invalid.
 */
DS1307_Result_t
DS1307_SetOutWave(DS1307_Handler_t *Handler, DS1307_OutWave_t OutWave)
{
  uint8_t ControlReg;

  switch (OutWave)
  {
  case DS1307_OutWave_Low:
    ControlReg = 0;
    break;

  case DS1307_OutWave_High:
    ControlReg = (1 << DS1307_OUT);
    break;

  case DS1307_OutWave_1Hz:
    ControlReg = (1 << DS1307_SQWE);
    break;

  case DS1307_OutWave_4KHz:
    ControlReg = (1 << DS1307_SQWE) | (1 << DS1307_RS0);
    break;

  case DS1307_OutWave_8KHz:
    ControlReg = (1 << DS1307_SQWE) | (1 << DS1307_RS1);
    break;

  case DS1307_OutWave_32KHz:
    ControlReg = (1 << DS1307_SQWE) | (3 << DS1307_RS0);
    break;

  default:
    return DS1307_INVALID_PARAM;
  }

  if (DS1307_WriteRegs(Handler, DS1307_CONTROL, &ControlReg, 1) < 0)
    return DS1307_FAIL;

  return DS1307_OK;
}
