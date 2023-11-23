/**
 **********************************************************************************
 * @file   main.c
 * @author Hossein.M (https://github.com/Hossein-M98)
 * @brief  example code for DS1307 Driver (for ATmega32)
 **********************************************************************************
 *
 * Copyright (c) 2023 Mahda Embedded System (MIT License)
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

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "Retarget.h"
#include "DS1307.h"
#include "DS1307_platform.h"


int main(void)
{
  DS1307_Handler_t Handler;
  DS1307_DateTime_t DateTime =
  {
    .Second   = 0,
    .Minute   = 2,
    .Hour     = 10,
    .WeekDay  = 5,
    .Day      = 23,
    .Month    = 11,
    .Year     = 23
  };

  Retarget_Init(F_CPU, 9600);
  printf("DS1307 Driver Example\r\n\r\n");

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

    _delay_ms(1000);
  }

  DS1307_DeInit(&Handler);
  return 0;
}
