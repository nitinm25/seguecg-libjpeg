#pragma once
#include <stdint.h>
#include <sched.h>

#ifdef __cplusplus
#  include <atomic>
using spin_atomic_u32 = std::atomic<uint32_t>;
#  define spin_load(addr, order)        (addr)->load(order)
#  define spin_store(addr, val, order)  (addr)->store(val, order)
#else
#  include <stdatomic.h>
typedef _Atomic uint32_t spin_atomic_u32;
#  define spin_load(addr, order)        atomic_load_explicit(addr, order)
#  define spin_store(addr, val, order)  atomic_store_explicit(addr, val, order)
#endif

#define SPIN_IDLE     0
#define SPIN_REQUEST  1
#define SPIN_RESPONSE 2

#define IPC_CONTROL_SIZE 128  // two cache lines; heap starts after this offset

struct alignas(64) ipc_control {
    spin_atomic_u32 status;
    uint32_t        cmd;
    uint64_t        args[8];
    uint64_t        result;
};

static_assert(sizeof(ipc_control) <= IPC_CONTROL_SIZE, "ipc_control exceeds reserved size");

static inline void spin_wait(spin_atomic_u32* addr, uint32_t until) {
    while (spin_load(addr, std::memory_order_acquire) != until)
        __builtin_ia32_pause();
}

