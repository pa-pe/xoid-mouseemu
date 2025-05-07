#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include "config.h"
#include "utils.h"
#include <sys/stat.h>

static char workdir[PATH_MAX] = DEFAULT_WORKDIR;

void set_workdir(const char *path) {
    strncpy(workdir, path, PATH_MAX - 1);
}

const char *get_workdir() {
    return workdir;
}

int file_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}

void unlink_file(const char *filename) {
    char fullpath[PATH_MAX];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", get_workdir(), filename);
    unlink(fullpath);
}

void get_pid_file_path(char *buf, size_t size) {
    snprintf(buf, size, "%s/%s.pid", get_workdir(), APP_NAME);
}

void get_crash_file_path(char *buf, size_t size) {
    snprintf(buf, size, "%s/%s.crash", get_workdir(), APP_NAME);
}

void get_socket_path(char *buf, size_t size) {
    snprintf(buf, size, "%s/%s.sock", get_workdir(), APP_NAME);
}
