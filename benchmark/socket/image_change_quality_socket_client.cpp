#include <chrono>
#include "libjpeg_client.cpp"

#include "../test_bytes.h"

using namespace std::chrono;

#define TEST_ITERATIONS 100

int main() {
    int server_fd = socket_setup_client();
    unsigned long input_size = sizeof(inputData) - 1;
    unsigned long output_size = sizeof(outputData) - 1;

    int shm_fd;
    void* shm_ptr;
    mspace shared_heap = shared_memory_setup(shm_fd, shm_ptr, false);

    ////////// libjpeg calls //////////
    
    struct jpeg_parsed_data in_jpeg_data {0}, out_jpeg_data {0};

    auto enter_time = high_resolution_clock::now();

    for (int i = 0; i < TEST_ITERATIONS; i++) {
        if (i > 0) {
            mspace_free(shared_heap, in_jpeg_data.image_buffer);
            mspace_free(shared_heap, out_jpeg_data.image_buffer);
        }
        in_jpeg_data = read_jpeg(server_fd, shared_heap, inputData, input_size);
        out_jpeg_data = write_jpeg(server_fd, shared_heap, 30, in_jpeg_data);
    }
    
    auto exit_time = high_resolution_clock::now();

    ///////////////////////////////////

    ipc_terminate(server_fd);

    // Validation
    RELEASE_ASSERT(output_size == out_jpeg_data.image_buffer_size, "Size mismatch");
    for(unsigned long i = 0; i < output_size; i++) {
        if (out_jpeg_data.image_buffer[i] != outputData[i]) {
            printf("Output data doesn't match at index: %lu!\n", i);
            exit(1);
        }
    }

    int64_t ns = duration_cast<nanoseconds>(exit_time - enter_time).count();
    printf("JPEG recoding time: %lld\n", (long long) (ns / TEST_ITERATIONS));

    // Cleanup
    close(server_fd);
    close(shm_fd);
    munmap(shm_ptr, SHARED_MEM_SIZE);
    
    return 0;
}
