#include <stdio.h>

#include "common.h"
#include "i2c_master.h"

void i2c_scan(struct i2c_master *master, int min, int max)
{
	uint8_t i;

	printf("\n");
	printf("   ");
	for (i = 0; i < 0x10 ; i ++) {
		printf("  %1x", i);
	}
	printf("\n");

	i2c_stop(master);

	for (i = 0; ; i += 2) {
		if (i % 0x20 == 0x00) {
			printf("%02x: ", i >> 1);
		}

		if (i < (min << 1)) {
			printf("   ");
			continue;
		}

		i2c_start(master);
		if (i2c_rawwrite(master, i)) {
			printf("-- ");
		} else {
			printf("%02x ", i >> 1);
		}
		i2c_stop(master);

		if (i % 0x20 == 0x1e) {
			printf("\n");
		}

		if (i == (max << 1)) {
			printf("\n");
			break;
		}
	}
}
