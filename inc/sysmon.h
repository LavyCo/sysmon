
#ifndef SYSMON_H
#define SYSMON_H

#include <pthread.h>

#define UPDATE_INTERVAL 2

typedef struct {
    // Memory
    long mem_total;
    long mem_free;
    long mem_avail;
    long swap_total;
    long swap_free;

    // CPU Load
    double load1;
    double load5;
    double load15;

    // Process
    int total_procs;

    // Synchronization
    pthread_mutex_t lock;
    pthread_cond_t alert_cond;

    int running;
    int alert_pending;

} SystemState;

#endif
