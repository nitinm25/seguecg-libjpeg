
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

void ipc_jpeg_create_compress(int server_fd, j_compress_ptr cinfo) {
    uint32_t ipc_call_num = IPC_JPEG_CREATE_COMPRESS;
    socket_send(server_fd, (unsigned char*)&ipc_call_num, sizeof(ipc_call_num));

    socket_send(server_fd, (unsigned char*)&cinfo, sizeof(cinfo));

    uint32_t ack;
    socket_recv(server_fd, (unsigned char*)&ack, sizeof(ack));
}

void ipc_jpeg_mem_dest(int server_fd, j_compress_ptr cinfo, unsigned char** outbuffer, unsigned long* outsize) {
    uint32_t ipc_call_num = IPC_JPEG_MEM_DEST;
    socket_send(server_fd, (unsigned char*)&ipc_call_num, sizeof(ipc_call_num));

    socket_send(server_fd, (unsigned char*)&cinfo, sizeof(cinfo));
    socket_send(server_fd, (unsigned char*)&outbuffer, sizeof(outbuffer));
    socket_send(server_fd, (unsigned char*)&outsize, sizeof(outsize));

    uint32_t ack;
    socket_recv(server_fd, (unsigned char*)&ack, sizeof(ack));
}

void ipc_jpeg_set_defaults(int server_fd, j_compress_ptr cinfo) {
    uint32_t ipc_call_num = IPC_JPEG_SET_DEFAULTS;
    socket_send(server_fd, (unsigned char*)&ipc_call_num, sizeof(ipc_call_num));

    socket_send(server_fd, (unsigned char*)&cinfo, sizeof(cinfo));

    uint32_t ack;
    socket_recv(server_fd, (unsigned char*)&ack, sizeof(ack));
}

void ipc_jpeg_set_quality(int server_fd, j_compress_ptr cinfo, int quality, boolean force_baseline) {
    uint32_t ipc_call_num = IPC_JPEG_SET_QUALITY;
    socket_send(server_fd, (unsigned char*)&ipc_call_num, sizeof(ipc_call_num));

    socket_send(server_fd, (unsigned char*)&cinfo, sizeof(cinfo));
    socket_send(server_fd, (unsigned char*)&quality, sizeof(quality));
    socket_send(server_fd, (unsigned char*)&force_baseline, sizeof(force_baseline));

    uint32_t ack;
    socket_recv(server_fd, (unsigned char*)&ack, sizeof(ack));
}

void ipc_jpeg_start_compress(int server_fd, j_compress_ptr cinfo, boolean write_all_tables) {
    uint32_t ipc_call_num = IPC_JPEG_START_COMPRESS;
    socket_send(server_fd, (unsigned char*)&ipc_call_num, sizeof(ipc_call_num));

    socket_send(server_fd, (unsigned char*)&cinfo, sizeof(cinfo));
    socket_send(server_fd, (unsigned char*)&write_all_tables, sizeof(write_all_tables));

    uint32_t ack;
    socket_recv(server_fd, (unsigned char*)&ack, sizeof(ack));
}

JDIMENSION ipc_jpeg_write_scanlines(int server_fd, j_compress_ptr cinfo, JSAMPARRAY scanlines, JDIMENSION num_lines) {
    uint32_t ipc_call_num = IPC_JPEG_WRITE_SCANLINES;
    socket_send(server_fd, (unsigned char*)&ipc_call_num, sizeof(ipc_call_num));

    socket_send(server_fd, (unsigned char*)&cinfo, sizeof(cinfo));
    socket_send(server_fd, (unsigned char*)&scanlines, sizeof(scanlines));
    socket_send(server_fd, (unsigned char*)&num_lines, sizeof(num_lines));

    JDIMENSION result;
    socket_recv(server_fd, (unsigned char*)&result, sizeof(result));
    return result;
}

void ipc_jpeg_finish_compress(int server_fd, j_compress_ptr cinfo) {
    uint32_t ipc_call_num = IPC_JPEG_FINISH_COMPRESS;
    socket_send(server_fd, (unsigned char*)&ipc_call_num, sizeof(ipc_call_num));

    socket_send(server_fd, (unsigned char*)&cinfo, sizeof(cinfo));

    uint32_t ack;
    socket_recv(server_fd, (unsigned char*)&ack, sizeof(ack));
}

void ipc_jpeg_destroy_compress(int server_fd, j_compress_ptr cinfo) {
    uint32_t ipc_call_num = IPC_JPEG_DESTROY_COMPRESS;
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

struct jpeg_parsed_data write_jpeg(int server_fd, mspace shared_heap, int quality, struct jpeg_parsed_data input) {
    struct jpeg_parsed_data ret = {0};

    struct jpeg_compress_struct* cinfo = (struct jpeg_compress_struct*)mspace_malloc(shared_heap, sizeof(struct jpeg_compress_struct));
    memset(cinfo, 0, sizeof(struct jpeg_compress_struct));

    struct jpeg_error_mgr* jerr = (struct jpeg_error_mgr*)mspace_malloc(shared_heap, sizeof(struct jpeg_error_mgr));
    memset(jerr, 0, sizeof(struct jpeg_error_mgr));

    cinfo->err = ipc_jpeg_std_error(server_fd, jerr);
    ipc_jpeg_create_compress(server_fd, cinfo);

    unsigned char** outbuffer_ptr = (unsigned char**)mspace_malloc(shared_heap, sizeof(unsigned char*));
    unsigned long* outsize_ptr = (unsigned long*)mspace_malloc(shared_heap, sizeof(unsigned long));
    *outbuffer_ptr = NULL;
    *outsize_ptr = 0;

    ipc_jpeg_mem_dest(server_fd, cinfo, outbuffer_ptr, outsize_ptr);

    cinfo->image_width = input.image_width;
    cinfo->image_height = input.image_height;
    cinfo->input_components = 3;
    cinfo->in_color_space = JCS_RGB;

    ipc_jpeg_set_defaults(server_fd, cinfo);
    ipc_jpeg_set_quality(server_fd, cinfo, quality, TRUE);
    ipc_jpeg_start_compress(server_fd, cinfo, TRUE);

    // Allocate row pointer in shared memory
    JSAMPLE** row_pointer = (JSAMPLE**)mspace_malloc(shared_heap, sizeof(JSAMPLE*));
    int row_stride = input.image_width * 3;

    while (cinfo->next_scanline < cinfo->image_height) {
        *row_pointer = &input.image_buffer[cinfo->next_scanline * row_stride];
        ipc_jpeg_write_scanlines(server_fd, cinfo, row_pointer, 1);
    }

    ipc_jpeg_finish_compress(server_fd, cinfo);

    ret.image_width = cinfo->image_width;
    ret.image_height = cinfo->image_height;
    ret.image_buffer_size = *outsize_ptr;
    ret.image_buffer = *outbuffer_ptr;

    ipc_jpeg_destroy_compress(server_fd, cinfo);

    return ret;
}
