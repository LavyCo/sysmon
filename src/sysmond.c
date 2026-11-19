#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sysmon_ipc.h>
#include <errno.h>




pid_t mem_pid, cpu_pid, proc_pid;

volatile sig_atomic_t running = 1;


void stop_handler(int sig)
{
    (void)sig;
    running = 0;
}
void wait_for_child(pid_t pid)
{
    if (pid <= 0)
        return;

    while (waitpid(pid, NULL, 0) == -1) {
        if (errno == EINTR)
            continue;

        if (errno != ECHILD)
            perror("waitpid");

        break;
    }
}
int read_sensor_data(int fd, void *data, size_t size, const char *name)
{
    size_t total = 0;

    while (total < size && running) {
        ssize_t bytes = read(fd, (char *)data + total, size - total);

        if (bytes == -1) {
            if (errno == EINTR)
                continue;

            perror(name);
            return 0;
        }

        if (bytes == 0) {
            fprintf(stderr, "%s: pipe closed\n", name);
            return 0;
        }

        total += bytes;
    }

    return total == size && running;
}

int main(void)
{
    struct sigaction sa = {0};

    sa.sa_handler = stop_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    



    int mem_pipe[2];

    if (pipe(mem_pipe) == -1) {
        perror("pipe");
        return 1;
    }

    if (unlink(CPU_FIFO_PATH) == -1 && errno != ENOENT) {
        perror("unlink CPU");
        return 1;
    }

    if (mkfifo(CPU_FIFO_PATH, 0666) == -1) {
        perror("mkfifo CPU");
        return 1;
    }

    if (unlink(PROC_FIFO_PATH) == -1 && errno != ENOENT) {
        perror("unlink PROC");
        return 1;
    }

    if (mkfifo(PROC_FIFO_PATH, 0666) == -1) {
        perror("mkfifo PROC");
        return 1;
    }

   

   
    mem_pid = fork();

    if (mem_pid < 0) {
        perror("fork mem_sensor");
        return 1;
    }

    if (mem_pid == 0) {
        close(mem_pipe[0]);

        char fd_str[20];
        snprintf(fd_str, sizeof(fd_str), "%d", mem_pipe[1]);

        char *args[] = {"mem_sensor", fd_str, NULL};

        extern char **environ;
        execve("./bin/mem_sensor", args, environ);

        perror("execve mem_sensor");
        _exit(1);
    }
    close(mem_pipe[1]);

    
    cpu_pid = fork();

    if (cpu_pid < 0) {
        perror("fork cpu_sensor");
        return 1;
    }

    if (cpu_pid == 0) {
        close(mem_pipe[0]);

        char *args[] = {"cpu_sensor", NULL};

        extern char **environ;
        execve("./bin/cpu_sensor", args, environ);

        perror("execve cpu_sensor");
        _exit(1);
    }

    
    proc_pid = fork();

    if (proc_pid < 0) {
        perror("fork proc_sensor");
        return 1;
    }

    if (proc_pid == 0) {
        close(mem_pipe[0]);

        char *args[] = {"proc_sensor", NULL};

        extern char **environ;
        execve("./bin/proc_sensor", args, environ);

        perror("execve proc_sensor");
        _exit(1);
    }
    int cpu_fd = open(CPU_FIFO_PATH, O_RDONLY);

    if (cpu_fd == -1) {
        perror("open CPU FIFO");
        return 1;
    }

    int proc_fd = open(PROC_FIFO_PATH, O_RDONLY);

    if (proc_fd == -1) {
        perror("open PROC FIFO");
        close(cpu_fd);
        return 1;
    }
    MemData mem_data;
    CpuData cpu_data;
    ProcData proc_data;
    while (running) {
            if (!read_sensor_data(mem_pipe[0], &mem_data, sizeof(mem_data), "memory"))
                break;

            if (!read_sensor_data(cpu_fd, &cpu_data, sizeof(cpu_data), "CPU"))
                break;

            if (!read_sensor_data(proc_fd, &proc_data, sizeof(proc_data), "processes"))
                break;



        printf("\n=== MEMORY ===\n");
        printf("MemTotal: %ld kB\n", mem_data.mem_total);
        printf("MemFree: %ld kB\n", mem_data.mem_free);
        printf("MemAvailable: %ld kB\n", mem_data.mem_avail);
        printf("SwapTotal: %ld kB\n", mem_data.swap_total);
        printf("SwapFree: %ld kB\n", mem_data.swap_free);

        printf("\n=== CPU ===\n");
        printf("Load1: %.2f\n", cpu_data.load1);
        printf("Load5: %.2f\n", cpu_data.load5);
        printf("Load15: %.2f\n", cpu_data.load15);

        printf("\n=== PROCESSES ===\n");
        printf("Active processes: %d\n", proc_data.total_procs);

        printf("\n===================================\n");
    }
        if (mem_pid > 0)
            kill(mem_pid, SIGTERM);

        if (cpu_pid > 0)
            kill(cpu_pid, SIGTERM);

        if (proc_pid > 0)
            kill(proc_pid, SIGTERM);

        wait_for_child(mem_pid);
        wait_for_child(cpu_pid);
        wait_for_child(proc_pid);

        close(mem_pipe[0]);
        close(cpu_fd);
        close(proc_fd);

        unlink(CPU_FIFO_PATH);
        unlink(PROC_FIFO_PATH);

        return 0;
    }
