#include "log.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "config.h"
#include "utils.h"

static FILE *log_fp = NULL;
static const char *log_path_saved = NULL;

void log_init(char *log_path) {
    if (!log_path || strcmp(log_path, "disabled")  == 0) return;

    if (strcmp(log_path, "1") == 0){
        const char *workdir = get_workdir();
        printf("Workdir: %s\n", workdir);
//        log_path = "./mouseemu.log";
        snprintf(log_path, PATH_MAX, "%s/%s.log", get_workdir(), APP_NAME);
    }
    log_path_saved = log_path;

    log_fp = fopen(log_path, "a");
    if (!log_fp) {
        fprintf(stderr, "ERROR: cannot open log file: %s\n", log_path);
        exit(EXIT_FAILURE);
    }

    atexit(log_close);
}

void log_msg(const char *fmt, ...) {
    if (!log_fp) return;

    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_buf[20];
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(log_fp, "[%s] ", time_buf);

    va_list args;
    va_start(args, fmt);
    vfprintf(log_fp, fmt, args);
    va_end(args);

    fprintf(log_fp, "\n");
    fflush(log_fp);
}

void log_close(void) {
    if (log_fp) {
        fclose(log_fp);
        log_fp = NULL;
    }
}

const char *get_log_path(void) {
    return log_fp ? log_path_saved : "disabled";
}