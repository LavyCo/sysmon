#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void read_sensor(const char *filename)
{
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        perror(filename);
        return;
    }

    char line[256];

    while (fgets(line, sizeof(line), file) != NULL) {
        printf("%s", line);
    }

    fclose(file);
}

int main(void)
{
    int status = system("mkdir -p /tmp/sysmon");

    if (status != 0) {
        printf("Failed to create directory\n");
        return 1;
    }

    printf("Storage directory is ready\n");
    while(1){

        printf("\n=== MEMORY ===\n");
        read_sensor("/tmp/sysmon/mem.dat");

        printf("\n=== CPU ===\n");
        read_sensor("/tmp/sysmon/cpu.dat");

        printf("\n=== PROCESSES ===\n");
        read_sensor("/tmp/sysmon/proc.dat");
        printf("\n===================================\n");
        sleep(5);
    }
    return 0;
}