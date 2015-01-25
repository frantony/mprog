#include <string.h>
#include "common.h"
#include "i2c_master.h"

#define DRIVERS_MAX 4
struct i2c_driver *registered_drivers[DRIVERS_MAX];
int registered_driver_count = 0;

void i2c_driver_register(struct i2c_driver *driver)
{
//	printf("driver_register:%s\n", driver->name);

	if (registered_driver_count >= DRIVERS_MAX) {
		printf("Tried to register more than %i driver "
			 "interfaces.\n", DRIVERS_MAX);
		BUG();
	}

	registered_drivers[registered_driver_count] = driver;
	registered_driver_count++;
}

struct i2c_master *i2c_probe_master(char *name)
{
	struct i2c_master *master;
	int j;

	for (j = 0; j < registered_driver_count; j++) {
		struct i2c_driver *driver = registered_drivers[j];
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
