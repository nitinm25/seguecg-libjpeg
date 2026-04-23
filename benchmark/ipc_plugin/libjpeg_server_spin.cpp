#include "common.cpp"
#include "spin_transport.h"
#include "jpeglib.h"

#ifdef IPC_INSTRUMENT
static uint64_t server_work_ns = 0;
static uint64_t server_call_count = 0;
#endif

static mspace     shared_heap        = nullptr;
static unsigned char** compress_outbuffer = nullptr;
static unsigned long*  compress_outsize   = nullptr;
static unsigned char*  local_compress_buf  = nullptr;
static unsigned long   local_compress_size = 0;

static void server_run(ipc_control* ctrl) {
    while (true) {
        spin_wait(&ctrl->status, SPIN_REQUEST);

        uint32_t cmd = ctrl->cmd;

#ifdef IPC_INSTRUMENT
        uint64_t ts = now_ns();
#endif

        switch (cmd) {
            case IPC_JPEG_STD_ERROR: {
                jpeg_error_mgr* err = (jpeg_error_mgr*)ctrl->args[0];
                ctrl->result = (uint64_t)jpeg_std_error(err);
                break;
            }
            case IPC_JPEG_CREATE_DECOMPRESS: {
                j_decompress_ptr cinfo = (j_decompress_ptr)ctrl->args[0];
                jpeg_create_decompress(cinfo);
                ctrl->result = 1;
                break;
            }
            case IPC_JPEG_MEM_SRC: {
                j_decompress_ptr cinfo  = (j_decompress_ptr)ctrl->args[0];
                unsigned char*   buffer = (unsigned char*)ctrl->args[1];
                unsigned long    size   = (unsigned long)ctrl->args[2];
                jpeg_mem_src(cinfo, buffer, size);
                ctrl->result = 1;
                break;
            }
            case IPC_JPEG_READ_HEADER: {
                j_decompress_ptr cinfo         = (j_decompress_ptr)ctrl->args[0];
                boolean          require_image = (boolean)ctrl->args[1];
                ctrl->result = (uint64_t)jpeg_read_header(cinfo, require_image);
                break;
            }
            case IPC_JPEG_START_DECOMPRESS: {
                j_decompress_ptr cinfo = (j_decompress_ptr)ctrl->args[0];
                ctrl->result = (uint64_t)jpeg_start_decompress(cinfo);
                break;
            }
            case IPC_JPEG_READ_SCANLINES: {
                j_decompress_ptr cinfo     = (j_decompress_ptr)ctrl->args[0];
                JSAMPARRAY       buffer    = (JSAMPARRAY)ctrl->args[1];
                JDIMENSION       max_lines = (JDIMENSION)ctrl->args[2];
                ctrl->result = (uint64_t)jpeg_read_scanlines(cinfo, buffer, max_lines);
                break;
            }
            case IPC_JPEG_FINISH_DECOMPRESS: {
                j_decompress_ptr cinfo = (j_decompress_ptr)ctrl->args[0];
                ctrl->result = (uint64_t)jpeg_finish_decompress(cinfo);
                break;
            }
            case IPC_JPEG_DESTROY_DECOMPRESS: {
                j_decompress_ptr cinfo = (j_decompress_ptr)ctrl->args[0];
                jpeg_destroy_decompress(cinfo);
                ctrl->result = 1;
                break;
            }
            case IPC_JPEG_CREATE_COMPRESS: {
                j_compress_ptr cinfo = (j_compress_ptr)ctrl->args[0];
                jpeg_create_compress(cinfo);
                ctrl->result = 1;
                break;
            }
            case IPC_JPEG_MEM_DEST: {
                j_compress_ptr  cinfo     = (j_compress_ptr)ctrl->args[0];
                unsigned char** outbuffer = (unsigned char**)ctrl->args[1];
                unsigned long*  outsize   = (unsigned long*)ctrl->args[2];
                compress_outbuffer = outbuffer;
                compress_outsize   = outsize;
                local_compress_buf  = nullptr;
                local_compress_size = 0;
                jpeg_mem_dest(cinfo, &local_compress_buf, &local_compress_size);
                ctrl->result = 1;
                break;
            }
            case IPC_JPEG_SET_DEFAULTS: {
                j_compress_ptr cinfo = (j_compress_ptr)ctrl->args[0];
                jpeg_set_defaults(cinfo);
                ctrl->result = 1;
                break;
            }
            case IPC_JPEG_SET_QUALITY: {
                j_compress_ptr cinfo          = (j_compress_ptr)ctrl->args[0];
                int            quality        = (int)ctrl->args[1];
                boolean        force_baseline = (boolean)ctrl->args[2];
                jpeg_set_quality(cinfo, quality, force_baseline);
                ctrl->result = 1;
                break;
            }
            case IPC_JPEG_START_COMPRESS: {
                j_compress_ptr cinfo           = (j_compress_ptr)ctrl->args[0];
                boolean        write_all_tables = (boolean)ctrl->args[1];
                jpeg_start_compress(cinfo, write_all_tables);
                ctrl->result = 1;
                break;
            }
            case IPC_JPEG_WRITE_SCANLINES: {
                j_compress_ptr cinfo     = (j_compress_ptr)ctrl->args[0];
                JSAMPARRAY     scanlines = (JSAMPARRAY)ctrl->args[1];
                JDIMENSION     num_lines = (JDIMENSION)ctrl->args[2];
                ctrl->result = (uint64_t)jpeg_write_scanlines(cinfo, scanlines, num_lines);
                break;
            }
            case IPC_JPEG_FINISH_COMPRESS: {
                j_compress_ptr cinfo = (j_compress_ptr)ctrl->args[0];
                jpeg_finish_compress(cinfo);
                unsigned char* shm_buf = (unsigned char*)mspace_malloc(shared_heap, local_compress_size);
                memcpy(shm_buf, local_compress_buf, local_compress_size);
                free(local_compress_buf);
                local_compress_buf  = nullptr;
                *compress_outbuffer = shm_buf;
                *compress_outsize   = local_compress_size;
                ctrl->result = 1;
                break;
            }
            case IPC_JPEG_DESTROY_COMPRESS: {
                j_compress_ptr cinfo = (j_compress_ptr)ctrl->args[0];
                jpeg_destroy_compress(cinfo);
                ctrl->result = 1;
                break;
            }
            case IPC_TERMINATE: {
#ifdef IPC_INSTRUMENT
                printf("server_calls\t%llu\n",      (unsigned long long)server_call_count);
                printf("server_work_total\t%llu\n", (unsigned long long)server_work_ns);
#endif
                spin_store(&ctrl->status, SPIN_RESPONSE, std::memory_order_release);
                return;
            }
        }

#ifdef IPC_INSTRUMENT
        server_work_ns += now_ns() - ts;
        server_call_count++;
#endif

        spin_store(&ctrl->status, SPIN_RESPONSE, std::memory_order_release);
    }
}

int main() {
    cpu_pin(1);  // dedicated core — client pins to core 0 in impl_create_sandbox

    int   shm_fd;
    void* shm_ptr;
    shared_heap = shared_memory_setup(shm_fd, shm_ptr, true, IPC_CONTROL_SIZE);

    ipc_control* ctrl = (ipc_control*)shm_ptr;
    spin_store(&ctrl->status, SPIN_IDLE, std::memory_order_relaxed);

    server_run(ctrl);

    munmap(shm_ptr, SHARED_MEM_SIZE);
    shm_unlink(SHARED_MEM_NAME);
    close(shm_fd);
    return 0;
}
