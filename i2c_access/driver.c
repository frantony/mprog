#include <string.h>
#include "i2c_master.h"

LIST_HEAD(registered_drivers);

void i2c_driver_register(struct i2c_driver *driver)
{
//	printf("driver_register:%s\n", driver->name);

	list_add_tail(&driver->node, &registered_drivers);
}

struct i2c_master *i2c_probe_master(char *name)
{
	struct i2c_master *master;
	struct i2c_driver *driver;

	list_for_each_entry(driver, &registered_drivers, node) {
		if (!strcmp(driver->name, name)) {
			master = driver->probe();
			if (master) {
				master->driver = driver;
			}

			return master;
		}
        }

	return NULL;
}

void i2c_remove_master(struct i2c_master *master)
{
	struct i2c_driver *driver;

	if (!master) {
		BUG();
		return;
	}

	driver = master->driver;
	if (!driver) {
		BUG();
		return;
	}

	if (!driver->remove) {
		BUG();
		return;
	}

	driver->remove(master);
}
