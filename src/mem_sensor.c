#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sysmon_ipc.h>

int main(int argc, char *argv[])
{
    int fd = STDOUT_FILENO;

    if (argc > 1) {
        fd = atoi(argv[1]);
    }
    while (1) {
        FILE *file = fopen("/proc/meminfo", "r");

        if (file == NULL) {
         perror("fopen");
            return 1;
        }

        char line[256];
        long memTotal = 0;
        long memFree = 0;
        long memAvailable = 0;
        long swapTotal = 0;
        long swapFree = 0;

        while (fgets(line, sizeof(line), file) != NULL) {
            if (sscanf(line, "MemTotal: %ld", &memTotal) == 1)
            continue;

            if (sscanf(line, "MemFree: %ld", &memFree) == 1)
                continue;

            if (sscanf(line, "MemAvailable: %ld", &memAvailable) == 1)
                continue;

            if (sscanf(line, "SwapTotal: %ld", &swapTotal) == 1)
                continue;

            if (sscanf(line, "SwapFree: %ld", &swapFree) == 1)
                continue;
        }

        fclose(file);
        MemData data;

        data.mem_total = memTotal;
        data.mem_free = memFree;
        data.mem_avail = memAvailable;
        data.swap_total = swapTotal;
        data.swap_free = swapFree;

        ssize_t bytes = write(fd, &data, sizeof(data));

        if (bytes == -1) {
            perror("write");
            return 1;
        }

        if (bytes != sizeof(data)) {
            fprintf(stderr, "Incomplete write\n");
            return 1;
        }
    


        sleep(1);
    }
    return 0;
}
