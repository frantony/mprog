#include <string.h>
#include <assert.h>
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
	char *shortname;
	char *args;

	args = strchr(name, ':');
	if (!args) {
		shortname = name;
	} else {
		shortname = strdup(name);
		args = strchr(shortname, ':');
		*args = 0;
		args++;
	}

	master = NULL;
	list_for_each_entry(driver, &registered_drivers, node) {
		if (!strcmp(driver->name, shortname)) {
			master = driver->probe(args);
			if (master) {
				master->driver = driver;
			}
			break;
		}
        }

	if (shortname != name)
		free(shortname);

	return master;
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

void i2c_driver_list(void)
{
	struct i2c_driver *driver;

	list_for_each_entry(driver, &registered_drivers, node) {
		assert(strchr(driver->name, ':') == NULL);
		printf("    %s\n", driver->name);
        }
}
