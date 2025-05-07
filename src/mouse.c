#include <unistd.h>
#include <linux/uinput.h>
#include "virtual.h"  // для uinput_fd
#include "mouse.h"

int move_mouse_relative(int dx, int dy) {
    struct input_event ev;

    gettimeofday(&ev.time, NULL);
    ev.type = EV_REL;
    ev.code = REL_X;
    ev.value = dx;
    if (write(uinput_fd, &ev, sizeof(ev)) < 0) return -1;

    gettimeofday(&ev.time, NULL);
    ev.code = REL_Y;
    ev.value = dy;
    if (write(uinput_fd, &ev, sizeof(ev)) < 0) return -1;

    gettimeofday(&ev.time, NULL);
    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;
    if (write(uinput_fd, &ev, sizeof(ev)) < 0) return -1;

    return 0;
}

int move_mouse_absolute(int x, int y) {
    struct input_event ev;

    gettimeofday(&ev.time, NULL);
    ev.type = EV_ABS;
    ev.code = ABS_X;
    ev.value = x;
    if (write(uinput_fd, &ev, sizeof(ev)) < 0) return -1;

    gettimeofday(&ev.time, NULL);
    ev.type = EV_ABS;
    ev.code = ABS_Y;
    ev.value = y;
    if (write(uinput_fd, &ev, sizeof(ev)) < 0) return -1;

    gettimeofday(&ev.time, NULL);
    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;
    if (write(uinput_fd, &ev, sizeof(ev)) < 0) return -1;

    return 0;
}

int click_mouse(void) {
    struct input_event ev;

    gettimeofday(&ev.time, NULL);
    ev.type = EV_KEY;
    ev.code = BTN_LEFT;
    ev.value = 1; // Нажатие
    if (write(uinput_fd, &ev, sizeof(ev)) < 0) return -1;

    gettimeofday(&ev.time, NULL);
    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;
    if (write(uinput_fd, &ev, sizeof(ev)) < 0) return -1;

    usleep(10000); // 10 мс

    gettimeofday(&ev.time, NULL);
    ev.type = EV_KEY;
    ev.code = BTN_LEFT;
    ev.value = 0; // Отпускание
    if (write(uinput_fd, &ev, sizeof(ev)) < 0) return -1;

    gettimeofday(&ev.time, NULL);
    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;
    if (write(uinput_fd, &ev, sizeof(ev)) < 0) return -1;

    return 0;
}

int scroll_mouse(int delta) {
    struct input_event ev = {0};
    gettimeofday(&ev.time, NULL);
    ev.type = EV_REL;
    ev.code = REL_WHEEL;
    ev.value = delta;
    if (write(uinput_fd, &ev, sizeof(ev)) < 0) return -1;

    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;
    if (write(uinput_fd, &ev, sizeof(ev)) < 0) return -1;

    return 0;
}
