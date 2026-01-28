#include "libjpeg_client.cpp"

#include "../test_bytes.h"


int main() {
    int server_fd = socket_setup_client();
    unsigned long input_size = sizeof(inputData) - 1;
    unsigned long output_size = sizeof(outputData) - 1;

    int shm_fd;
    void* shm_ptr;
    mspace shared_heap = shared_memory_setup(shm_fd, shm_ptr, false);

    //////////////////////////////

    struct jpeg_parsed_data in_jpeg_data = {0};
    struct jpeg_parsed_data out_jpeg_data = {0};

    in_jpeg_data = read_jpeg(server_fd, shared_heap, inputData, input_size);
    out_jpeg_data = write_jpeg(server_fd, shared_heap, 30, in_jpeg_data);
    
    //////////////////////////////
    
    // Validation
    // RELEASE_ASSERT(output_size == out_jpeg_data.image_buffer_size, "Size mismatch");
    // for(unsigned long i = 0; i < output_size; i++) {
    //     if (out_jpeg_data.image_buffer[i] != outputData[i]) {
    //     printf("Output data doesn't match at index: %lu!\n", i);
    //     exit(1);
    //     }
    // }

    ipc_terminate(server_fd);

    // Cleanup
    close(server_fd);
    close(shm_fd);
    munmap(shm_ptr, SHARED_MEM_SIZE);
    
    return 0;
}
