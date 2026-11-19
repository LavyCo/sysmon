
## Phase 3 – Multi-Process IPC

Implemented a supervised multi-process architecture using Linux IPC.

- **fork() + execve():** `sysmond` automatically launches and manages three sensor processes.
- **Anonymous Pipe:** Transfers memory data from `mem_sensor` to `sysmond`.
- **Named Pipes (FIFOs):** Transfer CPU and process statistics.
- **Binary IPC:** Sensors transmit C structures instead of writing data files.
- **Live Monitoring:** Displays memory, CPU load, and active process statistics.
- **Graceful Shutdown:** Handles SIGINT/SIGTERM, terminates child processes, and cleans up FIFOs.

**Testing:** Successfully verified live data updates, parent-child process relationships, FIFO creation, and cleanup after Ctrl+C.
