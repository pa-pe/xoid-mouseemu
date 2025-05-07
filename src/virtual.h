#ifndef VIRTUAL_H
#define VIRTUAL_H

extern int uinput_fd;

int init_virtual_device(void);
void cleanup_virtual_device(void);
const char* get_virtual_device_path(void);
//void set_virtual_device_path(const char *path);

#endif // VIRTUAL_H