#include "common.cpp"
#include "socket.cpp"
#include "jpeglib.h"


void handle_ipc_jpeg_std_error(int client_fd) {
    jpeg_error_mgr* err;
    socket_recv(client_fd, (unsigned char*)&err, sizeof(err));
    
    jpeg_error_mgr* result = jpeg_std_error(err);
    
    socket_send(client_fd, (unsigned char*)&result, sizeof(result));
}

void handle_ipc_jpeg_create_decompress(int client_fd) {
    j_decompress_ptr cinfo;
    socket_recv(client_fd, (unsigned char*)&cinfo, sizeof(cinfo));
    
    jpeg_create_decompress(cinfo);
    
    // No return value, just send ACK
    uint32_t ack = 1;
    socket_send(client_fd, (unsigned char*)&ack, sizeof(ack));
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
            case IPC_JPEG_CREATE_DECOMPRESS:
                handle_ipc_jpeg_create_decompress(client_fd);
                std::cout << "JPEG CREATE DECOMPRESS" << std::endl;
                break;
            case IPC_TERMINATE:
                std::cout << "JPEG TERMINATE" << std::endl;
                return;
        }
    }
}
