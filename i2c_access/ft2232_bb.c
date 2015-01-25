/* This program is distributed under the GPL, version 2 */

/*

http://www.embeddedrelated.com/showcode/334.php

I2C driver using bit bang. This can be used with any microcontroller which has 2 GPIO lines which can be configured as input/output

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ftdi.h>

#define HIGH 1
#define LOW  0

#include "common.h"
#include "i2c_master.h"

struct ft2232_bb {
	struct i2c_master master;

	struct ftdi_context *ftdi;
	uint8_t scl_mask;
	uint8_t sda_mask;
	int usleep;

	uint8_t dir_mask;
	int sda_state;
	int scl_state;
};

static inline struct ft2232_bb *master_to_ft2232_bb(struct i2c_master *c)
{
	return container_of(c, struct ft2232_bb, master);
}

static inline int mpsse_flush(struct ftdi_context *ftdi)
{
	uint8_t buf[1];
	int ret;

	buf[0] = SEND_IMMEDIATE;

	ret = ftdi_write_data(ftdi, buf, 1);

	return ret;
}

static int mpsse_set_data_bits_high_byte(struct ftdi_context *ftdi,
					uint8_t data, uint8_t dir)
{
	uint8_t buf[3];
	int ret;

	buf[0] = SET_BITS_HIGH;
	buf[1] = data;
	buf[2] = dir;

	ret = ftdi_write_data(ftdi, buf, 3);

	ret = mpsse_flush(ftdi);

	return ret;
}

static int mpsse_read_data_bits_high_byte(struct ft2232_bb *ftbb, uint8_t *data)
{
	struct ftdi_context *ftdi = ftbb->ftdi;
	uint8_t obuf;
	int ret;

	obuf = GET_BITS_HIGH;
	ret = ftdi_write_data(ftdi, &obuf, 1);

	ret = ftdi_read_data(ftdi, data, 1);

	return ret;
}

static inline void ft2232_bb_update_data_mask(struct ft2232_bb *ftbb)
{
	ftbb->dir_mask = 0;

	if (!ftbb->sda_state) {
		ftbb->dir_mask |= ftbb->sda_mask;
	}

	if (!ftbb->scl_state) {
		ftbb->dir_mask |= ftbb->scl_mask;
	}
}

static inline void ft2232_bb_setpins(struct ft2232_bb *ftbb)
{
	ft2232_bb_update_data_mask(ftbb);

	/*
	 * I2C lines are pull-upped so
	 * always set output pins to 0.
	 * If we want set output line to 1
	 * then we simply setup it as an input line.
	 */
	mpsse_set_data_bits_high_byte(ftbb->ftdi, 0, ftbb->dir_mask);

	usleep(ftbb->usleep);
}

static void i2c_setsda(struct ft2232_bb *ftbb, int state)
{
	if (ftbb->sda_state != state) {
		ftbb->sda_state = state;
		ft2232_bb_setpins(ftbb);
	}
}

static void i2c_setscl(struct ft2232_bb *ftbb, int state)
{
	if (ftbb->scl_state != state) {
		ftbb->scl_state = state;
		ft2232_bb_setpins(ftbb);
	}
}

static int i2c_getsda(struct ft2232_bb *ftbb)
{
	uint8_t t;

	if (ftbb->sda_state == LOW) {
		printf("\n");
		printf("SDA is driven low: can't read SDA\n");
		BUG();
	}

	mpsse_read_data_bits_high_byte(ftbb, &t);

	if (t & ftbb->sda_mask)
		return 1;

	return 0;
}

static void i2c_wait_scl_high(struct ft2232_bb *ftbb)
{
	uint8_t t;

	if (ftbb->scl_state == LOW) {
		printf("\n");
		printf("SCL is driven low: can't wait high\n");
		BUG();
	}

	/* FIXME: add timeout */
	do {
		mpsse_read_data_bits_high_byte(ftbb, &t);
	} while (!(t & ftbb->scl_mask));
}

static void ft2232_bb_i2c_start(struct i2c_master *master)
{
	struct ft2232_bb *ftbb = master_to_ft2232_bb(master);

	/* I2C Start condition, data line goes low when clock is high */
	i2c_setsda(ftbb, HIGH);
	i2c_setscl(ftbb, HIGH);
	i2c_setsda(ftbb, LOW);
	i2c_setscl(ftbb, LOW);
}

static void ft2232_bb_i2c_stop(struct i2c_master *master)
{
	struct ft2232_bb *ftbb = master_to_ft2232_bb(master);

	/* I2C Stop condition, clock goes high when data is low */
	i2c_setscl(ftbb, LOW);
	i2c_setsda(ftbb, LOW);
	i2c_setscl(ftbb, HIGH);
	i2c_setsda(ftbb, HIGH);
}

static int ft2232_bb_i2c_rawwrite(struct i2c_master *master, uint8_t data)
{
	struct ft2232_bb *ftbb = master_to_ft2232_bb(master);
	int outBits;
	int ack;

	for (outBits = 0; outBits < 8; outBits++) {
		if (data & 0x80) {
			i2c_setsda(ftbb, HIGH);
		} else {
			i2c_setsda(ftbb, LOW);
		}

		data <<= 1;

		/* Generate clock for 8 data bits */
		i2c_setscl(ftbb, HIGH);
		i2c_setscl(ftbb, LOW);
	}

	/* Neitralize sda */
	i2c_setsda(ftbb, HIGH);

	/* Generate clock for ACK */
	i2c_setscl(ftbb, HIGH);

	/* Wait for clock to go high, clock stretching */
	i2c_wait_scl_high(ftbb);

	/* Clock high, check ACK */
	ack = i2c_getsda(ftbb);

	i2c_setscl(ftbb, LOW);

	return ack;
}

static void ft2232_bb_i2c_rawread(struct i2c_master *master,
				uint8_t *data, int need_ack)
{
	struct ft2232_bb *ftbb = master_to_ft2232_bb(master);
	uint8_t inData;
	int t;

	i2c_setsda(ftbb, HIGH);

	inData = 0x00;
	/* 8 bits */
	for (t = 0; t < 8; t++) {
		i2c_setscl(ftbb, LOW);
		i2c_setscl(ftbb, HIGH);
		i2c_wait_scl_high(ftbb);
		inData = (inData << 1) | i2c_getsda(ftbb);
	}

	i2c_setscl(ftbb, LOW);
	if (need_ack) {
		i2c_setsda(ftbb, LOW);
	} else {
		i2c_setsda(ftbb, HIGH);
	}

	i2c_setscl(ftbb, HIGH);

	/* Wait for clock to go high, clock stretching */
	i2c_wait_scl_high(ftbb);
	i2c_setscl(ftbb, LOW);

	*data = inData;

	//printf("ft2232_bb_i2c_read: %02x\n", inData);
}

static struct i2c_master *ft2232_bb_probe(void)
{
	struct ftdi_context *ftdi;
	int ret;
	struct ft2232_bb *ftbb;

	if ((ftdi = ftdi_new()) == 0) {
		fprintf(stderr, "ftdi_new failed\n");
		goto error;
	}

	ret = ftdi_usb_open(ftdi, 0x0403, 0x6010);
	if (ret  < 0 && ret != -5) {
		fprintf(stderr, "unable to open ftdi device: %d (%s)\n",
			ret, ftdi_get_error_string(ftdi));
		goto error;
	}

	ftdi_set_interface(ftdi, INTERFACE_A);
	ftdi_set_bitmode(ftdi, 0x00, BITMODE_MPSSE);

	ftbb = xzalloc(sizeof(struct ft2232_bb));

	ftbb->ftdi = ftdi;
	ftbb->dir_mask = 0;
	/* < 100 kHz */
	ftbb->usleep = 10;
	ftbb->scl_mask = (1 << 5);
	ftbb->sda_mask = (1 << 6);

	/* reset pins to default neutral state */
	ftbb->sda_state = HIGH;
	ftbb->scl_state = HIGH;
	ft2232_bb_setpins(ftbb);

	ftbb->master.start = ft2232_bb_i2c_start;
	ftbb->master.stop = ft2232_bb_i2c_stop;
	ftbb->master.rawwrite = ft2232_bb_i2c_rawwrite;
	ftbb->master.rawread = ft2232_bb_i2c_rawread;

	return &ftbb->master;

error:
	return NULL;
}

static void ft2232_bb_remove(struct i2c_master *master)
{
	struct ft2232_bb *ftbb = master_to_ft2232_bb(master);
	struct ftdi_context *ftdi = ftbb->ftdi;

	ftdi_set_interface(ftdi, INTERFACE_ANY);

	ftdi_usb_close(ftdi);

	ftdi_free(ftdi);
}

static struct i2c_driver ft2232_bb_driver = {
	.name = "ft2232-bb",
	.probe = ft2232_bb_probe,
	.remove = ft2232_bb_remove,
};
register_i2c_driver(ft2232_bb_driver);
