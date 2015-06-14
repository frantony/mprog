#include <stdio.h>

#include "i2c_devplugin.h"

static void i2c_scan(struct i2c_master *master, int min, int max)
{
	uint8_t i;

	printf("\n");
	printf("   ");
	for (i = 0; i < 0x10 ; i ++) {
		printf("  %1x", i);
	}
	printf("\n");

	for (i = 0; ; i ++) {
		if (i % 0x10 == 0x00) {
			printf("%02x: ", i);
		}

		if (i < (min << 1)) {
			printf("   ");
			continue;
		}

		if (i2c_check_addr(master, i)) {
			printf("-- ");
		} else {
			printf("%02x ", i);
		}

		if (i == max) {
			printf("\n");
			break;
		}

		if (i % 0x10 == 0xf) {
			printf("\n");
		}
	}
}

static int i2c_scan_probe(struct i2c_master *master, int argc, char **argv)
{
	(void)argc;
	(void)argv;

	i2c_scan(master, 0x03, 0x77);

	return 0;
}

static struct i2c_devplugin i2c_scan_devplugin = {
	.name = "scan",
	.probe = i2c_scan_probe,
};
register_i2c_devplugin(i2c_scan_devplugin);
