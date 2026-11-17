#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sysmon.h>
#include <string.h>

extern void *mem_sensor_thread(void *arg);
extern void *cpu_sensor_thread(void *arg);
extern void *proc_sensor_thread(void *arg);

void *alert_thread(void *arg)
{
    SystemState *state = (SystemState *)arg;

    while (1) {
        pthread_mutex_lock(&state->lock);

        while (!state->alert_pending && state->running) {
            pthread_cond_wait(&state->alert_cond, &state->lock);
        }

        if (!state->running) {
            pthread_mutex_unlock(&state->lock);
            break;
        }

        if (state->alert_pending) {
            fprintf(stderr,
                    "\nALERT: High CPU load: %.2f\n",
                    state->load1);

            state->alert_pending = 0;
        }

        pthread_mutex_unlock(&state->lock);
    }

    return NULL;
}

volatile sig_atomic_t stop_requested = 0;

void handle_sigint(int sig)
{
    (void)sig;
    stop_requested = 1;
}


int main(void)
{
    signal(SIGINT, handle_sigint);

    SystemState state = {0};

    pthread_t threads[4];

    void *(*functions[4])(void *) = {
        alert_thread,
        mem_sensor_thread,
        cpu_sensor_thread,
        proc_sensor_thread
    };

    pthread_mutex_init(&state.lock, NULL);
    pthread_cond_init(&state.alert_cond, NULL);
    state.running = 1;

   int created = 0;
   int ret;

    for (int i = 0; i < 4; i++) {
        ret = pthread_create(&threads[i], NULL, functions[i], &state);

        if (ret != 0) {
            fprintf(stderr, "pthread_create failed: %s\n", strerror(ret));
            break;
        }

        created++;
    }
    if (created != 4) {
        pthread_mutex_lock(&state.lock);
        state.running = 0;
        pthread_cond_broadcast(&state.alert_cond);
        pthread_mutex_unlock(&state.lock);

        for (int i = 0; i < created; i++) {
            pthread_join(threads[i], NULL);
        }

        pthread_cond_destroy(&state.alert_cond);
        pthread_mutex_destroy(&state.lock);

        return 1;
    }
    

    while (!stop_requested) {
    pthread_mutex_lock(&state.lock);

    printf("\n=== MEMORY ===\n");
    printf("MemTotal: %ld kB\n", state.mem_total);
    printf("MemFree: %ld kB\n", state.mem_free);
    printf("MemAvailable: %ld kB\n", state.mem_avail);
    printf("SwapTotal: %ld kB\n", state.swap_total);
    printf("SwapFree: %ld kB\n", state.swap_free);

    printf("\n=== CPU ===\n");
    printf("Load1: %.2f\n", state.load1);
    printf("Load5: %.2f\n", state.load5);
    printf("Load15: %.2f\n", state.load15);

    printf("\n=== PROCESSES ===\n");
    printf("Active processes: %d\n", state.total_procs);

    printf("\n===================================\n");

    pthread_mutex_unlock(&state.lock);

    sleep(UPDATE_INTERVAL);
}
    pthread_mutex_lock(&state.lock);

    state.running = 0;

    pthread_cond_broadcast(&state.alert_cond);

    pthread_mutex_unlock(&state.lock);
    for (int i = 0; i < created; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_cond_destroy(&state.alert_cond);
    pthread_mutex_destroy(&state.lock);

    printf("\nSysmon stopped successfully.\n");
    return 0;
}