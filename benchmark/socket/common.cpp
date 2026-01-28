#include <iostream>
#include <cstdint>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define SHARED_MEM_NAME "/libjpeg_shm"
#define SHARED_MEM_BASE 0x700000000000UL
#define SHARED_MEM_SIZE (50 * 1024 * 1024)  // 50MB

#define USE_DL_PREFIX 1  // Prefix functions with dl_
#define MSPACES 1        // Enable mspace API
#include "dlmalloc.c"


enum IPCCommand : uint32_t {
    IPC_JPEG_STD_ERROR = 0,
    IPC_JPEG_CREATE_DECOMPRESS = 1,
    IPC_JPEG_MEM_SRC = 2,
    IPC_JPEG_READ_HEADER = 3,
    IPC_JPEG_START_DECOMPRESS = 4,
    IPC_JPEG_READ_SCANLINES = 5,
    IPC_JPEG_FINISH_DECOMPRESS = 6,
    IPC_JPEG_DESTROY_DECOMPRESS = 7,
    IPC_JPEG_CREATE_COMPRESS = 8,
    IPC_JPEG_MEM_DEST = 9,
    IPC_JPEG_SET_DEFAULTS = 10,
    IPC_JPEG_SET_QUALITY = 11,
    IPC_JPEG_START_COMPRESS = 12,
    IPC_JPEG_WRITE_SCANLINES = 13,
    IPC_JPEG_FINISH_COMPRESS = 14,
    IPC_JPEG_DESTROY_COMPRESS = 15,

    IPC_TERMINATE = 16
};


mspace shared_memory_setup(int &shm_fd, void* &shm_ptr, bool create_shm) {
    int flags = create_shm ? (O_CREAT | O_RDWR) : O_RDWR;
    shm_fd = shm_open(SHARED_MEM_NAME, flags, 0666);
    if (shm_fd < 0) {
        perror("shm_open");
        exit(1);
    }

    if (create_shm) {
        if (ftruncate(shm_fd, SHARED_MEM_SIZE) < 0) {
            perror("ftruncate");
            exit(1);
        }
    }

    shm_ptr = mmap((void*)SHARED_MEM_BASE, SHARED_MEM_SIZE, 
                   PROT_READ | PROT_WRITE, 
                   MAP_SHARED | MAP_FIXED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED || shm_ptr != (void*)SHARED_MEM_BASE) {
        perror("mmap");
        exit(1);
    }

    mspace shared_heap = create_mspace_with_base(shm_ptr, SHARED_MEM_SIZE, 0);
    if (shared_heap == 0) {
        fprintf(stderr, "Failed to create mspace\n");
        exit(1);
    }

    return shared_heap;
}
