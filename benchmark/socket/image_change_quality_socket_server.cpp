#include <iostream>

#include "common.cpp"
#include "socket.cpp"


int main() {
    int server_fd = socket_setup_server();
    
    int shm_fd;
    void* shm_ptr;
    mspace shared_heap = shared_memory_setup(shm_fd, shm_ptr, true);

    // Accept connection after setting up shared memory region
    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0) {perror("accept");}
    
    // Shared memory test
    char* client_buffer;
    socket_recv(client_fd, (unsigned char*)&client_buffer, sizeof(client_buffer));
    strcat(client_buffer, " - modified by server");
    socket_send(client_fd, (const unsigned char*)"Modified!", 10);
    
    // Cleanup
    munmap(shm_ptr, SHARED_MEM_SIZE);
    shm_unlink(SHARED_MEM_NAME);
    close(shm_fd);
    close(client_fd);
    close(server_fd);

    return 0;
}
