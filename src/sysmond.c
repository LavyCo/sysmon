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
    pid_t pid;

   
    pid = fork();

    if (pid < 0) {
        perror("fork mem_sensor");
        return 1;
    }

    if (pid == 0) {
        execl("./bin/mem_sensor", "mem_sensor", (char *)NULL);
        perror("execl mem_sensor");
        _exit(1);
    }

    
    pid = fork();

    if (pid < 0) {
        perror("fork cpu_sensor");
        return 1;
    }

    if (pid == 0) {
        execl("./bin/cpu_sensor", "cpu_sensor", (char *)NULL);
        perror("execl cpu_sensor");
        _exit(1);
    }

    
    pid = fork();

    if (pid < 0) {
        perror("fork proc_sensor");
        return 1;
    }

    if (pid == 0) {
        execl("./bin/proc_sensor", "proc_sensor", (char *)NULL);
        perror("execl proc_sensor");
        _exit(1);
    }
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