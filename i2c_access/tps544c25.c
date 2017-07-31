#include <stdio.h>

#include "i2c_devplugin.h"
#include "pmbus.h"

#undef TPS544C25_DEBUG

int i2c_smbus_read_byte_data(struct i2c_master *master,
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

#ifdef TPS544C25_DEBUG
	printf("i2c_smbus_read_byte_data, addr=%04x, command=%02x, data=%02x\n",
		addr, command, *data);
#endif

	return 0;
}

int i2c_smbus_read_word_data(struct i2c_master *master,
				uint16_t addr, uint8_t command, uint16_t *data)
{
	uint8_t buf[2];

	i2c_stop(master);

	i2c_start(master);
	i2c_begin_write(master, addr);
	i2c_rawwrite(master, command);
	i2c_start(master);
	i2c_begin_read(master, addr);
	i2c_read(master, &buf[0], 2);
	i2c_stop(master);

	*data = buf[0] | ((uint16_t)buf[1] << 8);

	return 0;
}

int i2c_smbus_write_byte_data(struct i2c_master *master,
				uint16_t addr, uint8_t command, uint8_t data)
{
	i2c_stop(master);

	i2c_start(master);
	i2c_begin_write(master, addr);
	i2c_rawwrite(master, command);
	i2c_rawwrite(master, data);
	i2c_stop(master);

#ifdef TPS544C25_DEBUG
	printf("i2c_smbus_write_byte_data, addr=%04x, command=%02x, data=%02x\n",
		addr, command, data);
#endif

	return 0;
}

int i2c_smbus_write_word_data(struct i2c_master *master,
				uint16_t addr, uint8_t command, uint16_t data)
{
	i2c_stop(master);

	i2c_start(master);
	i2c_begin_write(master, addr);
	i2c_rawwrite(master, command);
	i2c_rawwrite(master, (uint8_t) ((data >> 0) & 0xff));
	i2c_rawwrite(master, (uint8_t) ((data >> 8) & 0xff));
	i2c_stop(master);

#ifdef TPS544C25_DEBUG
	printf("i2c_smbus_write_word_data, addr=%04x, command=%02x, data=%04x\n",
		addr, command, data);
#endif

	return 0;
}


static int tps544c25_dump_volt_reg(struct i2c_master *master,
				uint16_t addr, char *name, uint8_t command)
{
	uint16_t data;
	float v;

	i2c_smbus_read_word_data(master, addr, command, &data);
	v = (float)(data) / 512.;
        printf("%s = %.7g V (raw value: 0x%04x)\n", name, v, data);

	return 0;
}

static int tps544c25_dump_byte_reg(struct i2c_master *master,
				uint16_t addr, char *name, uint8_t command)
{
	uint8_t data;

	i2c_smbus_read_byte_data(master, addr, command, &data);
        printf("%s = 0x%02x\n", name, data);

	return 0;
}

#define ON_OFF_CONFIG_NAME		"ON_OFF_CONFIG"
#define WRITE_PROTECT_NAME		"WRITE_PROTECT"
#define VOUT_COMMAND_NAME		"VOUT_COMMAND"
#define VOUT_SCALE_LOOP_NAME		"VOUT_SCALE_LOOP"
#define VOUT_MAX_NAME			"VOUT_MAX"
#define VIN_ON_NAME			"VIN_ON"
#define VIN_OFF_NAME			"VIN_OFF"
#define IOUT_CAL_OFFSET_NAME		"IOUT_CAL_OFFSET"
#define VOUT_OV_FAULT_LIMIT_NAME	"VOUT_OV_FAULT_LIMIT"
#define VOUT_OV_FAULT_RESPONSE_NAME	"VOUT_OV_FAULT_RESPONSE"
#define VOUT_OV_WARN_LIMIT_NAME		"VOUT_OV_WARN_LIMIT"
#define VOUT_UV_WARN_LIMIT_NAME		"VOUT_UV_WARN_LIMIT"
#define VOUT_UV_FAULT_LIMIT_NAME	"VOUT_UV_FAULT_LIMIT"
#define VOUT_UV_FAULT_RESPONSE_NAME	"VOUT_UV_FAULT_RESPONSE"
#define IOUT_OC_FAULT_LIMIT_NAME	"IOUT_OC_FAULT_LIMIT"
#define IOUT_OC_FAULT_RESPONSE_NAME	"IOUT_OC_FAULT_RESPONSE"
#define OT_FAULT_LIMIT_NAME		"OT_FAULT_LIMIT"
#define OT_FAULT_RESPONSE_NAME		"OT_FAULT_RESPONSE"
#define TON_DELAY_NAME			"TON_DELAY"
#define TON_RISE_NAME			"TON_RISE"
#define TON_MAX_FAULT_RESPONSE_NAME	"TON_MAX_FAULT_RESPONSE"
#define TOFF_DELAY_NAME			"TOFF_DELAY"
#define TOFF_FALL_NAME			"TOFF_FALL"
#define MFR_VOUT_MIN_NAME		"MFR_VOUT_MIN"
#define MFR_SPECIFIC_00_NAME		"MFR_SPECIFIC_00"
#define MFR_SPECIFIC_21_NAME		"MFR_SPECIFIC_21"
#define MFR_SPECIFIC_32_NAME		"MFR_SPECIFIC_32"

struct nvm_cmds {
	const int cmd_code;
	const char *name;
	const int size;
	int value;
} nvm_cmds[] = {
	{ 0x02, ON_OFF_CONFIG_NAME, 1, 0 },
// ??? 1Bh SMBALRT_MASK
	{ 0x21, VOUT_COMMAND_NAME, 2, 0 },
	{ 0x24, VOUT_MAX_NAME, 2, 0 },
	{ 0x29, VOUT_SCALE_LOOP_NAME, 2, 0 },
	{ 0x35, VIN_ON_NAME, 2, 0 },
	{ 0x36, VIN_OFF_NAME, 2, 0 },
	{ 0x39, IOUT_CAL_OFFSET_NAME, 2, 0 },
	{ 0x40, VOUT_OV_FAULT_LIMIT_NAME, 2, 0 },
	{ 0x41, VOUT_OV_FAULT_RESPONSE_NAME, 1, 0 },
	{ 0x42, VOUT_OV_WARN_LIMIT_NAME, 2, 0 }, // Volatile
	{ 0x43, VOUT_UV_WARN_LIMIT_NAME, 2, 0 }, // Volatile
	{ 0x44, VOUT_UV_FAULT_LIMIT_NAME, 2, 0 },
	{ 0x45, VOUT_UV_FAULT_RESPONSE_NAME, 1, 0 },
	{ 0x46, IOUT_OC_FAULT_LIMIT_NAME, 2, 0 },
	{ 0x47, IOUT_OC_FAULT_RESPONSE_NAME, 1, 0 },
	{ 0x4F, OT_FAULT_LIMIT_NAME, 2, 0 },
	{ 0x50, OT_FAULT_RESPONSE_NAME, 1, 0 },
	{ 0x60, TON_DELAY_NAME, 2, 0 },
	{ 0x61, TON_RISE_NAME, 2, 0 },
	{ 0x63, TON_MAX_FAULT_RESPONSE_NAME, 1, 0 },
	{ 0x64, TOFF_DELAY_NAME, 2, 0 },
	{ 0x65, TOFF_FALL_NAME, 2, 0 },
	{ 0xa4, MFR_VOUT_MIN_NAME, 2, 0 },
	{ 0xd0, MFR_SPECIFIC_00_NAME, 2, 0 },
	{ 0xe5, MFR_SPECIFIC_21_NAME, 2, 0 },
	{ 0xf0, MFR_SPECIFIC_32_NAME, 2, 0 },
	{ 0x10, WRITE_PROTECT_NAME, 1, 0 }, // WRITE_PROTECT is last!
	{ }
};

static int tps544c25_dump_regs(struct i2c_master *master, uint16_t addr)
{
	printf("== tps544c25, addr=0x%02x ==\n", addr);

	tps544c25_dump_byte_reg(master, addr, "OPERATION", 0x01);
	tps544c25_dump_byte_reg(master, addr, "ON_OFF_CONFIG", 0x02);
	tps544c25_dump_volt_reg(master, addr, "VOUT_UV_FAULT_LIMIT", 0x44);
	tps544c25_dump_volt_reg(master, addr, "VOUT_UV_WARN_LIMIT", 0x43);
	tps544c25_dump_volt_reg(master, addr, "MFR_VOUT_MIN", 0xa4);
	tps544c25_dump_volt_reg(master, addr, "VOUT_COMMAND", PMBUS_VOUT_COMMAND);
	tps544c25_dump_volt_reg(master, addr, "VOUT_MAX", PMBUS_VOUT_MAX);
	tps544c25_dump_volt_reg(master, addr, "VOUT_OV_WARN_LIMIT", 0x42);
	tps544c25_dump_volt_reg(master, addr, "VOUT_OV_FAULT_LIMIT", 0x40);

	return 0;
}

static int tps544c25_dump_all_regs(struct i2c_master *master, uint16_t addr)
{
	struct nvm_cmds *i;

	i = &nvm_cmds[0];
	while (i->name) {
		if (i->size == 2) {
			uint16_t data;

			i2c_smbus_read_word_data(master, addr, i->cmd_code, &data);
			printf("# CMD CODE 0x%02x\n", i->cmd_code);
			printf("%s = 0x%04x\n", i->name, data);
		} else if (i->size == 1) {
			uint8_t data;

			i2c_smbus_read_byte_data(master, addr, i->cmd_code, &data);
			printf("# CMD CODE 0x%02x\n", i->cmd_code);
			printf("%s = 0x%02x\n", i->name, data);
		} else {
			printf("warning: i->size=%d\n", i->size);
		}
		i++;
	}

	return 0;
}

#include <confuse.h>

void check_cfg(cfg_t *cfg, const char *name, int *ret)
{
	long int t;

	t = cfg_getint(cfg, name);
	if (t == -1) {
		printf("error: please set %s\n", name);
		*ret = -1;
	}
}

#define OPERATION 0x01
#define ON_OFF_CONFIG 0x02
#define STORE_DEFAULT_ALL    0x11
#define VOUT_SCALE_LOOP      0x29
#define VOUT_OV_FAULT_LIMIT  0x40
#define VOUT_OV_WARN_LIMIT   0x42
#define VOUT_MAX             0x24
#define VOUT_COMMAND         0x21
#define MFR_VOUT_MIN         0xa4
#define VOUT_UV_WARN_LIMIT   0x43
#define VOUT_UV_FAULT_LIMIT  0x44

static int tps544c25_write_regs(struct i2c_master *master, uint16_t addr)
{
	cfg_opt_t opts[] = {
		CFG_INT(ON_OFF_CONFIG_NAME, -1, CFGF_NONE),
		CFG_INT(WRITE_PROTECT_NAME, -1, CFGF_NONE),
		CFG_INT(VOUT_COMMAND_NAME, -1, CFGF_NONE),
		CFG_INT(VOUT_MAX_NAME, -1, CFGF_NONE),
		CFG_INT(VOUT_OV_WARN_LIMIT_NAME, -1, CFGF_NONE),
		CFG_INT(VOUT_UV_WARN_LIMIT_NAME, -1, CFGF_NONE),
		CFG_INT(VOUT_SCALE_LOOP_NAME, -1, CFGF_NONE),
		CFG_INT(VIN_ON_NAME, -1, CFGF_NONE),
		CFG_INT(VIN_OFF_NAME, -1, CFGF_NONE),
		CFG_INT(IOUT_CAL_OFFSET_NAME, -1, CFGF_NONE),
		CFG_INT(VOUT_OV_FAULT_LIMIT_NAME, -1, CFGF_NONE),
		CFG_INT(VOUT_OV_FAULT_RESPONSE_NAME, -1, CFGF_NONE),
		CFG_INT(VOUT_UV_FAULT_LIMIT_NAME, -1, CFGF_NONE),
		CFG_INT(VOUT_UV_FAULT_RESPONSE_NAME, -1, CFGF_NONE),
		CFG_INT(IOUT_OC_FAULT_LIMIT_NAME, -1, CFGF_NONE),
		CFG_INT(IOUT_OC_FAULT_RESPONSE_NAME, -1, CFGF_NONE),
		CFG_INT(OT_FAULT_LIMIT_NAME, -1, CFGF_NONE),
		CFG_INT(OT_FAULT_RESPONSE_NAME, -1, CFGF_NONE),
		CFG_INT(TON_DELAY_NAME, -1, CFGF_NONE),
		CFG_INT(TON_RISE_NAME, -1, CFGF_NONE),
		CFG_INT(TON_MAX_FAULT_RESPONSE_NAME, -1, CFGF_NONE),
		CFG_INT(TOFF_DELAY_NAME, -1, CFGF_NONE),
		CFG_INT(TOFF_FALL_NAME, -1, CFGF_NONE),
		CFG_INT(MFR_VOUT_MIN_NAME, -1, CFGF_NONE),
		CFG_INT(MFR_SPECIFIC_00_NAME, -1, CFGF_NONE),
		CFG_INT(MFR_SPECIFIC_21_NAME, -1, CFGF_NONE),
		CFG_INT(MFR_SPECIFIC_32_NAME, -1, CFGF_NONE),
		CFG_END()
	};
	cfg_t *cfg;
	int ret;

	ret = 0;
	cfg = cfg_init(opts, CFGF_NONE);
	if (cfg_parse(cfg, "hello.conf") == CFG_PARSE_ERROR)
		return 1;

	// (40h) VOUT_OV_FAULT_LIMIT
	check_cfg(cfg, VOUT_OV_FAULT_LIMIT_NAME, &ret);

	// (42h) VOUT_OV_WARN_LIMIT
	check_cfg(cfg, VOUT_OV_WARN_LIMIT_NAME, &ret);

	// (24h) VOUT_MAX
	check_cfg(cfg, VOUT_MAX_NAME, &ret);

	// (21h) VOUT_COMMAND
	check_cfg(cfg, VOUT_COMMAND_NAME, &ret);

	// (A4h) MFR_VOUT_MIN
	check_cfg(cfg, MFR_VOUT_MIN_NAME, &ret);

	// (43h) VOUT_UV_WARN_LIMIT
	check_cfg(cfg, VOUT_UV_WARN_LIMIT_NAME, &ret);

	// (44h) VOUT_UV_FAULT_LIMIT
	check_cfg(cfg, VOUT_UV_FAULT_LIMIT_NAME, &ret);

	if (ret == -1) {
		cfg_free(cfg);
		return -1;
	}

{
	uint16_t new_vout, old_vout;
	uint16_t t;
	struct nvm_cmds *i;

	new_vout = cfg_getint(cfg, VOUT_COMMAND_NAME);
	i2c_smbus_read_word_data(master, addr, PMBUS_VOUT_COMMAND, &old_vout);

#if 0
	{

		uint8_t t;
		uint8_t on_off_config;

		printf("turning off\n");

		// ON_OFF_CONFIG (02h) <- set (1 << 3)
		// OPERATION (01h) <- (1 << 6)
		i2c_smbus_read_byte_data(master, addr, ON_OFF_CONFIG, &on_off_config);
		t = on_off_config;

		// set cmd bit: Device responds to the “on” bit in the OPERATION command.
		t |= (1 << 3);
		// set pu bit: Device does not power up until commanded by the CNTL pin and/or OPERATION command as programmed in bits [3:0] of the ON_OFF_CONFIG register.
		t |= (1 << 4);
		// clear cpr bit: Device ignores the CNTL pin.
		t &= ~(1 << 2);
		i2c_smbus_write_byte_data(master, addr, ON_OFF_CONFIG, t);
		i2c_smbus_write_byte_data(master, addr, OPERATION, 0);

		// restore ON_OFF_CONFIG
		//i2c_smbus_write_byte_data(master, addr, ON_OFF_CONFIG, on_off_config | (1 << 3));
	}
#endif

	printf("new_vout=%04x, old_vout=%04x\n", new_vout, old_vout);

	t = cfg_getint(cfg, VOUT_SCALE_LOOP_NAME);
	i2c_smbus_write_word_data(master, addr, VOUT_SCALE_LOOP, t);

	// VOUT_SCALE_LOOP should be changed first, if needed.
	if (new_vout > old_vout) {

		t = cfg_getint(cfg, VOUT_OV_FAULT_LIMIT_NAME);
		i2c_smbus_write_word_data(master, addr, VOUT_OV_FAULT_LIMIT, t);

		t = cfg_getint(cfg, VOUT_OV_WARN_LIMIT_NAME);
		i2c_smbus_write_word_data(master, addr, VOUT_OV_WARN_LIMIT, t);

		t = cfg_getint(cfg, VOUT_MAX_NAME);
		i2c_smbus_write_word_data(master, addr, VOUT_MAX, t);

		t = cfg_getint(cfg, VOUT_COMMAND_NAME);
		i2c_smbus_write_word_data(master, addr, VOUT_COMMAND, t);

		t = cfg_getint(cfg, MFR_VOUT_MIN_NAME);
		i2c_smbus_write_word_data(master, addr, MFR_VOUT_MIN, t);

		t = cfg_getint(cfg, VOUT_UV_WARN_LIMIT_NAME);
		i2c_smbus_write_word_data(master, addr, VOUT_UV_WARN_LIMIT, t);

		t = cfg_getint(cfg, VOUT_UV_FAULT_LIMIT_NAME);
		i2c_smbus_write_word_data(master, addr, VOUT_UV_FAULT_LIMIT, t);
	} else {
		t = cfg_getint(cfg, VOUT_UV_FAULT_LIMIT_NAME);
		i2c_smbus_write_word_data(master, addr, VOUT_UV_FAULT_LIMIT, t);

		t = cfg_getint(cfg, VOUT_UV_WARN_LIMIT_NAME);
		i2c_smbus_write_word_data(master, addr, VOUT_UV_WARN_LIMIT, t);

		t = cfg_getint(cfg, MFR_VOUT_MIN_NAME);
		i2c_smbus_write_word_data(master, addr, MFR_VOUT_MIN, t);

		t = cfg_getint(cfg, VOUT_COMMAND_NAME);
		i2c_smbus_write_word_data(master, addr, VOUT_COMMAND, t);

		t = cfg_getint(cfg, VOUT_MAX_NAME);
		i2c_smbus_write_word_data(master, addr, VOUT_MAX, t);

		t = cfg_getint(cfg, VOUT_OV_WARN_LIMIT_NAME);
		i2c_smbus_write_word_data(master, addr, VOUT_OV_WARN_LIMIT, t);

		t = cfg_getint(cfg, VOUT_OV_FAULT_LIMIT_NAME);
		i2c_smbus_write_word_data(master, addr, VOUT_OV_FAULT_LIMIT, t);
	}

	i = &nvm_cmds[0];
	while (i->name) {
		if (i->cmd_code == VOUT_SCALE_LOOP ||
			i->cmd_code == VOUT_OV_FAULT_LIMIT ||
			i->cmd_code == VOUT_OV_WARN_LIMIT ||
			i->cmd_code == VOUT_MAX ||
			i->cmd_code == VOUT_COMMAND ||
			i->cmd_code == MFR_VOUT_MIN ||
			i->cmd_code == VOUT_UV_WARN_LIMIT ||
			i->cmd_code == VOUT_UV_FAULT_LIMIT) {

			i++;
			continue;
		}

		if (i->size == 2) {
			uint16_t data;

			data = cfg_getint(cfg, i->name);
			i2c_smbus_write_word_data(master, addr, i->cmd_code, data);
		} else if (i->size == 1) {
			uint8_t data;

			data = cfg_getint(cfg, i->name);
			i2c_smbus_write_byte_data(master, addr, i->cmd_code, data);
		} else {
			printf("warning: i->size=%d\n", i->size);
		}

		i++;
	}
}

	cfg_free(cfg);
	return 0;
}

static int tps544c25_store_default_all(struct i2c_master *master, uint16_t addr)
{
	i2c_stop(master);

	i2c_start(master);
	i2c_begin_write(master, addr);
	i2c_rawwrite(master, STORE_DEFAULT_ALL);
	i2c_stop(master);

	return 0;
}

static int i2c_tps544c25_probe(struct i2c_master *master, int argc, char **argv)
{
	int addr;

	if (argc < 2) {
		printf("tps544c25: usage: tps544c25 <i2c addr>\n");
		return -1;
	}

	addr = atoi(argv[1]);
	if (addr < 0 || addr > 0x7f) {
		printf("tps544c25: wrong <i2c addr>\n");
		return -1;
	}

	if (argc >= 3) {
		if (!strcmp("dump", argv[2])) {
			tps544c25_dump_regs(master, addr);
		} else if (!strcmp("dumpall", argv[2])) {
			tps544c25_dump_all_regs(master, addr);
		} else if (!strcmp("writeregs", argv[2])) {
			tps544c25_write_regs(master, addr);
		} else if (!strcmp("store_default_all", argv[2])) {
			printf("%s: unknown command\n", argv[2]);
			tps544c25_store_default_all(master, addr);
			printf("%s: unknown command\n", argv[2]);
		} else {
			printf("%s: unknown command\n", argv[2]);
		}
	} else {
		tps544c25_dump_regs(master, addr);
	}

	return 0;
}

static struct i2c_devplugin i2c_tps544c25_devplugin = {
	.name = "tps544c25",
	.probe = i2c_tps544c25_probe,
};
register_i2c_devplugin(i2c_tps544c25_devplugin);
