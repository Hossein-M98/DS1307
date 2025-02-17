/*
 * DS1307_Platform.h
 *
 *  Created on: Jan 28, 2025
 *      Author: Hosseinali
 */

#ifndef SRC_DS1307_PLATFORM_H_
#define SRC_DS1307_PLATFORM_H_

#include "xiicps.h"
#include "DS1307.h"

#define IIC_SCLK_RATE		100000
#define I2C_NUM_0			0

/**
 ==================================================================================
                             ##### Functions #####                                 
 ==================================================================================
 */

/**
 * @brief  Initialize platform device to communicate DS1307.
 * @param  Handler: Pointer to handler
 * @retval None
 */
void
DS1307_Platform_Init(DS1307_Handler_t *Handler);

#endif /* SRC_DS1307_PLATFORM_H_ */
