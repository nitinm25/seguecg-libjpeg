#include "common.cpp"
#include "socket.cpp"
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

    ipc_terminate(server_fd);

    //////////////////////////////

    // Cleanup
    close(server_fd);
    close(shm_fd);
    munmap(shm_ptr, SHARED_MEM_SIZE);
    
    return 0;
}

// Shared memory test
// char* shared_data = (char*)mspace_malloc(shared_heap, 256);
// strcpy(shared_data, "Client allocated this!");
// std::cout << "[Client] Buffer content: " << shared_data << std::endl;
// socket_send(server_fd, (unsigned char*)&shared_data, sizeof(shared_data));
// char response[256];
// socket_recv(server_fd, (unsigned char*)response, sizeof(response));
// std::cout << "[Client] Buffer content: " << shared_data << std::endl;
// mspace_free(shared_heap, shared_data);
