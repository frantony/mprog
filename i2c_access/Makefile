CC = gcc

#MPSSE_CFLAGS = -Ilibmpsse-read-only/src
#MPSSE_LDFLAGS = libmpsse-read-only/src/libmpsse.a
##MPSSE_LDFLAGS = -Llibmpsse-read-only/src -lmpsse
#MPSSE_DRIVER=ft2232_mpsse.o
MPSSE_DRIVER=

CFLAGS = -W -Wall $(MPSSE_CFLAGS)
LDFLAGS = -lftdi $(MPSSE_LDFLAGS)

i2c_access: \
		i2c_access_main.o \
		driver.o \
		devplugin.o \
		ft2232_bb.o \
		$(MPSSE_DRIVER) \
		memory_display.o \
		dc1307.o \
		pca9545.o \
		at24.o \
		i2c_scan.o
	$(CC) $^ -o $@ $(LDFLAGS)

driver.o: driver.c common.h
	$(CC) $(CFLAGS) -c $< -o $@

devplugin.o: devplugin.c common.h list.h i2c_master.h i2c_devplugin.h
	$(CC) $(CFLAGS) -c $< -o $@

ft2232_bb.o: ft2232_bb.c common.h i2c_master.h
	$(CC) $(CFLAGS) -c $< -o $@

dc1307.o: dc1307.c common.h i2c_master.h
	$(CC) $(CFLAGS) -c $< -o $@

pca9545.o: pca9545.c common.h list.h i2c_master.h i2c_devplugin.h
	$(CC) $(CFLAGS) -c $< -o $@

at24.o: at24.c common.h list.h i2c_master.h i2c_devplugin.h
	$(CC) $(CFLAGS) -c $< -o $@

ft2232_mpsse.o: ft2232_mpsse.c common.h i2c_master.h
	$(CC) $(CFLAGS) -c $< -o $@

memory_display.o: memory_display.c common.h
	$(CC) $(CFLAGS) -c $< -o $@

i2c_scan.o: i2c_scan.c common.h i2c_master.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f i2c_access
	rm -f *.o
