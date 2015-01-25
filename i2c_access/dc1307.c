#include <stdio.h>
#include <string.h>

#include "common.h"
#include "i2c_master.h"

void dc1307_reset(struct i2c_master *master, int addr)
{
	i2c_stop(master);

	i2c_start(master);
	i2c_begin_write(master, addr);
	i2c_rawwrite(master, 0x00);
	i2c_rawwrite(master, 0x00);
	i2c_rawwrite(master, 0x59);
	i2c_rawwrite(master, 0x02);
	i2c_stop(master);
}

void dc1307_read_time(struct i2c_master *master, int addr)
{
	//uint8_t buf[3];
	//uint8_t buf[64];
	uint8_t buf[8192];
	uint8_t buf2[58] = {
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
		0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
		0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
		0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
		0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
	};

//	memset(buf, 0, 64);

	i2c_stop(master);
#if 0
	i2c_start(master);
	/* Slave address */
	i2c_begin_write(master, addr);
	i2c_write(master, 0x08);
	{
	int i = i2c_writeblock(master, &buf2[0], 56);
	printf("i=%d\n", i);
	}
	i2c_stop(master);
#endif

#if 1
	i2c_start(master);
	i2c_begin_write(master, addr);
	i2c_rawwrite(master, 0x00);
	i2c_stop(master);

	i2c_start(master);
	i2c_begin_read(master, addr);
	i2c_read(master, &buf[0], 64);
	i2c_stop(master);

	printf("dc1307: %02x:%02x:%02x\n", buf[2], buf[1], buf[0]);

	memory_display(&buf[0], 0, 8, 1, 0);
	memory_display(&buf[8], 8, 56, 1, 0);
#endif
}
