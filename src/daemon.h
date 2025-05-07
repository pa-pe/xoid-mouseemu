#ifndef DAEMON_H
#define DAEMON_H

#include <signal.h>

extern volatile sig_atomic_t running;

int run_daemon(void);
int stop_daemon(void);
int print_status(void);

#endif // DAEMON_H
