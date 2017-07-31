CC = gcc

ifeq ($(CONFIG_LIBMPSSE), yes)
# svn checkout  http://libmpsse.googlecode.com/svn/trunk libmpsse-read-only
MPSSE_CFLAGS = -Ilibmpsse-read-only/src
MPSSE_LDFLAGS = -Llibmpsse-read-only/src -lmpsse
MPSSE_DRIVER=ft2232_mpsse.c
endif

CFLAGS = -W -Wall $(MPSSE_CFLAGS)
CPPFLAGS = -MD -MP
LDFLAGS = -lftdi $(MPSSE_LDFLAGS) -lconfuse

SRC = i2c_access_main.c \
	driver.c \
	devplugin.c \
	ft2232_bb.c \
	$(MPSSE_DRIVER) \
	memory_display.c \
	dc1307.c \
	pca9545.c \
	at24.c \
	tps544c25.c \
	i2c_scan.c

i2c_access: $(SRC:%.c=%.o)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $*.c -o $*.o

.PHONY: clean
clean:
	rm -f i2c_access
	rm -f *.o *.d

-include $(SRC:%.c=%.d)
