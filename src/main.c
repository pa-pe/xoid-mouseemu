#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"
#include "socket.h"
#include "daemon.h"
#include "utils.h"
#include "log.h"
#include "help.h"

int main(int argc, char *argv[]) {
    const char *cmd = NULL;
    const char *workdir = DEFAULT_WORKDIR;
    char *log_file = DEFAULT_LOGFILE;

    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--workdir=", 10) == 0) {
            workdir = argv[i] + 10;
        } else if (strcmp(argv[i], "--log") == 0 && i + 1 < argc) {
            log_file = argv[++i];
        } else if (!cmd) {
            cmd = argv[i];
        }
    }

    if (!cmd) {
        fprintf(stderr, "%s", get_full_usage());
        return 1;
    }

    set_workdir(workdir);
    log_init(log_file);

    if (strcmp(cmd, "start") == 0) {
        return run_daemon();
    }

    if (strcmp(cmd, "stop") == 0) {
        return stop_daemon();
    }

    if (strcmp(cmd, "status") == 0) {
        return print_status();
    }

    if (strcmp(cmd, "help") == 0) {
        fprintf(stderr, "%s", get_full_usage());
        return 0;
    }

//    if (!cmd)
    fprintf(stderr, "Unknown command: %s\n", cmd);
    fprintf(stderr, "%s", get_full_usage());

    log_close();
    return 1;
}
