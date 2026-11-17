#include <stdio.h>
#include <unistd.h>
#include <sysmon.h>

void *cpu_sensor_thread(void *arg)
{
    SystemState *state = (SystemState *)arg;
    while (1) {
        pthread_mutex_lock(&state->lock);
        int running = state->running;
        pthread_mutex_unlock(&state->lock);

        if (!running) {
            break;
        }
        FILE *file = fopen("/proc/loadavg", "r");

        if (file == NULL) {
            perror("fopen");
            return NULL;
        }

        double load1 = 0;
        double load5 = 0;
        double load15 = 0;

        if (fscanf(file, "%lf %lf %lf", &load1, &load5, &load15) != 3) {
            printf("Error reading CPU load\n");
            fclose(file);
            return NULL;
        }

        fclose(file);
        pthread_mutex_lock(&state->lock);

        state->load1 = load1;
        state->load5 = load5;
        state->load15 = load15;

        if (load1 > 1.50) {
             state->alert_pending = 1;
            pthread_cond_signal(&state->alert_cond);
        }

        pthread_mutex_unlock(&state->lock);



       
        sleep(UPDATE_INTERVAL);
    }
    return NULL;
}