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
