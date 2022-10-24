/**
 * main.c
 * 2022 October 21
 *
 * Description:
 *  This is the test program to demonstrate the usage of the LTC2418 API functions
 *
 * Copyright 2022 Xiaosheng An
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 *
 **/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "ltc241x.h"

// Replace the sleep function as you like (sleep_ms(int ms))
#include "sleep.h"

#define _MAIN_TEST_SINGLE_CHANNEL_ENABLE_ 1
#define _MAIN_TEST_SINGLE_CHANNEL_ 2
#if _MAIN_TEST_SINGLE_CHANNEL_ENABLE_ == 1 && _MAIN_TEST_SINGLE_CHANNEL_ > 15
#error The channel number should be from 0 to 15!
#endif


const static char DEVICE_NAME[] =  "/dev/spidev1.0";

int main( void )
{
	LTC241X_config_t *configuration = malloc(sizeof(LTC241X_config_t));
	int i;
	int32_t *output = malloc(sizeof(int32_t));
	uint16_t sleep_time;
	LTC241X_init(configuration, DEVICE_NAME, true, false);
	
	printf("Calibration in process...\n");
	LTC241X_calibrate(configuration, 3);	// Calibrate the ADC at startup with average of 2. The calibration data will be applied in the later conversions.
	printf("Calibration finished!\n");
	
	sleep_time = configuration->conversion_time * 2; // Make sure it gets enough sleep time for accurate result
	
	#if _MAIN_TEST_SINGLE_CHANNEL_ENABLE_ == 1
	LTC241X_readSingle(configuration, _MAIN_TEST_SINGLE_CHANNEL_, output, 3);
	printf("Channel %d: %d\n", _MAIN_TEST_SINGLE_CHANNEL_, output[0]);
	sleep_ms(sleep_time);
	#elif _MAIN_TEST_SINGLE_CHANNEL_ENABLE_ == 0
	while (1)
	{
		#if LTC241x == LTC2414
		for (i = 0; i < 8; i++)
		#elif LTC241x == LTC2418
		for (i = 0; i < 16; i++)
		#endif
		{
			LTC241X_readSingle(configuration, i, output, 3);
			printf("Channel %d: %d\n", i, output[0]);
			sleep_ms(sleep_time);
		}
	}
	#endif
	
	free(output);
	free(configuration);
	return 0;
}
