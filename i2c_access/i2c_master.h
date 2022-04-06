#ifndef _I2C_MASTER_H_
#define _I2C_MASTER_H_

#include <stdint.h>
#include "common.h"
#include "list.h"

struct i2c_master {
	struct i2c_driver *driver;
	void (*start)(struct i2c_master *);
	void (*stop)(struct i2c_master *);
	int (*rawwrite)(struct i2c_master *, uint8_t data);
	int (*write)(struct i2c_master *, uint8_t *data, int len);
	void (*rawread)(struct i2c_master *, uint8_t *data, int need_ack);
	void (*read)(struct i2c_master *, uint8_t *data, int len);
};

struct i2c_driver {
	struct list_head node;

	const char *name;

	struct i2c_master *(*probe)(char *);
	void (*remove)(struct i2c_master *);
};

extern void i2c_driver_register(struct i2c_driver *driver);

#define register_i2c_driver(drv)		\
	static void __attribute__((constructor)) drv##_register(void)	\
	{						\
		i2c_driver_register(&drv);	\
	}

struct i2c_master *i2c_probe_master(char *name);
void i2c_remove_master(struct i2c_master *master);

static inline void i2c_start(struct i2c_master *master)
{
	if (!master->start) {
		printf("master->start is NULL\n");
		BUG();
	}

	(master->start)(master);
}

static inline void i2c_stop(struct i2c_master *master)
{
	if (!master->stop) {
		printf("master->stop is NULL\n");
		BUG();
	}

	(master->stop)(master);
}

static inline int i2c_rawwrite(struct i2c_master *master, uint8_t data)
{
	if (!master->rawwrite) {
		printf("master->rawwrite is NULL\n");
		BUG();
	}

	return (master->rawwrite)(master, data);
}

static inline int i2c_write(struct i2c_master *master, uint8_t *data, int len)
{
	int i;

	if (master->write) {
		return (master->write)(master, data, len);
	}

	if (!master->rawwrite) {
		printf("master->write & master->rawwrite are NULL\n");
		BUG();
	}

	i = 0;
	while (i < len) {
		if (i2c_rawwrite(master, data[i])) {
			break;
		}
		i++;
	}

	return i;
}

static inline void i2c_rawread(struct i2c_master *master, uint8_t *data, int need_ack)
{
	if (!master->rawread) {
		printf("master->rawread is NULL\n");
		BUG();
	}

	(master->rawread)(master, data, need_ack);
}

static inline void i2c_read(struct i2c_master *master, uint8_t *data, int len)
{
	int i;

	if (master->read) {
		(master->read)(master, data, len);
		return;
	}

	if (!master->rawread) {
		printf("master->read & master->rawread are NULL\n");
		BUG();
	}

	i = 0;
	while (len > 0) {
		int need_ack = 1;

		if (len == 1) {
			need_ack = 0;
		}

		master->rawread(master, &data[i], need_ack);

		i++;
		len--;
	}
}

static inline int i2c_begin_write(struct i2c_master *master, uint8_t addr)
{
	return i2c_rawwrite(master, (addr << 1) | 0);
}

static inline int i2c_begin_read(struct i2c_master *master, uint8_t addr)
{
	return i2c_rawwrite(master, (addr << 1) | 1);
}

static inline int i2c_check_addr(struct i2c_master *master, uint8_t addr)
{
	int ret;

	i2c_stop(master);

	i2c_start(master);
	ret = i2c_begin_write(master, addr);
	i2c_stop(master);

	return ret;
}

#endif /* _I2C_MASTER_H_ */
