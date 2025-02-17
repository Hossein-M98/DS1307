/**
 **********************************************************************************
 * @file   DS1307_platform.h
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

/* Define to prevent recursive inclusion ----------------------------------------*/
#ifndef _DS1307_PLATFORM_H_
#define _DS1307_PLATFORM_H_

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ---------------------------------------------------------------------*/
#include "xiicps.h"
#include "DS1307.h"


/* Functionality Options --------------------------------------------------------*/
#define IIC_SCLK_RATE		100000
#define I2C_NUM_0			  0



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


#ifdef __cplusplus
}
#endif


#endif //! _DS1307_PLATFORM_H_
