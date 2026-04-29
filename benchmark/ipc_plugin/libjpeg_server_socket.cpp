#include "common.cpp"
#include "socket.cpp"
#include "jpeglib.h"

#ifdef IPC_INSTRUMENT
static uint64_t server_work_ns = 0;
static uint64_t server_call_count = 0;
#endif

static mspace shared_heap = nullptr;
static unsigned char** compress_outbuffer = nullptr;
static unsigned long*  compress_outsize   = nullptr;
static unsigned char*  local_compress_buf  = nullptr;
static unsigned long   local_compress_size = 0;


void handle_ipc_jpeg_std_error(int client_fd) {
    jpeg_error_mgr* err;
    socket_recv(client_fd, (unsigned char*)&err, sizeof(err));
#ifdef IPC_INSTRUMENT
    uint64_t ts = now_ns();
#endif
    jpeg_error_mgr* result = jpeg_std_error(err);
#ifdef IPC_INSTRUMENT
    server_work_ns += now_ns() - ts; server_call_count++;
#endif
    socket_send(client_fd, (unsigned char*)&result, sizeof(result));
}

void handle_ipc_jpeg_create_decompress(int client_fd) {
    j_decompress_ptr cinfo;
    socket_recv(client_fd, (unsigned char*)&cinfo, sizeof(cinfo));
#ifdef IPC_INSTRUMENT
    uint64_t ts = now_ns();
#endif
    jpeg_create_decompress(cinfo);
#ifdef IPC_INSTRUMENT
    server_work_ns += now_ns() - ts; server_call_count++;
#endif
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
#ifdef IPC_INSTRUMENT
    uint64_t ts = now_ns();
#endif
    jpeg_mem_src(cinfo, buffer, size);
#ifdef IPC_INSTRUMENT
    server_work_ns += now_ns() - ts; server_call_count++;
#endif
    uint32_t ack = 1;
    socket_send(client_fd, (unsigned char*)&ack, sizeof(ack));
}

void handle_ipc_jpeg_read_header(int client_fd) {
    j_decompress_ptr cinfo;
    boolean require_image;

    socket_recv(client_fd, (unsigned char*)&cinfo, sizeof(cinfo));
    socket_recv(client_fd, (unsigned char*)&require_image, sizeof(require_image));
#ifdef IPC_INSTRUMENT
    uint64_t ts = now_ns();
#endif
    int result = jpeg_read_header(cinfo, require_image);
#ifdef IPC_INSTRUMENT
    server_work_ns += now_ns() - ts; server_call_count++;
#endif
    socket_send(client_fd, (unsigned char*)&result, sizeof(result));
}

void handle_ipc_jpeg_start_decompress(int client_fd) {
    j_decompress_ptr cinfo;
    socket_recv(client_fd, (unsigned char*)&cinfo, sizeof(cinfo));
#ifdef IPC_INSTRUMENT
    uint64_t ts = now_ns();
#endif
    boolean result = jpeg_start_decompress(cinfo);
#ifdef IPC_INSTRUMENT
    server_work_ns += now_ns() - ts; server_call_count++;
#endif
    socket_send(client_fd, (unsigned char*)&result, sizeof(result));
}

void handle_ipc_jpeg_read_scanlines(int client_fd) {
    j_decompress_ptr cinfo;
    JSAMPARRAY buffer;
    JDIMENSION max_lines;

    socket_recv(client_fd, (unsigned char*)&cinfo, sizeof(cinfo));
    socket_recv(client_fd, (unsigned char*)&buffer, sizeof(buffer));
    socket_recv(client_fd, (unsigned char*)&max_lines, sizeof(max_lines));
#ifdef IPC_INSTRUMENT
    uint64_t ts = now_ns();
#endif
    JDIMENSION result = jpeg_read_scanlines(cinfo, buffer, max_lines);
#ifdef IPC_INSTRUMENT
    server_work_ns += now_ns() - ts; server_call_count++;
#endif
    socket_send(client_fd, (unsigned char*)&result, sizeof(result));
}

void handle_ipc_jpeg_finish_decompress(int client_fd) {
    j_decompress_ptr cinfo;
    socket_recv(client_fd, (unsigned char*)&cinfo, sizeof(cinfo));
#ifdef IPC_INSTRUMENT
    uint64_t ts = now_ns();
#endif
    boolean result = jpeg_finish_decompress(cinfo);
#ifdef IPC_INSTRUMENT
    server_work_ns += now_ns() - ts; server_call_count++;
#endif
    socket_send(client_fd, (unsigned char*)&result, sizeof(result));
}

void handle_ipc_jpeg_destroy_decompress(int client_fd) {
    j_decompress_ptr cinfo;
    socket_recv(client_fd, (unsigned char*)&cinfo, sizeof(cinfo));
#ifdef IPC_INSTRUMENT
    uint64_t ts = now_ns();
#endif
    jpeg_destroy_decompress(cinfo);
#ifdef IPC_INSTRUMENT
    server_work_ns += now_ns() - ts; server_call_count++;
#endif
    uint32_t ack = 1;
    socket_send(client_fd, (unsigned char*)&ack, sizeof(ack));
}

void handle_ipc_jpeg_create_compress(int client_fd) {
    j_compress_ptr cinfo;
    socket_recv(client_fd, (unsigned char*)&cinfo, sizeof(cinfo));
#ifdef IPC_INSTRUMENT
    uint64_t ts = now_ns();
#endif
    jpeg_create_compress(cinfo);
#ifdef IPC_INSTRUMENT
    server_work_ns += now_ns() - ts; server_call_count++;
#endif
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

    compress_outbuffer = outbuffer;
    compress_outsize   = outsize;
    local_compress_buf  = nullptr;
    local_compress_size = 0;
#ifdef IPC_INSTRUMENT
    uint64_t ts = now_ns();
#endif
    jpeg_mem_dest(cinfo, &local_compress_buf, &local_compress_size);
#ifdef IPC_INSTRUMENT
    server_work_ns += now_ns() - ts; server_call_count++;
#endif
    uint32_t ack = 1;
    socket_send(client_fd, (unsigned char*)&ack, sizeof(ack));
}

void handle_ipc_jpeg_set_defaults(int client_fd) {
    j_compress_ptr cinfo;
    socket_recv(client_fd, (unsigned char*)&cinfo, sizeof(cinfo));
#ifdef IPC_INSTRUMENT
    uint64_t ts = now_ns();
#endif
    jpeg_set_defaults(cinfo);
#ifdef IPC_INSTRUMENT
    server_work_ns += now_ns() - ts; server_call_count++;
#endif
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
#ifdef IPC_INSTRUMENT
    uint64_t ts = now_ns();
#endif
    jpeg_set_quality(cinfo, quality, force_baseline);
#ifdef IPC_INSTRUMENT
    server_work_ns += now_ns() - ts; server_call_count++;
#endif
    uint32_t ack = 1;
    socket_send(client_fd, (unsigned char*)&ack, sizeof(ack));
}

void handle_ipc_jpeg_start_compress(int client_fd) {
    j_compress_ptr cinfo;
    boolean write_all_tables;

    socket_recv(client_fd, (unsigned char*)&cinfo, sizeof(cinfo));
    socket_recv(client_fd, (unsigned char*)&write_all_tables, sizeof(write_all_tables));
#ifdef IPC_INSTRUMENT
    uint64_t ts = now_ns();
#endif
    jpeg_start_compress(cinfo, write_all_tables);
#ifdef IPC_INSTRUMENT
    server_work_ns += now_ns() - ts; server_call_count++;
#endif
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
#ifdef IPC_INSTRUMENT
    uint64_t ts = now_ns();
#endif
    JDIMENSION result = jpeg_write_scanlines(cinfo, scanlines, num_lines);
#ifdef IPC_INSTRUMENT
    server_work_ns += now_ns() - ts; server_call_count++;
#endif
    socket_send(client_fd, (unsigned char*)&result, sizeof(result));
}

void handle_ipc_jpeg_finish_compress(int client_fd) {
    j_compress_ptr cinfo;
    socket_recv(client_fd, (unsigned char*)&cinfo, sizeof(cinfo));
#ifdef IPC_INSTRUMENT
    uint64_t ts = now_ns();
#endif
    jpeg_finish_compress(cinfo);
    unsigned char* shm_buf = (unsigned char*)mspace_malloc(shared_heap, local_compress_size);
    memcpy(shm_buf, local_compress_buf, local_compress_size);
    free(local_compress_buf);
    local_compress_buf     = nullptr;
    *compress_outbuffer    = shm_buf;
    *compress_outsize      = local_compress_size;
#ifdef IPC_INSTRUMENT
    server_work_ns += now_ns() - ts; server_call_count++;
#endif
    uint32_t ack = 1;
    socket_send(client_fd, (unsigned char*)&ack, sizeof(ack));
}

void handle_ipc_jpeg_destroy_compress(int client_fd) {
    j_compress_ptr cinfo;
    socket_recv(client_fd, (unsigned char*)&cinfo, sizeof(cinfo));
#ifdef IPC_INSTRUMENT
    uint64_t ts = now_ns();
#endif
    jpeg_destroy_compress(cinfo);
#ifdef IPC_INSTRUMENT
    server_work_ns += now_ns() - ts; server_call_count++;
#endif
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
#ifdef IPC_INSTRUMENT
                printf("server_calls\t%llu\n",      (unsigned long long)server_call_count);
                printf("server_work_total\t%llu\n", (unsigned long long)server_work_ns);
#endif
                return;
        }
    }
}

int main() {
    cpu_pin(IPC_SERVER_CPU);
    int server_fd = socket_setup_server();

    int shm_fd;
    void* shm_ptr;
    shared_heap = shared_memory_setup(shm_fd, shm_ptr, true);

    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0) { perror("accept"); }

    server_run(client_fd);

    munmap(shm_ptr, SHARED_MEM_SIZE);
    shm_unlink(SHARED_MEM_NAME);
    close(shm_fd);
    close(client_fd);
    close(server_fd);

    return 0;
}
