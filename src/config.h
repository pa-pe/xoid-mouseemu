#ifndef MOUSEEMU_H
#define MOUSEEMU_H

#define DEVICE_NAME "Virtual Mouse by Xoid"
#define APP_NAME    "xoid_mouseemu"
#define VERSION     "1.1.21"

#define DEFAULT_SOCKET_PATH "/data/local/tmp/mouseemu.sock"
#define DEFAULT_WORKDIR     "/data/local/tmp"
#define DEFAULT_LOGFILE     "disabled"

#define MAX_CMD_LEN 256

void handle_signal(int sig);

#endif
