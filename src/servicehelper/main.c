#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

static void printUsage()
{
    fprintf(stderr, "usage: crashreporter-servicehelper journalspy/endurance start/stop\n");
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printUsage();
        return EXIT_FAILURE;
    }

    char *serviceName;
    if (strcmp(argv[1], "journalspy") == 0) {
        serviceName = "crash-reporter-journalspy.service";
    } else if (strcmp(argv[1], "endurance") == 0) {
        serviceName = "crash-reporter-endurance.service";
    } else {
        printUsage();
        return EXIT_FAILURE;
    }

    bool start;
    if (strcmp(argv[2], "start") == 0) {
        start = true;
    } else if (strcmp(argv[2], "stop") == 0) {
        start = false;
    } else {
        printUsage();
        return EXIT_FAILURE;
    }

    return execlp("systemctl", "systemctl", (start ? "start" : "stop"), serviceName, NULL);
}
