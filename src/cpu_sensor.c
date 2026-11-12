#include <stdio.h>
#include <unistd.h>

int main(void)
{
    while(1){
        FILE *file = fopen("/proc/loadavg", "r");

        if (file == NULL) {
            perror("fopen");
            return 1;
        }

        double load1 = 0;
        double load5 = 0;
        double load15 = 0;

        if (fscanf(file, "%lf %lf %lf", &load1, &load5, &load15) != 3) {
            printf("Error reading CPU load\n");
            fclose(file);
            return 1;
        }

        fclose(file);

        FILE *output = fopen("/tmp/sysmon/cpu.dat", "w");

        if (output == NULL) {
            perror("fopen");
            return 1;
        }

        fprintf(output, "Load1: %.2f\n", load1);
        fprintf(output, "Load5: %.2f\n", load5);
        fprintf(output, "Load15: %.2f\n", load15);

        fclose(output);

        printf("Load 1 min: %.2f\n", load1);
        printf("Load 5 min: %.2f\n", load5);
        printf("Load 15 min: %.2f\n", load15);
        sleep(5);
    }
    return 0;
}