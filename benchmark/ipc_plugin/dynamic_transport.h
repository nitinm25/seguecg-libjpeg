#pragma once
#include <stdint.h>
#include <sched.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <unistd.h>

#ifdef __cplusplus
#  include <atomic>
using dyn_atomic_u32 = std::atomic<uint32_t>;
#  define dyn_load(addr, order)        (addr)->load(order)
#  define dyn_store(addr, val, order)  (addr)->store(val, order)
#else
#  include <stdatomic.h>
typedef _Atomic uint32_t dyn_atomic_u32;
#  define dyn_load(addr, order)        atomic_load_explicit(addr, order)
#  define dyn_store(addr, val, order)  atomic_store_explicit(addr, val, order)
#endif

#define DYN_IDLE     0
#define DYN_REQUEST  1
#define DYN_RESPONSE 2

#define IPC_CONTROL_SIZE 128

struct alignas(64) ipc_control {
    dyn_atomic_u32 status;
    uint32_t       cmd;
    uint32_t       server_cpu;
    uint32_t       client_cpu;
    uint64_t       args[8];
    uint64_t       result;
    // 4+4+4+4+64+8 = 88 bytes
};

static_assert(sizeof(ipc_control) <= IPC_CONTROL_SIZE, "ipc_control exceeds reserved size");

static inline void dyn_futex_wait(dyn_atomic_u32* addr, uint32_t val) {
    syscall(SYS_futex, addr, FUTEX_WAIT, val, NULL, NULL, 0);
}

static inline void dyn_futex_wake(dyn_atomic_u32* addr) {
    syscall(SYS_futex, addr, FUTEX_WAKE, 1, NULL, NULL, 0);
}

static inline void dyn_spin_pause() {
    __builtin_ia32_pause();
}
