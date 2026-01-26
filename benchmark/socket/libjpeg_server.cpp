#include "common.cpp"
#include "socket.cpp"
#include "jpeglib.h"


void handle_ipc_jpeg_std_error(int client_fd) {
    jpeg_error_mgr* err;
    socket_recv(client_fd, (unsigned char*)&err, sizeof(err));
    
    jpeg_error_mgr* result = jpeg_std_error(err);
    
    socket_send(client_fd, (unsigned char*)&result, sizeof(result));
}

void server_run(int client_fd) {
    // Server loop
    while(true) {
        uint32_t cmd;
        socket_recv(client_fd, (unsigned char*)&cmd, sizeof(cmd));

        switch(cmd) {
            case IPC_JPEG_STD_ERROR:
                handle_ipc_jpeg_std_error(client_fd);
                std::cout << "JPEG STD ERRROR HANDLER" << std::endl;
                break;
            case IPC_TERMINATE:
                std::cout << "JPEG TERMINATE" << std::endl;
                return;
        }
    }
}
