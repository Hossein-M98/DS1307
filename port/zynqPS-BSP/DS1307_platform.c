/*
 * DS1307_Platform.c
 *
 *  Created on: Jan 28, 2025
 *      Author: Hosseinali
 */


#include "DS1307_platform.h"


XIicPs Iic;		/**< Instance of the IIC Device */
XIicPs_Config *Config;

static int8_t
Platform_Init(void){
	int Status;
	/*
	 * Initialize the IIC driver so that it's ready to use
	 * Look up the configuration in the config table,
	 * then initialize it.
	 */
	Config = XIicPs_LookupConfig(I2C_NUM_0);
	if (NULL == Config) {
		return -1;
	}
	Status = XIicPs_CfgInitialize(&Iic, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		return -1;
	}
	/*
	 * Perform a self-test to ensure that the hardware was built correctly.
	 */
	Status = XIicPs_SelfTest(&Iic);
	if (Status != XST_SUCCESS) {
		return -1;
	}
	/*
	 * Set the IIC serial clock rate.
	 */
	XIicPs_SetSClk(&Iic, IIC_SCLK_RATE);
}
static int8_t
Platform_DeInit(void)
{
  return 0;
}

static int8_t
Platform_WriteData(uint8_t Address, uint8_t *Data, uint8_t DataLen){
    if (XIicPs_MasterSendPolled(&Iic, Data, DataLen, Address) != XST_SUCCESS)
        return -1;
	else
		return 0;
}

static int8_t
Platform_ReadData(uint8_t Address, uint8_t *Data, uint8_t DataLen){
    if (XIicPs_MasterRecvPolled(&Iic, Data, DataLen, Address) != XST_SUCCESS)
        return -1;
	else
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

