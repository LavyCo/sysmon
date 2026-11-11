
#include <stdio.h>
#include <unistd.h>

int main(void)
{
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
        printf("MemTotal: %ld kB\n", memTotal);
        printf("MemFree: %ld kB\n", memFree);
        printf("MemAvailable: %ld kB\n", memAvailable);
        printf("SwapTotal: %ld kB\n", swapTotal);
        printf("SwapFree: %ld kB\n", swapFree);

        FILE *output = fopen("/tmp/sysmon/mem.dat", "w");

        if (output == NULL) {
            perror("fopen");
            return 1;
        }

        fprintf(output, "MemTotal: %ld kB\n", memTotal);
        fprintf(output, "MemFree: %ld kB\n", memFree);
        fprintf(output, "MemAvailable: %ld kB\n", memAvailable);
        fprintf(output, "SwapTotal: %ld kB\n", swapTotal);
        fprintf(output, "SwapFree: %ld kB\n", swapFree);

        fclose(output);
        sleep(5);
    }
    return 0;
}
