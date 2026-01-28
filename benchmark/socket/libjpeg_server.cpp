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

void handle_ipc_jpeg_create_compress(int client_fd) {
    j_compress_ptr cinfo;
    socket_recv(client_fd, (unsigned char*)&cinfo, sizeof(cinfo));
    
    jpeg_create_compress(cinfo);
    
    uint32_t ack = 1;
    socket_send(client_fd, (unsigned char*)&ack, sizeof(ack));
}

void handle_ipc_jpeg_mem_dest(int client_fd) {
    j_compress_ptr cinfo;
    unsigned char** outbuffer;
    unsigned long* outsize;
    
    socket_recv(client_fd, (unsigned char*)&cinfo, sizeof(cinfo));
    socket_recv(client_fd, (unsigned char*)&outbuffer, sizeof(outbuffer));
    socket_recv(client_fd, (unsigned char*)&outsize, sizeof(outsize));
    
    jpeg_mem_dest(cinfo, outbuffer, outsize);
    
    uint32_t ack = 1;
    socket_send(client_fd, (unsigned char*)&ack, sizeof(ack));
}

void handle_ipc_jpeg_set_defaults(int client_fd) {
    j_compress_ptr cinfo;
    socket_recv(client_fd, (unsigned char*)&cinfo, sizeof(cinfo));
    
    jpeg_set_defaults(cinfo);
    
    uint32_t ack = 1;
    socket_send(client_fd, (unsigned char*)&ack, sizeof(ack));
}

void handle_ipc_jpeg_set_quality(int client_fd) {
    j_compress_ptr cinfo;
    int quality;
    boolean force_baseline;
    
    socket_recv(client_fd, (unsigned char*)&cinfo, sizeof(cinfo));
    socket_recv(client_fd, (unsigned char*)&quality, sizeof(quality));
    socket_recv(client_fd, (unsigned char*)&force_baseline, sizeof(force_baseline));
    
    jpeg_set_quality(cinfo, quality, force_baseline);
    
    uint32_t ack = 1;
    socket_send(client_fd, (unsigned char*)&ack, sizeof(ack));
}

void handle_ipc_jpeg_start_compress(int client_fd) {
    j_compress_ptr cinfo;
    boolean write_all_tables;
    
    socket_recv(client_fd, (unsigned char*)&cinfo, sizeof(cinfo));
    socket_recv(client_fd, (unsigned char*)&write_all_tables, sizeof(write_all_tables));
    
    jpeg_start_compress(cinfo, write_all_tables);
    
    uint32_t ack = 1;
    socket_send(client_fd, (unsigned char*)&ack, sizeof(ack));
}

void handle_ipc_jpeg_write_scanlines(int client_fd) {
    j_compress_ptr cinfo;
    JSAMPARRAY scanlines;
    JDIMENSION num_lines;
    
    socket_recv(client_fd, (unsigned char*)&cinfo, sizeof(cinfo));
    socket_recv(client_fd, (unsigned char*)&scanlines, sizeof(scanlines));
    socket_recv(client_fd, (unsigned char*)&num_lines, sizeof(num_lines));
    
    JDIMENSION result = jpeg_write_scanlines(cinfo, scanlines, num_lines);
    
    socket_send(client_fd, (unsigned char*)&result, sizeof(result));
}

void handle_ipc_jpeg_finish_compress(int client_fd) {
    j_compress_ptr cinfo;
    socket_recv(client_fd, (unsigned char*)&cinfo, sizeof(cinfo));
    
    jpeg_finish_compress(cinfo);
    
    uint32_t ack = 1;
    socket_send(client_fd, (unsigned char*)&ack, sizeof(ack));
}

void handle_ipc_jpeg_destroy_compress(int client_fd) {
    j_compress_ptr cinfo;
    socket_recv(client_fd, (unsigned char*)&cinfo, sizeof(cinfo));
    
    jpeg_destroy_compress(cinfo);
    
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
                // std::cout << "JPEG STD ERRROR HANDLER" << std::endl;
                break;
            case IPC_JPEG_CREATE_DECOMPRESS:
                handle_ipc_jpeg_create_decompress(client_fd);
                // std::cout << "JPEG CREATE DECOMPRESS" << std::endl;
                break;
            case IPC_JPEG_MEM_SRC:
                handle_ipc_jpeg_mem_src(client_fd);
                // std::cout << "JPEG MEM SRC" << std::endl;
                break;
            case IPC_JPEG_READ_HEADER:
                handle_ipc_jpeg_read_header(client_fd);
                // std::cout << "JPEG READ HEADER" << std::endl;
                break;
            case IPC_JPEG_START_DECOMPRESS:
                handle_ipc_jpeg_start_decompress(client_fd);
                // std::cout << "JPEG START DECOMPRESS" << std::endl;
                break;
            case IPC_JPEG_READ_SCANLINES:
                handle_ipc_jpeg_read_scanlines(client_fd);
                // std::cout << "JPEG READ SCANLINES" << std::endl;
                break;
            case IPC_JPEG_FINISH_DECOMPRESS:
                handle_ipc_jpeg_finish_decompress(client_fd);
                // std::cout << "JPEG FINISH DECOMPRESS" << std::endl;
                break;
            case IPC_JPEG_DESTROY_DECOMPRESS:
                handle_ipc_jpeg_destroy_decompress(client_fd);
                // std::cout << "JPEG DESTROY DECOMPRESS" << std::endl;
                break;
            case IPC_JPEG_CREATE_COMPRESS:
                handle_ipc_jpeg_create_compress(client_fd);
                // std::cout << "JPEG CREATE COMPRESS" << std::endl;
                break;
            case IPC_JPEG_MEM_DEST:
                handle_ipc_jpeg_mem_dest(client_fd);
                // std::cout << "JPEG MEM DEST" << std::endl;
                break;
            case IPC_JPEG_SET_DEFAULTS:
                handle_ipc_jpeg_set_defaults(client_fd);
                // std::cout << "JPEG SET DEFAULTS" << std::endl;
                break;
            case IPC_JPEG_SET_QUALITY:
                handle_ipc_jpeg_set_quality(client_fd);
                // std::cout << "JPEG SET QUALITY" << std::endl;
                break;
            case IPC_JPEG_START_COMPRESS:
                handle_ipc_jpeg_start_compress(client_fd);
                // std::cout << "JPEG START COMPRESS" << std::endl;
                break;
            case IPC_JPEG_WRITE_SCANLINES:
                handle_ipc_jpeg_write_scanlines(client_fd);
                // std::cout << "JPEG WRITE SCANLINES" << std::endl;
                break;
            case IPC_JPEG_FINISH_COMPRESS:
                handle_ipc_jpeg_finish_compress(client_fd);
                // std::cout << "JPEG FINISH COMPRESS" << std::endl;
                break;
            case IPC_JPEG_DESTROY_COMPRESS:
                handle_ipc_jpeg_destroy_compress(client_fd);
                // std::cout << "JPEG DESTROY COMPRESS" << std::endl;
                break;
            case IPC_TERMINATE:
                // std::cout << "[JPEG TERMINATE] IPC count: " << ipc_counter << std::endl;
                return;
        }
    }
}
