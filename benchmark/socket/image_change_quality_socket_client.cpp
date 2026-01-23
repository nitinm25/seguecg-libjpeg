#include <iostream>

#include "common.cpp"
#include "socket.cpp"
#include "libjpeg_client.cpp"

int main() {
    int server_fd = socket_setup_client();

    int shm_fd;
    void* shm_ptr;
    mspace shared_heap = shared_memory_setup(shm_fd, shm_ptr, false);

    // Shared memory test
    char* shared_data = (char*)mspace_malloc(shared_heap, 256);
    strcpy(shared_data, "Client allocated this!");
    std::cout << "[Client] Buffer content: " << shared_data << std::endl;
    socket_send(server_fd, (unsigned char*)&shared_data, sizeof(shared_data));
    char response[256];
    socket_recv(server_fd, (unsigned char*)response, sizeof(response));
    std::cout << "[Client] Buffer content: " << shared_data << std::endl;
    mspace_free(shared_heap, shared_data);
    
    // Cleanup
    close(server_fd);
    close(shm_fd);
    munmap(shm_ptr, SHARED_MEM_SIZE);

    return 0;
}
