#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"
#include "socket.h"
#include "daemon.h"
#include "utils.h"
#include "log.h"

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

//    if (strcmp(cmd, "move") == 0
//        || strcmp(cmd, "click") == 0
//        || strcmp(cmd, "scroll") == 0
//            ) {
//
//        char command[MAX_CMD_LEN] = {0};
//        for (int i = 1; i < argc; i++) {
//            if (strncmp(argv[i], "--", 2) != 0) {
//                strcat(command, argv[i]);
//                if (i < argc - 1) strcat(command, " ");
//            }
//        }

//        if (handle_command(command) == 0) {
//            printf("Command sent: %s\n", command);
//        } else {
//            fprintf(stderr, "Failed to send command: %s\n", command);
//            return 1;
//        }
//
//        return 0;
//    }

    fprintf(stderr, "Unknown command: %s\n", cmd);

    log_close();
    return 1;
}
