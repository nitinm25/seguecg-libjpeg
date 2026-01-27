
#include <stdio.h>
#include <stdlib.h>

#include "common.cpp"
#include "socket.cpp"
#include "jpeglib.h"

#define RELEASE_ASSERT(cond, msg)           \
  if(!(cond))                               \
  {                                         \
    printf("FAILED: " #cond ". " msg "\n"); \
    exit(1);                                \
  }

void my_error_exit (j_common_ptr cinfo) {
    RELEASE_ASSERT(false, "my_error_exit exit handler called");
}

struct jpeg_parsed_data {
    JSAMPLE* image_buffer;
    size_t image_buffer_size;
    int image_height;
    int image_width;
};


jpeg_error_mgr* ipc_jpeg_std_error(int server_fd, jpeg_error_mgr *err) {
    uint32_t ipc_call_num = IPC_JPEG_STD_ERROR;
    socket_send(server_fd, (unsigned char*)&ipc_call_num, sizeof(ipc_call_num));

    socket_send(server_fd, (unsigned char*)&err, sizeof(err));

    jpeg_error_mgr* result;
    socket_recv(server_fd, (unsigned char*)&result, sizeof(result));

    return result;
}


void ipc_jpeg_create_decompress(int server_fd, j_decompress_ptr cinfo) {
    uint32_t ipc_call_num = IPC_JPEG_CREATE_DECOMPRESS;
    socket_send(server_fd, (unsigned char*)&ipc_call_num, sizeof(ipc_call_num));

    socket_send(server_fd, (unsigned char*)&cinfo, sizeof(cinfo));

    uint32_t ack;
    socket_recv(server_fd, (unsigned char*)&ack, sizeof(ack));
}


void ipc_terminate(int server_fd) {
    uint32_t ipc_call_num = IPC_TERMINATE;
    socket_send(server_fd, (unsigned char*)&ipc_call_num, sizeof(ipc_call_num));
}


struct jpeg_parsed_data read_jpeg(int server_fd, mspace shared_heap, unsigned char *fileBuff, unsigned long fsize) {
    struct jpeg_parsed_data ret = {0};

    struct jpeg_decompress_struct* cinfo = (struct jpeg_decompress_struct*)mspace_malloc(shared_heap, sizeof(struct jpeg_decompress_struct));
    memset(cinfo, 0, sizeof(struct jpeg_decompress_struct));

    struct jpeg_error_mgr* jerr = (struct jpeg_error_mgr*)mspace_malloc(shared_heap, sizeof(struct jpeg_error_mgr));
    memset(jerr, 0, sizeof(struct jpeg_error_mgr));

    cinfo->err = ipc_jpeg_std_error(server_fd, jerr);
    jerr->error_exit = my_error_exit;

    ipc_jpeg_create_decompress(server_fd, cinfo);

    return ret;
}
