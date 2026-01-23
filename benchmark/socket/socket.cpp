#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/domain_socket.sock"


int socket_send(int client_fd, const unsigned char* data, size_t size) {
    ssize_t total_sent = 0;
    while (total_sent < size) {
        ssize_t sent = send(client_fd, data + total_sent, size - total_sent, 0);
        if (sent < 0) {
            perror("send");
            return -1;
        }
        total_sent += sent;
    }
    return total_sent;
}

int socket_recv(int client_fd, unsigned char* buffer, size_t size) {
    ssize_t total_received = 0;
    while (total_received < size) {
        ssize_t received = recv(client_fd, buffer + total_received, size - total_received, 0);
        if (received < 0) {
            perror("recv");
            return -1;
        }
        if (received == 0) {
            // Connection closed by peer
            break;
        }
        total_received += received;
    }
    return total_received;
}

int socket_setup_client() {
    int server_fd;

    struct sockaddr_un server_addr;

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if (connect(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(server_fd);
        return 1;
    }
    
    // printf("Connected to server\n");

    return server_fd;
}

int socket_setup_server() {
    int server_fd;

    struct sockaddr_un server_addr;

    unlink(SOCKET_PATH);

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        close(server_fd);
        return 1;
    }
    
    // printf("Server listening on %s\n", SOCKET_PATH);

    return server_fd;
}
