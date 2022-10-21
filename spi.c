/**
 * spi.c
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <linux/spi/spidev.h>
#include <sys/ioctl.h>

#include "spi.h"

const static spi_config_t spi_config_default = 
{
	.device			= "/dev/spidev1.0",
	.fd				= -1,
	.mode			= 0,
	.lsb			= false,
	.bits_per_word	= 8,
	.block_size		= 1,
	.frequency		= 10000000U,
	.ready			= false
};

/**
 * Get the default SPI configuration.
 *
 **/
spi_config_t SPI_get_default_config( void )
{
	return spi_config_default;
}

/**
 * Init the SPI bus
 *
 * The SPI bus will be configured as described by the `config` input. The device file is opened to get the device file descriptor.
 **/
int SPI_init(spi_config_t *config)
{
	config->fd = open(config->device, O_RDONLY);
	// TODO ... refer to `Write_spi_configuration` in https://github.com/cpb-/spi-tools/blob/master/src/spi-tools.c
}

/**
 * Transfer the data between the SPI master and slave
 *
 * This function transfers the data according to the `config` input. 
 * `tx_buf` is the buffer to be transmitted to the slave.
 * `rx_buf` is the buffer to be received from the slave.
 **/
int SPI_transfer(spi_config_t *config, void *tx_buf, void *rx_buf)
{
	// TODO ... refer to `Transfer_spi_buffers` in https://github.com/cpb-/spi-tools/blob/master/src/spi-tools.c
}