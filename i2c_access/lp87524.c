#include <stdio.h>

#include "i2c_devplugin.h"
#include "pmbus.h"

#undef LP87524_DEBUG

static int i2c_smbus_read_byte_data(struct i2c_master *master,
				uint16_t addr, uint8_t command, uint8_t *data)
{
	i2c_stop(master);

	i2c_start(master);
	i2c_begin_write(master, addr);
	i2c_rawwrite(master, command);
	i2c_start(master);
	i2c_begin_read(master, addr);
	i2c_read(master, data, 1);
	i2c_stop(master);

#ifdef LP87524_DEBUG
	printf("i2c_smbus_read_byte_data, addr=%04x, command=%02x, data=%02x\n",
		addr, command, *data);
#endif

	return 0;
}

static int i2c_smbus_write_byte_data(struct i2c_master *master,
				uint16_t addr, uint8_t command, uint8_t data)
{
	i2c_stop(master);

	i2c_start(master);
	i2c_begin_write(master, addr);
	i2c_rawwrite(master, command);
	i2c_rawwrite(master, data);
	i2c_stop(master);

#ifdef LP87524_DEBUG
	printf("i2c_smbus_write_byte_data, addr=%04x, command=%02x, data=%02x\n",
		addr, command, data);
#endif

	return 0;
}

static int lp87524_dump_volt_reg(struct i2c_master *master,
				uint16_t addr, char *name, uint8_t command)
{
	uint16_t data;
	float v;

	i2c_smbus_read_word_data(master, addr, command, &data);
	v = (float)(data) / 512.;
        printf("%s = %.7g V (raw value: 0x%04x)\n", name, v, data);

	return 0;
}

static int lp87524_dump_byte_reg(struct i2c_master *master,
				uint16_t addr, char *name, uint8_t command, uint8_t *data)
{
	i2c_smbus_read_byte_data(master, addr, command, data);
        printf("%s [0x%02x] = 0x%02x\n", name, command, *data);

	return 0;
}

static int lp87524_dump_regs(struct i2c_master *master, uint16_t addr)
{
	uint8_t data;

	printf("== lp87524, addr=0x%02x ==\n", addr);

	lp87524_dump_byte_reg(master, addr, "OTP_REV", 0x01, &data);
	lp87524_dump_byte_reg(master, addr, "BUCK0_CTRL1", 0x02, &data);
	lp87524_dump_byte_reg(master, addr, "BUCK1_CTRL1", 0x04, &data);
	lp87524_dump_byte_reg(master, addr, "BUCK2_CTRL1", 0x06, &data);
	lp87524_dump_byte_reg(master, addr, "BUCK3_CTRL1", 0x08, &data);

	lp87524_dump_byte_reg(master, addr, "BUCK0_VOUT", 0x0a, &data);
	lp87524_dump_byte_reg(master, addr, "BUCK0_FLOOR_VOUT", 0x0b, &data);

	lp87524_dump_byte_reg(master, addr, "BUCK1_VOUT", 0x0c, &data);
	lp87524_dump_byte_reg(master, addr, "BUCK1_FLOOR_VOUT", 0x0d, &data);

	lp87524_dump_byte_reg(master, addr, "BUCK2_VOUT", 0x0e, &data);
	lp87524_dump_byte_reg(master, addr, "BUCK2_FLOOR_VOUT", 0x0f, &data);

	lp87524_dump_byte_reg(master, addr, "BUCK3_VOUT", 0x10, &data);
	lp87524_dump_byte_reg(master, addr, "BUCK3_FLOOR_VOUT", 0x11, &data);

	lp87524_dump_byte_reg(master, addr, "BUCK0_DELAY", 0x12, &data);
	lp87524_dump_byte_reg(master, addr, "BUCK0_DELAY", 0x13, &data);
	lp87524_dump_byte_reg(master, addr, "BUCK0_DELAY", 0x14, &data);
	lp87524_dump_byte_reg(master, addr, "BUCK0_DELAY", 0x15, &data);

	lp87524_dump_byte_reg(master, addr, "GPIO2_DELAY", 0x16, &data);
	lp87524_dump_byte_reg(master, addr, "GPIO3_DELAY", 0x17, &data);

	lp87524_dump_byte_reg(master, addr, "RESET", 0x18, &data);
	lp87524_dump_byte_reg(master, addr, "CONFIG", 0x19, &data);

	lp87524_dump_byte_reg(master, addr, "INT_TOP1", 0x1a, &data);
	lp87524_dump_byte_reg(master, addr, "INT_TOP2", 0x1b, &data);
	lp87524_dump_byte_reg(master, addr, "INT_BUCK_0_1", 0x1c, &data);
	lp87524_dump_byte_reg(master, addr, "INT_BUCK_2_3", 0x1d, &data);
	lp87524_dump_byte_reg(master, addr, "TOP_STAT", 0x1e, &data);
	lp87524_dump_byte_reg(master, addr, "BUCK_0_1_STAT", 0x1f, &data);
	lp87524_dump_byte_reg(master, addr, "BUCK_2_3_STAT", 0x20, &data);
	lp87524_dump_byte_reg(master, addr, "TOP_MASK1", 0x21, &data);
	lp87524_dump_byte_reg(master, addr, "TOP_MASK2", 0x22, &data);
	lp87524_dump_byte_reg(master, addr, "BUCK_0_1_MASK", 0x23, &data);
	lp87524_dump_byte_reg(master, addr, "BUCK_2_3_MASK", 0x24, &data);

	lp87524_dump_byte_reg(master, addr, "SEL_I_LOAD", 0x25, &data);
	lp87524_dump_byte_reg(master, addr, "I_LOAD_2", 0x26, &data);
	lp87524_dump_byte_reg(master, addr, "I_LOAD_1", 0x27, &data);
	lp87524_dump_byte_reg(master, addr, "PGOOD_CTRL1", 0x28, &data);
	lp87524_dump_byte_reg(master, addr, "PGOOD_CTRL2", 0x29, &data);
	lp87524_dump_byte_reg(master, addr, "PGOOD_FLT", 0x2a, &data);
	lp87524_dump_byte_reg(master, addr, "PLL_CTRL", 0x2b, &data);
	lp87524_dump_byte_reg(master, addr, "PIN_FUNCTION", 0x2c, &data);
	lp87524_dump_byte_reg(master, addr, "GPIO_CONFIG", 0x2d, &data);
	lp87524_dump_byte_reg(master, addr, "GPIO_IN", 0x2e, &data);
	lp87524_dump_byte_reg(master, addr, "GPIO_OUT", 0x2f, &data);

	return 0;
}

static int i2c_lp87524_probe(struct i2c_master *master, int argc, char **argv)
{
	int addr;
	printf("<%s>\n", argv[0]);
	printf("<%s>\n", argv[1]);
	printf("<%s>\n", argv[2]);

	if (argc < 2) {
		printf("lp87524: usage: lp87524 <i2c addr>\n");
		return -1;
	}

	addr = atoi(argv[1]);
	printf("<0x%02x>\n", addr);
	if (addr < 0 || addr > 0x7f) {
		printf("lp87524: wrong <i2c addr>\n");
		return -1;
	}

	if (argc >= 3) {
		if (!strcmp("dump", argv[2])) {
			lp87524_dump_regs(master, addr);
		} else {
			printf("%s: unknown command\n", argv[2]);
		}
	} else {
		lp87524_dump_regs(master, addr);
	}

	return 0;
}

static struct i2c_devplugin i2c_lp87524_devplugin = {
	.name = "lp87524",
	.probe = i2c_lp87524_probe,
};
register_i2c_devplugin(i2c_lp87524_devplugin);
