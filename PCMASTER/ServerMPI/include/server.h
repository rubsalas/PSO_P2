#ifndef SERVER_H
#define SERVER_H

#define MAX 1024
#define PORT 8080

int initialize_server();
int accept_client(int server_fd);
void close_connection(int client_fd);

#endif // SERVER_H
