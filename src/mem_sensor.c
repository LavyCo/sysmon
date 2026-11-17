
#include <stdio.h>
#include <unistd.h>
#include <sysmon.h>

void *mem_sensor_thread(void *arg)
{
    SystemState *state = (SystemState *)arg;
    while (1) {
        pthread_mutex_lock(&state->lock);
        int running = state->running;
        pthread_mutex_unlock(&state->lock);

        if (!running) {
            break;
        }
        FILE *file = fopen("/proc/meminfo", "r");

        if (file == NULL) {
         perror("fopen");
            return NULL;
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
        pthread_mutex_lock(&state->lock);

        state->mem_total = memTotal;
        state->mem_free = memFree;
        state->mem_avail = memAvailable;
        state->swap_total = swapTotal;
        state->swap_free = swapFree;

        pthread_mutex_unlock(&state->lock);

        sleep(UPDATE_INTERVAL);
    }
     return NULL;

}
