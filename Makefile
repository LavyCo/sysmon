CC = gcc
CFLAGS = -Wall -g

TARGETS = bin/sysmond bin/cpu_sensor bin/mem_sensor bin/proc_sensor

all: $(TARGETS)

bin/sysmond: src/sysmond.c
	$(CC) $(CFLAGS) $< -o $@

bin/cpu_sensor: src/cpu_sensor.c
	$(CC) $(CFLAGS) $< -o $@

bin/mem_sensor: src/mem_sensor.c
	$(CC) $(CFLAGS) $< -o $@

bin/proc_sensor: src/proc_sensor.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGETS)
