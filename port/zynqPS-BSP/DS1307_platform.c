/**
 **********************************************************************************
 * @file   DS1307_platform.c
 * @author Hosseinali (https://github.com/iamhosseinali)
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


/* Private Variables ------------------------------------------------------------*/
static IicPs Iic; /**< Instance of the IIC Device */
static IicPs_Config *Config;



/**
 ==================================================================================
                           ##### Private Functions #####                           
 ==================================================================================
 */

static int8_t
Platform_Init(void)
{
  int Status = 0;
  /*
   * Initialize the IIC driver so that it's ready to use
   * Look up the configuration in the config table,
   * then initialize it.
   */
  Config = XIicPs_LookupConfig(I2C_NUM_0);
  if (NULL == Config)
  {
    return -1;
  }
  Status = XIicPs_CfgInitialize(&Iic, Config, Config->BaseAddress);
  if (Status != XST_SUCCESS)
  {
    return -1;
  }
  /*
   * Perform a self-test to ensure that the hardware was built correctly.
   */
  Status = XIicPs_SelfTest(&Iic);
  if (Status != XST_SUCCESS)
  {
    return -1;
  }
  /*
   * Set the IIC serial clock rate.
   */
  XIicPs_SetSClk(&Iic, IIC_SCLK_RATE);

  return 0;
}

static int8_t
Platform_DeInit(void)
{
  return 0;
}

static int8_t
Platform_WriteData(uint8_t Address, uint8_t *Data, uint8_t DataLen)
{
  if (XIicPs_MasterSendPolled(&Iic, Data, DataLen, Address) != XST_SUCCESS)
    return -1;

  return 0;
}

static int8_t
Platform_ReadData(uint8_t Address, uint8_t *Data, uint8_t DataLen)
{
  if (XIicPs_MasterRecvPolled(&Iic, Data, DataLen, Address) != XST_SUCCESS)
    return -1;

  return 0;
}



/**
 ==================================================================================
                            ##### Public Functions #####                           
 ==================================================================================
 */

/**
 * @brief  Initialize platform device to communicate DS1307.
 * @param  Handler: Pointer to handler
 * @retval None
 */
void
DS1307_Platform_Init(DS1307_Handler_t *Handler)
{
  Handler->PlatformInit = Platform_Init;
  Handler->PlatformDeInit = Platform_DeInit;
  Handler->PlatformSend = Platform_WriteData;
  Handler->PlatformReceive = Platform_ReadData;
}
