CC = gcc
CFLAGS = -Wall -g -Iinc

TARGETS = bin/sysmond bin/cpu_sensor bin/mem_sensor bin/proc_sensor

all: $(TARGETS)

bin/sysmond: src/sysmond.c inc/sysmon_ipc.h
	$(CC) $(CFLAGS) $< -o $@

bin/cpu_sensor: src/cpu_sensor.c inc/sysmon_ipc.h
	$(CC) $(CFLAGS) $< -o $@

bin/mem_sensor: src/mem_sensor.c inc/sysmon_ipc.h
	$(CC) $(CFLAGS) $< -o $@

bin/proc_sensor: src/proc_sensor.c inc/sysmon_ipc.h
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGETS)
