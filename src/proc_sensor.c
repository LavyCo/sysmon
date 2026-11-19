#include <stdio.h>
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sysmon_ipc.h>
int main(void)
{
    int fd = open(PROC_FIFO_PATH, O_WRONLY);

    if (fd == -1) {
        perror("open FIFO");
        return 1;
    }
    while (1) {
        DIR *dir = opendir("/proc");

        if (dir == NULL) {
            perror("opendir");
            close(fd);
            return 1;
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
        ProcData data;

        data.total_procs = count;

        ssize_t bytes = write(fd, &data, sizeof(data));

        if (bytes == -1) {
            perror("write");
            close(fd);
            return 1;
        }

        if (bytes != sizeof(data)) {
            fprintf(stderr, "Incomplete process data write\n");
            close(fd);
            return 1;
        }



      
        sleep(1);
    }
    return 0;
}