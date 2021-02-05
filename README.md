# DS1307 Library
Library for handling DS1307 Real Time Clock chip.

## Library Features
-   Time and date management
-   non-volatile internal RAM management
-   Output square wave management

## Hardware Support
-   AVR (ATMega32)
-   ESP32 (esp-idf)

## How To Use
 1. Add library files to your project
 2. Config `DS1307_platform.h`
 3. Call `DS1307_Init()` to initialize library
 4. Call other functions and enjoy

## Example
```C
#include <stdio.h>
#include "DS1307.h"

int main(void)
{
	DS1307_DateTime_t DateTime =
	{
		.Second   = 0,
		.Minute   = 18,
		.Hour     = 0,
		.WeekDay  = 6,
		.Day      = 6,
		.Month    = 2,
		.Year     = 21
	};
	
	DS1307_Init();
	DS1307_SetDateTime(&DataTime);
	DS1307_SetOutWave(DS1307_OutWave_1Hz);
	
	while(1)
	{
		DS1307_GetDateTime(&DataTime);
		printf("Date: 20%02u/%02u/%02u\r\n", DataTime.Year, DataTime.Month, DataTime.Day);
		printf("Time: %02u:%02u:%02u\r\n", DataTime.Hour, DataTime.Minute, DataTime.Second);
		printf("WeekDay: %u\r\n", DataTime.WeekDay);
	}
}
```