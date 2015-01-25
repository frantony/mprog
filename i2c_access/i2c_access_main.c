#include <stdio.h>
#include <string.h>

#include "common.h"
#include "i2c_master.h"

void i2c_scan(struct i2c_master *master, int min, int max);

void dc1307_reset(struct i2c_master *master, int addr);
void dc1307_read_time(struct i2c_master *master, int addr);

int main(int argc, char **argv)
{
	struct i2c_master *m;

	if (argc < 3) {
		printf("%s <driver> <command>\n", argv[0]);
		exit(1);
	}

	m = i2c_probe_master(argv[1]);
	if (!m) {
		printf("can't probe master %s\n", argv[1]);
		exit(1);
	}

	if (!strcmp(argv[2], "scan")) {
		i2c_scan(m, 0x03, 0x77);
	} else if (!strcmp(argv[2], "dc1307")) {
		dc1307_read_time(m, 0x68);
	} else if (!strcmp(argv[2], "dc1307-reset")) {
		dc1307_reset(m, 0x68);
	}

	i2c_remove_master(m);

	return 0;
}
