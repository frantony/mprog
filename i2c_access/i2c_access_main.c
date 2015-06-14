#include <stdio.h>
#include <string.h>

#include "common.h"
#include "i2c_master.h"
#include "i2c_devplugin.h"

extern void i2c_devplugin_list(void);
extern void i2c_driver_list(void);

int main(int argc, char **argv)
{
	struct i2c_master *m;

	if (argc < 3) {
		printf("Usage:\n  %s <driver> <plugin>\n", argv[0]);
		printf("\nDrivers:\n");
		i2c_driver_list();
		printf("\nPlugins:\n");
		i2c_devplugin_list();
		exit(1);
	}

	m = i2c_probe_master(argv[1]);
	if (!m) {
		printf("can't probe master %s\n", argv[1]);
		exit(1);
	}

	i2c_probe_devplugin(argv[2], m, argc - 2, &argv[2]);

	i2c_remove_master(m);

	return 0;
}
