#include <stdio.h>

#include "i2c_devplugin.h"

static int i2c_pca9545_probe(struct i2c_master *master, int argc, char **argv)
{
	int bus;
	int addr;

	if (argc < 2) {
		printf("pca9545: usage: pca9545 <bus number>\n");
		return -1;
	}

	bus = atoi(argv[1]);
	if (bus < 1 || bus > 4) {
		printf("pca9545: wrong <bus number>\n");
		return -1;
	}

	addr = 0x70;

	i2c_stop(master);

	i2c_start(master);
	i2c_begin_write(master, addr);
	i2c_rawwrite(master, (1 << (bus - 1)));
	i2c_stop(master);

	return 0;
}

static struct i2c_devplugin i2c_pca9545_devplugin = {
	.name = "pca9545",
	.probe = i2c_pca9545_probe,
};
register_i2c_devplugin(i2c_pca9545_devplugin);
