/**
 * ltc2418.c
 * 2022 October 21
 *
 * Description:
 *  This is the SPI API source file. The default SPI config is defined here. 
 *  This only contains necessary functions for LTC2418 ADC communications.
 *
 * Copyright 2022 Xiaosheng An
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 *
 **/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ltc2418.h"
#include "spi.h"

// Replace the sleep function as you like (sleep_ms(int ms))
#include "sleep.h"

static const char *addresses[16] = {
	"10000", "11000", "10001", "11001", "10010", "11010", "10011", "11011", "10100", "11100", "10101", "11101", "10110", "11110", "10111", "11111"
};

uint8_t fromBinary(const char *s) {
  return (uint8_t) strtoul(s, NULL, 2);
}

/**
 * Init the SPI bus for LTC2418 ADC IC.
 *
 * `config` is the empty LTC2418_config_t variable that would be set by the two 
 * `internal` determines whether the internal oscillator is used or not. If not, pull-down the SCK line.
 * `differential` determines whether the channel input is differential.
 **/
int LTC2418_init(LTC2418_config_t *config, const char* device, bool internal, bool differential)
{
	config->internal_osc = internal;
	if (internal)
	{
		config->frequency = INTERNAL_LTC2418_SCK_FREQ;
		config->conversion_time = INTERNAL_MAX_CONVERSION_TIME_MS;
	}
	else
	{
		config->frequency = CALC_LTC2418_SCK_FREQ(EXT_OSC_FREQ);
		config->conversion_time = CALC_MAX_CONVERSION_TIME_MS(EXT_OSC_FREQ);
	}
	config->differential = differential;
	memset(config->calibration, 0, sizeof(config->calibration));
	
	*(config->spi) = SPI_get_default_config();
	strcpy(config->spi->device, device);
	config->spi->lsb = false;
	config->spi->block_size = 4;
	config->spi->frequency = config->frequency;
	
	SPI_init(config->spi);
}

/**
 * Calibrate all the 16 channels with the offset.
 *
 * `loop` determins how many times a single channel is read and averaged.
 **/
int LTC2418_calibrate(LTC2418_config_t *config, int32_t loop)
{
	int i, j;
	int reading, total = 0;
	for (i = 0; i < 16; i++)
	{
		total = 0;
		for (j = 0; j < loop; j++)
		{
			LTC2418_readSingle(config, i, &reading, 3);
			sleep_ms(config->conversion_time * 2);
			total += reading;
		}
		config->calibration[i] = total / loop;
	}
	
	#ifdef _SPI_VERBOSE_
	printf("\nCalibration done! Calibration data as following:\n");
	for (i = 0;i < 16; i++)
	{
		printf("Calibration %d: %d\n", i, config->calibration[i]);
	}
	printf("\n");
	#endif
}

/**
 * This is to decode the input to the LTC2418 according to the requested channel and pre-defined configs
 *
 **/
uint8_t LTC2418_encodeDataIn(const LTC2418_config_t *config, int channel)
{
	char *data;
	uint8_t ret;
	data = malloc(8 * sizeof(char));
	strcpy(data, "101");
	
	if (!config->differential)	// Single-ended
	{
		strcat(data, addresses[channel]);
	}
	else						// Differential
	{
		// TODO: to be implemented...
	}
	ret = fromBinary(data);
	
	free(data);
	return ret;
}

/**
 * Check 32-bit parity
 *  Credit: https://graphics.stanford.edu/~seander/bithacks.html##ParityMultiply
 *
 * Return 0 if even number of 1.
 * Return 1 if odd number of 1.
 **/
unsigned int check_parity(unsigned int v)
{
	v ^= v >> 1;
	v ^= v >> 2;
	v = (v & 0x11111111U) * 0x11111111U;
	return (v >> 28) & 1;
}

/**
 * Perform a single read on a specific channel from 0 to 15. The output data is loaded into `output` argument.
 * The `attempts` parameter determines how many times of recursion maximum shall be called before returning error code.
 *
 * Return -2 if the channel address does not match.
 * Return -1 if Even parity is not met.
 * Return 0 if OK.
 **/
int LTC2418_readSingle(LTC2418_config_t *config, int channel, int32_t *output, int attempts)
{
	uint8_t *tx_buf;// {LTC2418_encodeDataIn(config, channel), 0, 0, 0};
	uint8_t *rx_buf;
	uint32_t result;
	int32_t sign = -1, uvalue_out;
	uint8_t address;
	
	tx_buf = malloc(4 * sizeof(uint8_t));
	rx_buf = malloc(4 * sizeof(uint8_t));
	
	memset(tx_buf, LTC2418_encodeDataIn(config, channel), sizeof(uint8_t));
	memset(tx_buf + 1, 0, 3 * sizeof(uint8_t));
	
	memset(rx_buf, 0, 4 * sizeof(uint8_t));
	
	SPI_transfer(config->spi, tx_buf, rx_buf);
	
	result = ((uint32_t)rx_buf[0] << 24) | ((uint32_t)rx_buf[1] << 16) | ((uint32_t)rx_buf[2] << 8) | ((uint32_t)rx_buf[3] << 0);
	
	if (check_parity(result) == 1)
	{
		#ifdef _SPI_VERBOSE_
		printf("Pariity attempt %d\n", attempts);
		#endif
		if (attempts < 0) {return -1;}
		else {
			sleep_ms(config->conversion_time * 2);
			return LTC2418_readSingle(config, channel, output, attempts - 1);
		}
	}
	
	if (result & (1 << 29)) {sign = 1;}
	
	address = (uint8_t)((result >> 1) & 31); // 0b11111
	if (address != fromBinary(addresses[channel]))
	{
		#ifdef _SPI_VERBOSE_
		printf("Address attempt %d\n", attempts);
		#endif
		if (attempts < 0) {return -2;}
		else
		{
			sleep_ms(config->conversion_time * 2);
			return LTC2418_readSingle(config, channel, output, attempts - 1);
		}
	}
	uvalue_out = (int32_t)((result >> 6) & 8388607); // 0b11111111111111111111111 -> 23-bit
	*output = sign * uvalue_out - config->calibration[channel];
	
	#ifdef _SPI_VERBOSE_
	printf("raw: %x\n", result);
	printf("output: %d\n", *output);
	#endif
	
	return 0;
}