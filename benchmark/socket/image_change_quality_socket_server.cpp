#include "libjpeg_server.cpp"

int main() {
    int server_fd = socket_setup_server();
    
    int shm_fd;
    void* shm_ptr;
    mspace shared_heap = shared_memory_setup(shm_fd, shm_ptr, true);

    // Accept connection after setting up shared memory region
    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0) {perror("accept");}

    server_run(client_fd);
    std::cout << "Server terminating" << std::endl;

    // Cleanup
    munmap(shm_ptr, SHARED_MEM_SIZE);
    shm_unlink(SHARED_MEM_NAME);
    close(shm_fd);
    close(client_fd);
    close(server_fd);

    return 0;
}
