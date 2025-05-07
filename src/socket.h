#ifndef SOCKET_H
#define SOCKET_H
//#include <stddef.h>

//int send_command(const char *cmd);
//int socket_listener_loop(int (*handle_command)(const char *));
//int start_socket_server(const char *sock_path, int dev_fd);
//static void respond_status(int client_fd);

int start_socket_server(void);
void cleanup_socket_server(void);
//int handle_command(const char *cmd);
int handle_command(int client_fd, const char *command);
//static void get_socket_path(char *buffer, size_t size);
int request_status(void);

#endif // SOCKET_H
