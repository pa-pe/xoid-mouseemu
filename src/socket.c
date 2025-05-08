#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "utils.h"
#include "mouse.h"
#include "daemon.h"
#include "virtual.h"
#include "log.h"
#include <sys/time.h>

static char sock_path[256] = {0};
static int server_fd = -1;
static struct timeval last_command_time = {0};
#define COMMAND_INTERVAL_US 50000  // 50 мс

void throttle_commands(void) {
    struct timeval now;
    gettimeofday(&now, NULL);

    // first run
    if (last_command_time.tv_sec == 0 && last_command_time.tv_usec == 0) {
        last_command_time = now;
        return;
    }

    long elapsed_us = (now.tv_sec - last_command_time.tv_sec) * 1000000L +
                      (now.tv_usec - last_command_time.tv_usec);

    if (elapsed_us < COMMAND_INTERVAL_US)
        usleep(COMMAND_INTERVAL_US - elapsed_us);

    gettimeofday(&last_command_time, NULL);
}

int handle_command(int client_fd, const char *command) {
    log_msg("Received command: %s", command);
    throttle_commands();  // Обеспечить интервал между командами что бы драйвер успел обработать предыдущую команду

    if (strncmp(command, "move ", 5) == 0) {
        int x, y;
        if (sscanf(command + 5, "%d %d", &x, &y) == 2) {
            if (move_mouse_relative(x, y) == 0)
                dprintf(client_fd, "OK: moved by %d %d\n", x, y);
            else
                dprintf(client_fd, "ERROR: move failed\n");
        } else {
            dprintf(client_fd, "ERROR: invalid move command\n");
        }
    } else if (strncmp(command, "amove ", 6) == 0) {
        int x, y;
        if (sscanf(command + 6, "%d %d", &x, &y) == 2) {
            if (move_mouse_absolute(x, y) == 0)
                dprintf(client_fd, "OK: moved to %d %d\n", x, y);
            else
                dprintf(client_fd, "ERROR: amove failed\n");
        } else {
            dprintf(client_fd, "ERROR: invalid amove command\n");
        }
    } else if (strncmp(command, "click", 5) == 0) {
        if (click_mouse() == 0)
            dprintf(client_fd, "OK: click sent\n");
        else
            dprintf(client_fd, "ERROR: click failed\n");
    } else if (strncmp(command, "scroll ", 7) == 0) {
        int delta;
        if (sscanf(command + 7, "%d", &delta) == 1) {
            if (scroll_mouse(delta) == 0)
                dprintf(client_fd, "OK: scrolled by %d\n", delta);
            else
                dprintf(client_fd, "ERROR: scroll failed\n");
        } else {
            dprintf(client_fd, "ERROR: invalid scroll command\n");
        }
    } else if (strncmp(command, "status", 6) == 0) {
        dprintf(client_fd,
                "%s v%s\nPID: %d\nSocket: %s\nLog: %s\n",
                APP_NAME,
                VERSION,
                getpid(),
                sock_path,
                get_log_path());
        return 1;  // <--- Сигнал закрыть соединение что бы клиент не остался в ожидании продолжения
    } else {
        dprintf(client_fd, "ERROR: unknown command\n");
    }
    return 0;
}


int socket_listener_loop(int (*handler)(int, const char *)) {
    get_socket_path(sock_path, sizeof(sock_path));
    unlink(sock_path);

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) return -1;

    struct sockaddr_un addr = {0};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr *) &addr, sizeof(addr)) != 0 ||
        listen(server_fd, 5) != 0) {
        close(server_fd);
        server_fd = -1;
        return -1;
    }

    while (running) {
        int client = accept(server_fd, NULL, NULL);
        if (client < 0) continue;

        FILE *client_fp = fdopen(client, "r+");
        if (!client_fp) {
            close(client);
            continue;
        }

        char buffer[MAX_CMD_LEN];
        while (fgets(buffer, sizeof(buffer), client_fp)) {
            buffer[strcspn(buffer, "\r\n")] = 0;  // Удаляем \n или \r\n
            if (strlen(buffer) == 0) continue;
            int should_close = handle_command(client, buffer);
            fflush(client_fp);
            if (should_close) break;  // <-- выйти из цикла и закрыть соединение по велению handle_command
        }

        fclose(client_fp);
    }

    return 0;
}

int start_socket_server(void) {
    get_socket_path(sock_path, sizeof(sock_path));
    printf("Socket server started on: %s\n", sock_path);
    return socket_listener_loop(handle_command);
}

void cleanup_socket_server(void) {
    if (server_fd >= 0) {
        close(server_fd);
        server_fd = -1;
    }
    if (sock_path[0]) {
        unlink(sock_path);
        printf("Socket removed: %s\n", sock_path);
    }
}

int send_command_and_read_response(const char *command) {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_un addr = {0};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path) - 1);

    if (connect(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("connect");
        close(fd);
        return 1;
    }

    dprintf(fd, "%s\n", command);
    fsync(fd);

    char buffer[512];
    ssize_t len;
    while ((len = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[len] = 0;
        printf("%s", buffer);
    }

    close(fd);
    return 0;
}

int request_status(void) {
    const char *workdir = get_workdir();
    printf("Workdir: %s\n", workdir);

    char pidfile[256];
    get_pid_file_path(pidfile, sizeof(pidfile));

    FILE *fp = fopen(pidfile, "r");
    if (!fp) {
        printf("PID file not found: %s\n", pidfile);
        return 1;
    }

    int pid = 0;
    if (fscanf(fp, "%d", &pid) != 1 || pid <= 0) {
        fclose(fp);
        printf("Invalid PID content in: %s\n", pidfile);
        return 1;
    }
    fclose(fp);

    // Проверка, жив ли процесс
    if (kill(pid, 0) != 0) {
        printf("Stale PID file: process %d is not running.\n", pid);
        unlink(pidfile);
        return 1;
    }

// Проверка сокета
    get_socket_path(sock_path, sizeof(sock_path));
    if (access(sock_path, F_OK) != 0) {
        printf("Socket not found: %s\n", sock_path);
        return 1;
    }

    // Запросим статус у демона
    return send_command_and_read_response("status");
}
