#include <stdio.h>
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>

int main(void)
{
    while (1) {
        DIR *dir = opendir("/proc");

        if (dir == NULL) {
            perror("opendir");
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

        FILE *output = fopen("/tmp/sysmon/proc.dat", "w");

        if (output == NULL) {
            perror("fopen");
            return 1;
        }

        fprintf(output, "Active processes: %d\n", count);

        fclose(output);

        printf("Active processes: %d\n", count);
        sleep(5);
    }
    return 0;
}