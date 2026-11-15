#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>


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


volatile sig_atomic_t mem_running = 1;
volatile sig_atomic_t cpu_running = 1;
volatile sig_atomic_t proc_running = 1;

pid_t mem_pid, cpu_pid, proc_pid;

void child_handler(int sig)
{
    (void)sig;

    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (pid == mem_pid)
            mem_running = 0;
        else if (pid == cpu_pid)
            cpu_running = 0;
        else if (pid == proc_pid)
            proc_running = 0;
    }
}

int main(void)
{
    signal(SIGCHLD, child_handler);
    int status = system("mkdir -p /tmp/sysmon");

    if (status != 0) {
        printf("Failed to create directory\n");
        return 1;
    }

    printf("Storage directory is ready\n");
   

   
    mem_pid = fork();

    if (mem_pid < 0) {
        perror("fork mem_sensor");
        return 1;
    }

    if (mem_pid == 0) {
        execl("./bin/mem_sensor", "mem_sensor", (char *)NULL);
        perror("execl mem_sensor");
        _exit(1);
    }

    
    cpu_pid = fork();

    if (cpu_pid < 0) {
        perror("fork cpu_sensor");
        return 1;
    }

    if (cpu_pid == 0) {
        execl("./bin/cpu_sensor", "cpu_sensor", (char *)NULL);
        perror("execl cpu_sensor");
        _exit(1);
    }

    
    proc_pid = fork();

    if (proc_pid < 0) {
        perror("fork proc_sensor");
        return 1;
    }

    if (proc_pid == 0) {
        execl("./bin/proc_sensor", "proc_sensor", (char *)NULL);
        perror("execl proc_sensor");
        _exit(1);
    }
    while(1){

        if(mem_running){
        printf("\n=== MEMORY ===\n");
        read_sensor("/tmp/sysmon/mem.dat");
        }
        if(cpu_running){
        printf("\n=== CPU ===\n");
        read_sensor("/tmp/sysmon/cpu.dat");
        }
        if(proc_running){
        printf("\n=== PROCESSES ===\n");
        read_sensor("/tmp/sysmon/proc.dat");
        }
        printf("\n===================================\n");
        sleep(5);
    }
    return 0;
}