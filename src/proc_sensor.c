#include <stdio.h>
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>
#include <sysmon.h>

void *proc_sensor_thread(void *arg)
{
    SystemState *state = (SystemState *)arg;
    while (1) {
        pthread_mutex_lock(&state->lock);
        int running = state->running;
        pthread_mutex_unlock(&state->lock);

        if (!running) {
            break;
        }
        DIR *dir = opendir("/proc");

        if (dir == NULL) {
            perror("opendir");
            return NULL;
        }

        struct dirent *entry;
        int count = 0;

        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_DIR) {
                int i = 0;

                while (entry->d_name[i] != '\0' &&
                    isdigit((unsigned char)entry->d_name[i])) {
                    i++;
                }

                if (i > 0 && entry->d_name[i] == '\0') {
                    count++;
                }
            }
        }

        closedir(dir);
        pthread_mutex_lock(&state->lock);

        state->total_procs = count;

        pthread_mutex_unlock(&state->lock);


        sleep(UPDATE_INTERVAL);
    }
    return NULL;
}