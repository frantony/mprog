#ifndef _I2C_DEVPLUGIN_H_
#define _I2C_DEVPLUGIN_H_

#include <stdint.h>
#include "i2c_master.h"
#include "list.h"

struct i2c_devplugin {
	struct list_head node;

	const char *name;

	int (*probe)(struct i2c_master *master, int argc, char **argv);
};

extern void i2c_devplugin_register(struct i2c_devplugin *devplugin);

#define register_i2c_devplugin(drv)		\
	static void __attribute__((constructor)) drv##_register(void)	\
	{						\
		i2c_devplugin_register(&drv);	\
	}

int i2c_probe_devplugin(char *name, struct i2c_master *master,
			int argc, char **argv);

#endif /* _I2C_DEVPLUGIN_H_ */
