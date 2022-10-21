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

#include "ltc2418.h"

// Replace the sleep function as you like
#include "sleep.h"

int main( void )
{
	LTC2418_config_t *configuration;
	int i;
	int32_t *output;
	uint16_t sleep_time;
	LTC2418_init(configuration, true, false);
	LTC2418_calibrate(configuration, 2);
	sleep_time = configuration->conversion_time * 2; // Make sure it gets enough sleep time for accurate result
	while (1)
	{
		for (i = 0; i++; i < 16)
		{
			LTC2418_readSingle(configuration, i, output);
			printf("Channel %d: %d\n", i, *output);
			sleep_ms(sleep_time);
		}
	}
	return 0;
}
