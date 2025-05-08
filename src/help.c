#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "help.h"

const char *get_main_usage(void) {
    static char usage[512];
    snprintf(usage, sizeof(usage),
             "Usage:\n"
             "  mouseemu <command> [options]\n\n"
             "Main commands:\n"
             "  start             Start the daemon\n"
             "  stop              Stop the daemon\n"
             "  status            Show daemon status\n"
             "  help              Show this help message\n\n"
             "Options:\n"
             "  --workdir=DIR     Set working directory (default: %s)\n"
             "  --log FILE        Set log file (default: %s, only used with 'start')\n\n",
             DEFAULT_WORKDIR,
             DEFAULT_LOGFILE);
    return usage;
}

const char *get_socket_usage(void) {
    return
            "Socket commands:\n"
            "  move <dx> <dy>     - Move mouse relatively\n"
            //"  amove <x> <y>      - Move mouse to absolute position (not yet implemented)\n"
            "  click              - Perform mouse click\n"
            "  scroll <delta>     - Scroll mouse wheel\n"
            "  status             - Show daemon status\n"
            "  help               - Show this help message\n";
}

const char *get_full_usage(void) {
    // Объединение строк — возвращается указатель на статическую память
    // Не потокобезопасно, но подходит для CLI-утилиты

    static char buffer[2048];
    snprintf(buffer, sizeof(buffer), "%s%s", get_main_usage(), get_socket_usage());
    return buffer;
}
