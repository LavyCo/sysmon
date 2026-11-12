# SysMon – Linux System Monitoring Tool

## Overview
SysMon is a Linux system monitoring project written in C as part of my Embedded Linux course.

In Phase 1, I built a multi-process application that collects system information from the `/proc` filesystem and displays it in the terminal.

## Implementation
The project consists of four programs:

- **mem_sensor** – Reads memory and swap information from `/proc/meminfo`.
- **cpu_sensor** – Reads CPU load averages from `/proc/loadavg`.
- **proc_sensor** – Counts active processes by scanning `/proc`.
- **sysmond** – Reads the collected data and displays a combined system report.

Each sensor runs independently and updates its own file in `/tmp/sysmon/` every 5 seconds.

## Build & Run

Compile the project:

```bash
make
```

Run the sensors in the background:

```bash
./bin/mem_sensor &
./bin/cpu_sensor &
./bin/proc_sensor &
```

Start the monitoring program:

```bash
./bin/sysmond
```

## Testing
I tested the sensors running simultaneously, checked that their output files update every 5 seconds, and verified that `sysmond` displays the latest collected data.
