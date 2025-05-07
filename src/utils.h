#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

void set_workdir(const char *path);
const char *get_workdir();
int file_exists(const char *path);
void unlink_file(const char *filename);

void get_pid_file_path(char *buf, size_t size);
void get_crash_file_path(char *buf, size_t size);
void get_socket_path(char *buf, size_t size);

#endif // UTILS_H
