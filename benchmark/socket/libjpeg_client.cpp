
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

void ipc_jpeg_mem_src(int server_fd, j_decompress_ptr cinfo, unsigned char* buffer, unsigned long size) {
    uint32_t ipc_call_num = IPC_JPEG_MEM_SRC;
    socket_send(server_fd, (unsigned char*)&ipc_call_num, sizeof(ipc_call_num));

    socket_send(server_fd, (unsigned char*)&cinfo, sizeof(cinfo));
    socket_send(server_fd, (unsigned char*)&buffer, sizeof(buffer));
    socket_send(server_fd, (unsigned char*)&size, sizeof(size));

    uint32_t ack;
    socket_recv(server_fd, (unsigned char*)&ack, sizeof(ack));
}

void ipc_jpeg_read_header(int server_fd, j_decompress_ptr cinfo, boolean require_image) {
    uint32_t ipc_call_num = IPC_JPEG_READ_HEADER;
    socket_send(server_fd, (unsigned char*)&ipc_call_num, sizeof(ipc_call_num));

    socket_send(server_fd, (unsigned char*)&cinfo, sizeof(cinfo));
    socket_send(server_fd, (unsigned char*)&require_image, sizeof(require_image));

    uint32_t ack;
    socket_recv(server_fd, (unsigned char*)&ack, sizeof(ack));
}

void ipc_jpeg_start_decompress(int server_fd, j_decompress_ptr cinfo) {
    uint32_t ipc_call_num = IPC_JPEG_START_DECOMPRESS;
    socket_send(server_fd, (unsigned char*)&ipc_call_num, sizeof(ipc_call_num));

    socket_send(server_fd, (unsigned char*)&cinfo, sizeof(cinfo));

    uint32_t ack;
    socket_recv(server_fd, (unsigned char*)&ack, sizeof(ack));
}

void ipc_jpeg_read_scanlines(int server_fd, j_decompress_ptr cinfo, JSAMPARRAY buffer, JDIMENSION max_lines) {
    uint32_t ipc_call_num = IPC_JPEG_READ_SCANLINES;
    socket_send(server_fd, (unsigned char*)&ipc_call_num, sizeof(ipc_call_num));

    socket_send(server_fd, (unsigned char*)&cinfo, sizeof(cinfo));
    socket_send(server_fd, (unsigned char*)&buffer, sizeof(buffer));
    socket_send(server_fd, (unsigned char*)&max_lines, sizeof(max_lines));

    uint32_t ack;
    socket_recv(server_fd, (unsigned char*)&ack, sizeof(ack));
}

void ipc_jpeg_finish_decompress(int server_fd, j_decompress_ptr cinfo) {
    uint32_t ipc_call_num = IPC_JPEG_FINISH_DECOMPRESS;
    socket_send(server_fd, (unsigned char*)&ipc_call_num, sizeof(ipc_call_num));

    socket_send(server_fd, (unsigned char*)&cinfo, sizeof(cinfo));

    uint32_t ack;
    socket_recv(server_fd, (unsigned char*)&ack, sizeof(ack));
}

void ipc_jpeg_destroy_decompress(int server_fd, j_decompress_ptr cinfo) {
    uint32_t ipc_call_num = IPC_JPEG_DESTROY_DECOMPRESS;
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
    // Libjpeg uses a pointer to the input buffer, copy to shared memory so it's accessible on the server
    unsigned char* shared_input = (unsigned char*)mspace_malloc(shared_heap, fsize);
    memcpy(shared_input, fileBuff, fsize);

    struct jpeg_parsed_data ret = {0};

    struct jpeg_decompress_struct* cinfo = (struct jpeg_decompress_struct*)mspace_malloc(shared_heap, sizeof(struct jpeg_decompress_struct));
    memset(cinfo, 0, sizeof(struct jpeg_decompress_struct));

    struct jpeg_error_mgr* jerr = (struct jpeg_error_mgr*)mspace_malloc(shared_heap, sizeof(struct jpeg_error_mgr));
    memset(jerr, 0, sizeof(struct jpeg_error_mgr));

    cinfo->err = ipc_jpeg_std_error(server_fd, jerr);
    jerr->error_exit = my_error_exit;

    ipc_jpeg_create_decompress(server_fd, cinfo);

    ipc_jpeg_mem_src(server_fd, cinfo, shared_input, fsize);
    ipc_jpeg_read_header(server_fd, cinfo, TRUE);
    ipc_jpeg_start_decompress(server_fd, cinfo);

    int row_stride = cinfo->output_width * cinfo->output_components;

    ret.image_height = cinfo->output_height;
    ret.image_width = cinfo->output_width;
    ret.image_buffer_size = ret.image_width * ret.image_height * 3 * sizeof(JSAMPLE);
    ret.image_buffer = (JSAMPLE*)mspace_malloc(shared_heap, ret.image_buffer_size);

    RELEASE_ASSERT(ret.image_buffer, "Memory alloc failure");

    int curr_image_row = 0;

    // Allocate array pointer in shared memory for jpeg_read_scanlines
    JSAMPLE** row_pointer = (JSAMPLE**)mspace_malloc(shared_heap, sizeof(JSAMPLE*));

    while (cinfo->output_scanline < cinfo->output_height) {
        JSAMPLE* target = &(ret.image_buffer[curr_image_row * row_stride]);
        *row_pointer = target;
        ipc_jpeg_read_scanlines(server_fd, cinfo, row_pointer, 1);
        curr_image_row++;
    }

    ipc_jpeg_finish_decompress(server_fd, cinfo);
    ipc_jpeg_destroy_decompress(server_fd, cinfo);

    return ret;
}
