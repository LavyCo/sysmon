#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sysmon_ipc.h>

int main(void)
{
    int fd = open(CPU_FIFO_PATH, O_WRONLY);

    if (fd == -1) {
        perror("open FIFO");
        return 1;
    }
    while(1){
        FILE *file = fopen("/proc/loadavg", "r");

        if (file == NULL) {
            perror("fopen");
            close(fd);
            return 1;
        }

        double load1 = 0;
        double load5 = 0;
        double load15 = 0;

        if (fscanf(file, "%lf %lf %lf", &load1, &load5, &load15) != 3) {
            printf("Error reading CPU load\n");
            fclose(file);
            close(fd);
            return 1;
        }

        fclose(file);

        CpuData data;

        data.load1 = load1;
        data.load5 = load5;
        data.load15 = load15;

        ssize_t bytes = write(fd, &data, sizeof(data));

        if (bytes == -1) {
            perror("write");
            close(fd);
            return 1;
        }

        if (bytes != sizeof(data)) {
            fprintf(stderr, "Incomplete CPU data write\n");
            close(fd);
            return 1;
        }

        sleep(1);
    }
    close(fd);
    return 0;
}