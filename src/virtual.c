#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <linux/uinput.h>
#include <sys/ioctl.h>
#include "config.h"
#include "virtual.h"

int uinput_fd = -1;
static char device_path[64] = "(not initialized)";

const char* get_virtual_device_path() {
    return device_path;
}

int init_virtual_device(void) {
    struct uinput_user_dev uidev;

    uinput_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (uinput_fd < 0) return -1;

    ioctl(uinput_fd, UI_SET_EVBIT, EV_REL);
    ioctl(uinput_fd, UI_SET_RELBIT, REL_X);
    ioctl(uinput_fd, UI_SET_RELBIT, REL_Y);
    ioctl(uinput_fd, UI_SET_RELBIT, REL_WHEEL);

//    ioctl(uinput_fd, UI_SET_EVBIT, EV_ABS);
//    ioctl(uinput_fd, UI_SET_ABSBIT, ABS_X);
//    ioctl(uinput_fd, UI_SET_ABSBIT, ABS_Y);

    ioctl(uinput_fd, UI_SET_EVBIT, EV_KEY);
    ioctl(uinput_fd, UI_SET_KEYBIT, BTN_LEFT);
    ioctl(uinput_fd, UI_SET_KEYBIT, BTN_RIGHT);

    ioctl(uinput_fd, UI_SET_PROPBIT, INPUT_PROP_POINTER);
//    ioctl(uinput_fd, UI_SET_PROPBIT, INPUT_PROP_DIRECT);  // Для тачпадов/тачскринов

    memset(&uidev, 0, sizeof(uidev));
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, DEVICE_NAME);
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor  = 0x1234;
    uidev.id.product = 0x5678;
    uidev.id.version = 1;

//    uidev.absmin[ABS_X] = 0;
//    uidev.absmax[ABS_X] = 1920;
//    uidev.absmin[ABS_Y] = 0;
//    uidev.absmax[ABS_Y] = 1080;

    write(uinput_fd, &uidev, sizeof(uidev));
    ioctl(uinput_fd, UI_DEV_CREATE);

    snprintf(device_path, sizeof(device_path), "(virtual device created, path unknown)");
    return uinput_fd;
}

void cleanup_virtual_device(void) {
    if (uinput_fd >= 0) {
        ioctl(uinput_fd, UI_DEV_DESTROY);
        close(uinput_fd);
        uinput_fd = -1;
    }
}
