
CC = gcc
CFLAGS = -Wall -g -Iinc -pthread

SRC = src/sysmond.c src/mem_sensor.c src/cpu_sensor.c src/proc_sensor.c
TARGET = bin/sysmond

all: $(TARGET)

$(TARGET): $(SRC) inc/sysmon.h
	mkdir -p bin
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
