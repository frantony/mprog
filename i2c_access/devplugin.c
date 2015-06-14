#include <string.h>
#include "i2c_devplugin.h"

LIST_HEAD(registered_devplugins);

void i2c_devplugin_register(struct i2c_devplugin *devplugin)
{
//	printf("devplugin_register:%s\n", driver->name);

	list_add_tail(&devplugin->node, &registered_devplugins);
}

int i2c_probe_devplugin(char *name, struct i2c_master *master,
			int argc, char **argv)
{
	struct i2c_devplugin *devplugin;

	list_for_each_entry(devplugin, &registered_devplugins, node) {
		if (!strcmp(devplugin->name, name)) {
			return devplugin->probe(master, argc, argv);
		}
        }

	return -1;
}

void i2c_devplugin_list(void)
{
	struct i2c_devplugin *devplugin;

	list_for_each_entry(devplugin, &registered_devplugins, node) {
		printf("    %s\n", devplugin->name);
        }
}
