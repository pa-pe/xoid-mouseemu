#ifndef MOUSEEMU_LOG_H
#define MOUSEEMU_LOG_H

#include <stdio.h>

void log_init(char *log_path);
void log_msg(const char *fmt, ...);
void log_close(void);
const char *get_log_path(void);

#endif // MOUSEEMU_LOG_H
