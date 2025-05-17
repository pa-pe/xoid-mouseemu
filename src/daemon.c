#include "daemon.h"
#include "config.h"
#include "socket.h"
#include "utils.h"
#include "virtual.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <errno.h>

static char pid_path[256];
volatile sig_atomic_t running = 1;

int write_pid_file(void) {
    char pidfile[PATH_MAX];
    get_pid_file_path(pidfile, sizeof(pidfile));

    FILE *f = fopen(pidfile, "w");
    if (!f) {
        perror("fopen (pidfile)");
        return -1;
    }

    if (fprintf(f, "%d\n", getpid()) < 0) {
        perror("fprintf (pidfile)");
        fclose(f);
        return -1;
    }

    fclose(f);
    return 0;
}

static int read_pid_file(void) {
    get_pid_file_path(pid_path, sizeof(pid_path));
    FILE *f = fopen(pid_path, "r");
    if (!f) return -1;
    int pid = -1;
    fscanf(f, "%d", &pid);
    fclose(f);
    return pid;
}

static void remove_pid_file(void) {
    if (pid_path[0]) unlink(pid_path);
}

static void on_signal(int sig) {
    (void)sig;  // подавление IDE warning
    running = 0;
    cleanup_virtual_device();
    cleanup_socket_server();
    remove_pid_file();
    exit(EXIT_SUCCESS);
}

static int is_already_running(void) {
    int pid = read_pid_file();
    if (pid <= 0) return 0;
    if (kill(pid, 0) == 0) {
        printf("mouseemu already running (PID %d)\n", pid);
        return 1;
    }
    return 0;
}

int run_daemon(void) {
//    if (is_already_running()) return 1;
    if (is_already_running()) {
        print_status();
        return 1;
    }

    // Демонизация
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }
    if (pid > 0) {
        fflush(stdout); // гарантируем вывод до завершения родителя
        printf("%s v%s started with PID %d\n", APP_NAME, VERSION, pid);
        print_status();
        return 0;
    }

    setsid();
    umask(0);
    chdir("/");

    // Закрываем стандартные потоки для корректного отделения демона при запуске через adb
    fclose(stdin);
    fclose(stdout);
    fclose(stderr);
    freopen("/dev/null", "r", stdin);
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);

    signal(SIGINT, on_signal);
    signal(SIGTERM, on_signal);

    int dev_fd = init_virtual_device();
    if (dev_fd < 0) {
        fprintf(stderr, "Failed to initialize virtual device\n");
        exit(EXIT_FAILURE);
    }

    if (write_pid_file() != 0) {
        fprintf(stderr, "Failed to write PID file\n");
        cleanup_virtual_device();
        exit(EXIT_FAILURE);
    }

    if (start_socket_server() != 0) {
        fprintf(stderr, "Socket server failed\n");
        cleanup_virtual_device();
        remove_pid_file();
        exit(EXIT_FAILURE);
    }

    // Демон завершился (например, по ошибке)
    cleanup_virtual_device();
    remove_pid_file();
    return 0;
}

int stop_daemon(void) {
    get_pid_file_path(pid_path, sizeof(pid_path));

    FILE *fp = fopen(pid_path, "r");
    if (!fp) {
        printf("%s is not running.\n", APP_NAME);
        return 1;
    }

    int pid = 0;
    fscanf(fp, "%d", &pid);
    fclose(fp);

    // Попытка послать SIGTERM
    if (kill(pid, SIGTERM) != 0) {
        perror("kill");
        return 1;
    }

    // Ожидаем завершения
    for (int i = 0; i < 20; i++) {
        if (kill(pid, 0) != 0) {
            unlink(pid_path);
            printf("%s stopped.\n", APP_NAME);
            return 0;
        }
        usleep(100000);
    }

    printf("Failed to stop %s (PID %d)\n", APP_NAME, pid);
    return 1;
}

int print_status(void) {
    return request_status();
}
