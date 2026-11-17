# SysMon – Linux System Monitoring Tool

## Overview

SysMon is a Linux system monitoring project written in C as part of my Embedded Linux course.

The project collects system information from the `/proc` filesystem and displays memory usage, CPU load averages, and active process statistics in the terminal.

The project was developed in two phases:

- **Phase 1:** Multi-process architecture using `fork()`, `exec()` and intermediate data files.
- **Phase 2:** Multi-threaded architecture using POSIX Threads, shared memory, mutexes, and condition variables.

## Phase 1 – Multi-Process Implementation

The original implementation consisted of four separate programs:

- **mem_sensor** – Reads memory and swap information from `/proc/meminfo`.
- **cpu_sensor** – Reads CPU load averages from `/proc/loadavg`.
- **proc_sensor** – Counts active processes by scanning `/proc`.
- **sysmond** – Displays the collected sensor information.

Each sensor updated its own file in `/tmp/sysmon/` every 5 seconds.

The monitoring application used `fork()` and `exec()` to launch the sensors and read their output files.

## Phase 2 – Multi-Threaded Implementation

In Phase 2, I refactored the application into a single multi-threaded process using POSIX Threads (`pthreads`).

The application creates four worker threads:

- **Memory Thread** – Collects memory and swap information.
- **CPU Thread** – Collects CPU load averages.
- **Process Thread** – Counts active processes.
- **Alert Thread** – Waits for high CPU load notifications.

All threads share a `SystemState` structure defined in `inc/sysmon.h`.

### Thread Synchronization

A `pthread_mutex_t` protects access to the shared system information.

A `pthread_cond_t` is used to notify the alert thread when the CPU load exceeds the configured threshold.

When the 1-minute CPU load average exceeds `1.50`, the CPU sensor signals the alert thread, which displays a warning in the terminal.

An `alert_pending` flag is used to track pending notifications.

### Monitoring and Shutdown

The monitoring application displays updated system information every 2 seconds.

The application supports graceful shutdown using `Ctrl+C` (`SIGINT`).

A shared `running` flag notifies the worker threads to stop, and `pthread_join()` waits for their completion before synchronization resources are destroyed.

## Build & Run

Compile the project:

```bash
make
```

Run the monitoring application:

```bash
./bin/sysmond
```

Stop the application:

```text
Ctrl+C
```

Clean the compiled executable:

```bash
make clean
```

## Testing

I tested the multi-threaded implementation and verified that:

- All four worker threads run within the same process.
- Memory, CPU, and process information is collected and displayed.
- Shared data is protected using a mutex.
- CPU load above `1.50` triggers an alert.
- The application shuts down successfully using `Ctrl+C`.
- No SysMon threads remain active after shutdown.

Thread execution was verified using:

```bash
ps -T -C sysmond
```
