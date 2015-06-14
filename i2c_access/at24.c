#include <stdio.h>

#include "i2c_devplugin.h"

static int i2c_at24_probe(struct i2c_master *master, int argc, char **argv)
{
	int addr;
	int offset_len;
	uint8_t buf[256];

	(void)argc;
	(void)argv;

	addr = 0x50;
	offset_len = 8;

	if (!strcmp("spd", argv[0])) {
		addr = 0x53;
	} else if (!strcmp("at24c1024", argv[0])) {
		offset_len = 16;
	}

	if (i2c_check_addr(master, addr)) {
		printf("no at24 chip at addr=0x%02x\n", addr);
		return -1;
	}

	i2c_start(master);
	i2c_begin_write(master, addr);
	i2c_rawwrite(master, 0x00);
	if (offset_len == 16) {
		i2c_rawwrite(master, 0x00);
	}
	i2c_stop(master);

	i2c_start(master);
	i2c_begin_read(master, addr);
	i2c_read(master, &buf[0], 256);
	i2c_stop(master);

	memory_display(&buf[0], 0, 256, 1, 0);

	return 0;
}

static struct i2c_devplugin i2c_at24c02_devplugin = {
	.name = "at24c02",
	.probe = i2c_at24_probe,
};
register_i2c_devplugin(i2c_at24c02_devplugin);

static struct i2c_devplugin i2c_spd_devplugin = {
	.name = "spd",
	.probe = i2c_at24_probe,
};
register_i2c_devplugin(i2c_spd_devplugin);

static struct i2c_devplugin i2c_at24c1024_devplugin = {
	.name = "at24c1024",
	.probe = i2c_at24_probe,
};
register_i2c_devplugin(i2c_at24c1024_devplugin);
