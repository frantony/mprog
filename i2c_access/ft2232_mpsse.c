#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "common.h"
#include "i2c_master.h"

#include <mpsse.h>

struct ft2232_mpsse {
	struct i2c_master master;

	struct mpsse_context *eeprom;
};

static inline struct ft2232_mpsse *master_to_ft2232_mpsse(struct i2c_master *c)
{
	return container_of(c, struct ft2232_mpsse, master);
}

static void ft2232_mpsse_i2c_start(struct i2c_master *master)
{
	struct ft2232_mpsse *ftmpsse = master_to_ft2232_mpsse(master);

	Start(ftmpsse->eeprom);
}

static void ft2232_mpsse_i2c_stop(struct i2c_master *master)
{
	struct ft2232_mpsse *ftmpsse = master_to_ft2232_mpsse(master);

	Stop(ftmpsse->eeprom);
}

static int ft2232_mpsse_i2c_rawwrite(struct i2c_master *master, uint8_t data)
{
	struct ft2232_mpsse *ftmpsse = master_to_ft2232_mpsse(master);

	Write(ftmpsse->eeprom, (char *)&data, 1);

	if (GetAck(ftmpsse->eeprom) == ACK) {
		return 0;
	}

	return 1;
}

static int ft2232_mpsse_i2c_write(struct i2c_master *master,
				uint8_t *data, int len)
{
	struct ft2232_mpsse *ftmpsse = master_to_ft2232_mpsse(master);
	int retval;

	retval = Write(ftmpsse->eeprom, (char *)data, len);

	if (retval == MPSSE_OK) {
		return len;
	}

	return 0;
}

#if 0
static void ft2232_mpsse_i2c_rawread(struct i2c_master *master,
				uint8_t *data, int need_ack)
{
	struct ft2232_mpsse *ftmpsse = master_to_ft2232_mpsse(master);
	char *t;

	t = Read(ftmpsse->eeprom, 1);

	if (t) {
		*data = (uint8_t)(*t);
		free(t);
	}

	if (need_ack) {
		Stop(ftmpsse->eeprom);
	}
}
#endif

static void ft2232_mpsse_i2c_read(struct i2c_master *master,
				uint8_t *data, int len)
{
	struct ft2232_mpsse *ftmpsse = master_to_ft2232_mpsse(master);
	char *t;

	t = Read(ftmpsse->eeprom, len);

	if (t) {
		memcpy(data, t, len);
		free(t);
	}
}

static struct i2c_master *ft2232_mpsse_probe(void)
{
	struct mpsse_context *eeprom;
	struct ft2232_mpsse *ftmpsse;

	eeprom = MPSSE(I2C, ONE_HUNDRED_KHZ, MSB);
	if (!eeprom || !eeprom->open) {
		printf("Failed to initialize MPSSE: %s\n", ErrorString(eeprom));
		goto error;
	}

	//printf("%s initialized at %dHz (I2C)\n", GetDescription(eeprom), GetClock(eeprom));

	ftmpsse = xzalloc(sizeof(struct ft2232_mpsse));

	ftmpsse->eeprom = eeprom;

	ftmpsse->master.start = ft2232_mpsse_i2c_start;
	ftmpsse->master.stop = ft2232_mpsse_i2c_stop;
	ftmpsse->master.rawwrite = ft2232_mpsse_i2c_rawwrite;
	ftmpsse->master.write = ft2232_mpsse_i2c_write;
	/* no rawread for libmpsse */
	ftmpsse->master.read = ft2232_mpsse_i2c_read;

	return &ftmpsse->master;

error:
	return NULL;
}

static void ft2232_mpsse_remove(struct i2c_master *master)
{
	struct ft2232_mpsse *ftmpsse = master_to_ft2232_mpsse(master);

	Close(ftmpsse->eeprom);
}

static struct i2c_driver ft2232_mpsse_driver = {
	.name = "ft2232-mpsse",
	.probe = ft2232_mpsse_probe,
	.remove = ft2232_mpsse_remove,
};
register_i2c_driver(ft2232_mpsse_driver);
