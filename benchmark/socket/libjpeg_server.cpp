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
    
    uint32_t ack = 1;
    socket_send(client_fd, (unsigned char*)&ack, sizeof(ack));
}

void handle_ipc_jpeg_mem_src(int client_fd) {
    j_decompress_ptr cinfo;
    unsigned char* buffer;
    unsigned long size;
    
    socket_recv(client_fd, (unsigned char*)&cinfo, sizeof(cinfo));
    socket_recv(client_fd, (unsigned char*)&buffer, sizeof(buffer));
    socket_recv(client_fd, (unsigned char*)&size, sizeof(size));
    
    jpeg_mem_src(cinfo, buffer, size);
    
    uint32_t ack = 1;
    socket_send(client_fd, (unsigned char*)&ack, sizeof(ack));
}

void handle_ipc_jpeg_read_header(int client_fd) {
    j_decompress_ptr cinfo;
    boolean require_image;
    
    socket_recv(client_fd, (unsigned char*)&cinfo, sizeof(cinfo));
    socket_recv(client_fd, (unsigned char*)&require_image, sizeof(require_image));
    
    jpeg_read_header(cinfo, require_image);
    
    uint32_t ack = 1;
    socket_send(client_fd, (unsigned char*)&ack, sizeof(ack));
}

void handle_ipc_jpeg_start_decompress(int client_fd) {
    j_decompress_ptr cinfo;
    socket_recv(client_fd, (unsigned char*)&cinfo, sizeof(cinfo));
    
    jpeg_start_decompress(cinfo);
    
    uint32_t ack = 1;
    socket_send(client_fd, (unsigned char*)&ack, sizeof(ack));
}

void handle_ipc_jpeg_read_scanlines(int client_fd) {
    j_decompress_ptr cinfo;
    JSAMPARRAY buffer;
    JDIMENSION max_lines;
    
    socket_recv(client_fd, (unsigned char*)&cinfo, sizeof(cinfo));
    socket_recv(client_fd, (unsigned char*)&buffer, sizeof(buffer));
    socket_recv(client_fd, (unsigned char*)&max_lines, sizeof(max_lines));
    
    jpeg_read_scanlines(cinfo, buffer, max_lines);
    
    uint32_t ack = 1;
    socket_send(client_fd, (unsigned char*)&ack, sizeof(ack));
}

void handle_ipc_jpeg_finish_decompress(int client_fd) {
    j_decompress_ptr cinfo;
    socket_recv(client_fd, (unsigned char*)&cinfo, sizeof(cinfo));
    
    jpeg_finish_decompress(cinfo);
    
    uint32_t ack = 1;
    socket_send(client_fd, (unsigned char*)&ack, sizeof(ack));
}

void handle_ipc_jpeg_destroy_decompress(int client_fd) {
    j_decompress_ptr cinfo;
    socket_recv(client_fd, (unsigned char*)&cinfo, sizeof(cinfo));
    
    jpeg_destroy_decompress(cinfo);
    
    uint32_t ack = 1;
    socket_send(client_fd, (unsigned char*)&ack, sizeof(ack));
}

void server_run(int client_fd) {
    int ipc_counter = 0;

    // Server loop
    while(true) {
        uint32_t cmd;
        socket_recv(client_fd, (unsigned char*)&cmd, sizeof(cmd));

        ipc_counter++;

        switch(cmd) {
            case IPC_JPEG_STD_ERROR:
                handle_ipc_jpeg_std_error(client_fd);
                std::cout << "JPEG STD ERRROR HANDLER" << std::endl;
                break;
            case IPC_JPEG_CREATE_DECOMPRESS:
                handle_ipc_jpeg_create_decompress(client_fd);
                std::cout << "JPEG CREATE DECOMPRESS" << std::endl;
                break;
            case IPC_JPEG_MEM_SRC:
                handle_ipc_jpeg_mem_src(client_fd);
                std::cout << "JPEG MEM SRC" << std::endl;
                break;
            case IPC_JPEG_READ_HEADER:
                handle_ipc_jpeg_read_header(client_fd);
                std::cout << "JPEG READ HEADER" << std::endl;
                break;
            case IPC_JPEG_START_DECOMPRESS:
                handle_ipc_jpeg_start_decompress(client_fd);
                std::cout << "JPEG START DECOMPRESS" << std::endl;
                break;
            case IPC_JPEG_READ_SCANLINES:
                handle_ipc_jpeg_read_scanlines(client_fd);
                // std::cout << "JPEG READ SCANLINES" << std::endl;
                break;
            case IPC_JPEG_FINISH_DECOMPRESS:
                handle_ipc_jpeg_finish_decompress(client_fd);
                std::cout << "JPEG FINISH DECOMPRESS" << std::endl;
                break;
            case IPC_JPEG_DESTROY_DECOMPRESS:
                handle_ipc_jpeg_destroy_decompress(client_fd);
                std::cout << "JPEG DESTROY DECOMPRESS" << std::endl;
                break;
            case IPC_TERMINATE:
                std::cout << "[JPEG TERMINATE] IPC count: " << ipc_counter << std::endl;
                return;
        }
    }
}
