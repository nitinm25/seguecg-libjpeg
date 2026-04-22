#pragma once
#include <stdint.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <unistd.h>

#ifdef __cplusplus
#  include <atomic>
using futex_atomic_u32 = std::atomic<uint32_t>;
#  define futex_load(addr, order)        (addr)->load(order)
#  define futex_store(addr, val, order)  (addr)->store(val, order)
#else
#  include <stdatomic.h>
typedef _Atomic uint32_t futex_atomic_u32;
#  define futex_load(addr, order)        atomic_load_explicit(addr, order)
#  define futex_store(addr, val, order)  atomic_store_explicit(addr, val, order)
#endif

#define FUTEX_IDLE     0
#define FUTEX_REQUEST  1
#define FUTEX_RESPONSE 2

#define IPC_CONTROL_SIZE 128  // two cache lines; heap starts after this offset

struct alignas(64) ipc_control {
    futex_atomic_u32 status;
    uint32_t         cmd;
    uint64_t         args[8];
    uint64_t         result;
};

static_assert(sizeof(ipc_control) <= IPC_CONTROL_SIZE, "ipc_control exceeds reserved size");

static inline void futex_wait(futex_atomic_u32* addr, uint32_t val) {
    syscall(SYS_futex, addr, FUTEX_WAIT, val, NULL, NULL, 0);
}

static inline void futex_wake(futex_atomic_u32* addr) {
    syscall(SYS_futex, addr, FUTEX_WAKE, 1, NULL, NULL, 0);
}
