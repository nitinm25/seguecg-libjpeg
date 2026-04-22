#pragma once

#include <cstdint>
#include <cstdlib>
#include <mutex>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>

#include "common.cpp"

#ifdef IPC_TRANSPORT_FUTEX
#  include "futex_transport.h"
#  define RLBOX_IPC_CLASS        rlbox_ipc_futex_sandbox
#  define RLBOX_IPC_THREAD_DATA  rlbox_ipc_futex_sandbox_thread_data
#  define RLBOX_IPC_GET_TD       get_rlbox_ipc_futex_sandbox_thread_data
#  define RLBOX_IPC_STATIC_VARS  RLBOX_IPC_FUTEX_SANDBOX_STATIC_VARIABLES
#  define RLBOX_IPC_LOOKUP       rlbox_ipc_futex_sandbox_lookup_symbol
#  define RLBOX_IPC_SERVER_PATH  "../build_nosimd_release/image_change_quality_ipc_futex_server"
#else
#  include "socket.cpp"
#  define RLBOX_IPC_CLASS        rlbox_ipc_socket_sandbox
#  define RLBOX_IPC_THREAD_DATA  rlbox_ipc_socket_sandbox_thread_data
#  define RLBOX_IPC_GET_TD       get_rlbox_ipc_socket_sandbox_thread_data
#  define RLBOX_IPC_STATIC_VARS  RLBOX_IPC_SOCKET_SANDBOX_STATIC_VARIABLES
#  define RLBOX_IPC_LOOKUP       rlbox_ipc_socket_sandbox_lookup_symbol
#  define RLBOX_IPC_SERVER_PATH  "../build_nosimd_release/image_change_quality_ipc_socket_server"
#endif

#ifndef RLBOX_USE_CUSTOM_SHARED_LOCK
#  include <shared_mutex>
#endif
#include <utility>

#include "rlbox_helpers.hpp"

#ifndef IPC_SERVER_PATH
#  define IPC_SERVER_PATH RLBOX_IPC_SERVER_PATH
#endif

namespace rlbox {

class RLBOX_IPC_CLASS;

struct RLBOX_IPC_THREAD_DATA
{
  RLBOX_IPC_CLASS* sandbox;
  uint32_t last_callback_invoked;
};

#ifdef RLBOX_EMBEDDER_PROVIDES_TLS_STATIC_VARIABLES

RLBOX_IPC_THREAD_DATA* RLBOX_IPC_GET_TD();
#  define RLBOX_IPC_STATIC_VARS()                                                   \
    thread_local rlbox::RLBOX_IPC_THREAD_DATA                                       \
      RLBOX_IPC_CLASS##_thread_info{ 0, 0 };                                        \
    namespace rlbox {                                                           \
      RLBOX_IPC_THREAD_DATA* RLBOX_IPC_GET_TD()                                          \
      {                                                                        \
        return &RLBOX_IPC_CLASS##_thread_info;                                      \
      }                                                                        \
    }                                                                          \
    static_assert(true, "Enforce semi-colon")

#endif

class RLBOX_IPC_CLASS
{
public:
  using T_LongLongType = long long;
  using T_LongType = long;
  using T_IntType = int;
  using T_PointerType = void*;
  using T_ShortType = short;
  using can_grant_deny_access = void;

private:
  RLBOX_SHARED_LOCK(callback_mutex);
  static inline const uint32_t MAX_CALLBACKS = 64;
  void* callback_unique_keys[MAX_CALLBACKS]{ 0 };
  void* callbacks[MAX_CALLBACKS]{ 0 };

#ifdef IPC_TRANSPORT_FUTEX
  ipc_control* ctrl = nullptr;
#else
  int server_fd = -1;
#endif
  int shm_fd = -1;
  void* shm_ptr = nullptr;
  mspace shared_heap = nullptr;
  pid_t server_pid = -1;

#ifdef IPC_INSTRUMENT
public:
  static inline ipc_timing timing{};
private:
#endif

#ifndef RLBOX_EMBEDDER_PROVIDES_TLS_STATIC_VARIABLES
  thread_local static inline RLBOX_IPC_THREAD_DATA thread_data{ 0, 0 };
#endif

  template<uint32_t N, typename T_Ret, typename... T_Args>
  static T_Ret callback_trampoline(T_Args... params)
  {
#ifdef RLBOX_EMBEDDER_PROVIDES_TLS_STATIC_VARIABLES
    auto& thread_data = *RLBOX_IPC_GET_TD();
#endif
    thread_data.last_callback_invoked = N;
    using T_Func = T_Ret (*)(T_Args...);
    T_Func func;
    {
#ifndef RLBOX_SINGLE_THREADED_INVOCATIONS
      RLBOX_ACQUIRE_SHARED_GUARD(lock, thread_data.sandbox->callback_mutex);
#endif
      func = reinterpret_cast<T_Func>(thread_data.sandbox->callbacks[N]);
    }
    return func(params...);
  }

#ifdef IPC_TRANSPORT_FUTEX
  template<typename... Args>
  void do_ipc_void(uint32_t cmd, const Args&... args) {
#ifdef IPC_INSTRUMENT
    uint64_t t0 = now_ns();
#endif
    uint8_t n = 0;
    ([&]{ uint64_t v = 0; memcpy(&v, &args, sizeof(args)); ctrl->args[n++] = v; }(), ...);
    ctrl->cmd = cmd;
    futex_store(&ctrl->status, FUTEX_REQUEST, std::memory_order_release);
    futex_wake(&ctrl->status);
#ifdef IPC_INSTRUMENT
    uint64_t t1 = now_ns();
#endif
    while (futex_load(&ctrl->status, std::memory_order_acquire) != FUTEX_RESPONSE)
        futex_wait(&ctrl->status, FUTEX_REQUEST);
#ifdef IPC_INSTRUMENT
    uint64_t t2 = now_ns();
    timing.send_ns += t1 - t0;
    timing.wait_ns += t2 - t1;
    timing.call_count++;
#endif
    futex_store(&ctrl->status, FUTEX_IDLE, std::memory_order_relaxed);
  }

  template<typename RetT, typename... Args>
  RetT do_ipc_ret(uint32_t cmd, const Args&... args) {
#ifdef IPC_INSTRUMENT
    uint64_t t0 = now_ns();
#endif
    uint8_t n = 0;
    ([&]{ uint64_t v = 0; memcpy(&v, &args, sizeof(args)); ctrl->args[n++] = v; }(), ...);
    ctrl->cmd = cmd;
    futex_store(&ctrl->status, FUTEX_REQUEST, std::memory_order_release);
    futex_wake(&ctrl->status);
#ifdef IPC_INSTRUMENT
    uint64_t t1 = now_ns();
#endif
    while (futex_load(&ctrl->status, std::memory_order_acquire) != FUTEX_RESPONSE)
        futex_wait(&ctrl->status, FUTEX_REQUEST);
#ifdef IPC_INSTRUMENT
    uint64_t t2 = now_ns();
    timing.send_ns += t1 - t0;
    timing.wait_ns += t2 - t1;
    timing.call_count++;
#endif
    RetT result;
    uint64_t raw = ctrl->result;
    memcpy(&result, &raw, sizeof(result));
    futex_store(&ctrl->status, FUTEX_IDLE, std::memory_order_relaxed);
    return result;
  }

#else // socket transport

  template<typename... Args>
  void do_ipc_void(uint32_t cmd, const Args&... args) {
#ifdef IPC_INSTRUMENT
    uint64_t t0 = now_ns();
#endif
    socket_send(server_fd, (unsigned char*)&cmd, sizeof(cmd));
    (socket_send(server_fd, (unsigned char*)&args, sizeof(args)), ...);
#ifdef IPC_INSTRUMENT
    uint64_t t1 = now_ns();
#endif
    uint32_t ack;
    socket_recv(server_fd, (unsigned char*)&ack, sizeof(ack));
#ifdef IPC_INSTRUMENT
    uint64_t t2 = now_ns();
    timing.send_ns += t1 - t0;
    timing.wait_ns += t2 - t1;
    timing.call_count++;
#endif
  }

  template<typename RetT, typename... Args>
  RetT do_ipc_ret(uint32_t cmd, const Args&... args) {
#ifdef IPC_INSTRUMENT
    uint64_t t0 = now_ns();
#endif
    socket_send(server_fd, (unsigned char*)&cmd, sizeof(cmd));
    (socket_send(server_fd, (unsigned char*)&args, sizeof(args)), ...);
#ifdef IPC_INSTRUMENT
    uint64_t t1 = now_ns();
#endif
    RetT result;
    socket_recv(server_fd, (unsigned char*)&result, sizeof(result));
#ifdef IPC_INSTRUMENT
    uint64_t t2 = now_ns();
    timing.send_ns += t1 - t0;
    timing.wait_ns += t2 - t1;
    timing.call_count++;
#endif
    return result;
  }

#endif // transport selection

public:
  static RLBOX_IPC_CLASS* get_current_sandbox()
  {
#ifdef RLBOX_EMBEDDER_PROVIDES_TLS_STATIC_VARIABLES
    auto& thread_data = *RLBOX_IPC_GET_TD();
#endif
    return thread_data.sandbox;
  }

  // --- libjpeg wrappers (identical for both transports) ---

  static jpeg_error_mgr* ipc_jpeg_std_error(jpeg_error_mgr* err) {
    return get_current_sandbox()->do_ipc_ret<jpeg_error_mgr*>(IPC_JPEG_STD_ERROR, err);
  }

  static void ipc_jpeg_CreateDecompress(j_decompress_ptr cinfo, int, size_t) {
    get_current_sandbox()->do_ipc_void(IPC_JPEG_CREATE_DECOMPRESS, cinfo);
  }

  static void ipc_jpeg_mem_src(j_decompress_ptr cinfo, const unsigned char* buffer, unsigned long size) {
    get_current_sandbox()->do_ipc_void(IPC_JPEG_MEM_SRC, cinfo, const_cast<unsigned char*>(buffer), size);
  }

  static int ipc_jpeg_read_header(j_decompress_ptr cinfo, boolean require_image) {
    return get_current_sandbox()->do_ipc_ret<int>(IPC_JPEG_READ_HEADER, cinfo, require_image);
  }

  static boolean ipc_jpeg_start_decompress(j_decompress_ptr cinfo) {
    return get_current_sandbox()->do_ipc_ret<boolean>(IPC_JPEG_START_DECOMPRESS, cinfo);
  }

  static JDIMENSION ipc_jpeg_read_scanlines(j_decompress_ptr cinfo, JSAMPARRAY buffer, JDIMENSION max_lines) {
    return get_current_sandbox()->do_ipc_ret<JDIMENSION>(IPC_JPEG_READ_SCANLINES, cinfo, buffer, max_lines);
  }

  static boolean ipc_jpeg_finish_decompress(j_decompress_ptr cinfo) {
    return get_current_sandbox()->do_ipc_ret<boolean>(IPC_JPEG_FINISH_DECOMPRESS, cinfo);
  }

  static void ipc_jpeg_destroy_decompress(j_decompress_ptr cinfo) {
    get_current_sandbox()->do_ipc_void(IPC_JPEG_DESTROY_DECOMPRESS, cinfo);
  }

  static void ipc_jpeg_CreateCompress(j_compress_ptr cinfo, int, size_t) {
    get_current_sandbox()->do_ipc_void(IPC_JPEG_CREATE_COMPRESS, cinfo);
  }

  static void ipc_jpeg_mem_dest(j_compress_ptr cinfo, unsigned char** outbuffer, unsigned long* outsize) {
    get_current_sandbox()->do_ipc_void(IPC_JPEG_MEM_DEST, cinfo, outbuffer, outsize);
  }

  static void ipc_jpeg_set_defaults(j_compress_ptr cinfo) {
    get_current_sandbox()->do_ipc_void(IPC_JPEG_SET_DEFAULTS, cinfo);
  }

  static void ipc_jpeg_set_quality(j_compress_ptr cinfo, int quality, boolean force_baseline) {
    get_current_sandbox()->do_ipc_void(IPC_JPEG_SET_QUALITY, cinfo, quality, force_baseline);
  }

  static void ipc_jpeg_start_compress(j_compress_ptr cinfo, boolean write_all_tables) {
    get_current_sandbox()->do_ipc_void(IPC_JPEG_START_COMPRESS, cinfo, write_all_tables);
  }

  static JDIMENSION ipc_jpeg_write_scanlines(j_compress_ptr cinfo, JSAMPARRAY scanlines, JDIMENSION num_lines) {
    return get_current_sandbox()->do_ipc_ret<JDIMENSION>(IPC_JPEG_WRITE_SCANLINES, cinfo, scanlines, num_lines);
  }

  static void ipc_jpeg_finish_compress(j_compress_ptr cinfo) {
    get_current_sandbox()->do_ipc_void(IPC_JPEG_FINISH_COMPRESS, cinfo);
  }

  static void ipc_jpeg_destroy_compress(j_compress_ptr cinfo) {
    get_current_sandbox()->do_ipc_void(IPC_JPEG_DESTROY_COMPRESS, cinfo);
  }

protected:
  inline void impl_create_sandbox()
  {
    server_pid = fork();
    if (server_pid < 0) { perror("fork"); abort(); }
    if (server_pid == 0) {
      execl(IPC_SERVER_PATH, IPC_SERVER_PATH, nullptr);
      perror("execl");
      _exit(1);
    }

    usleep(100000); // 100ms for server startup

#ifdef IPC_TRANSPORT_FUTEX
    shared_heap = shared_memory_setup(shm_fd, shm_ptr, false, IPC_CONTROL_SIZE);
    ctrl = (ipc_control*)shm_ptr;
#else
    shared_heap = shared_memory_setup(shm_fd, shm_ptr, false);
    server_fd = socket_setup_client();
#endif
  }

  inline void impl_destroy_sandbox()
  {
#ifdef IPC_TRANSPORT_FUTEX
    uint8_t n = 0;
    ctrl->cmd = IPC_TERMINATE;
    futex_store(&ctrl->status, FUTEX_REQUEST, std::memory_order_release);
    futex_wake(&ctrl->status);
    while (futex_load(&ctrl->status, std::memory_order_acquire) != FUTEX_RESPONSE)
        futex_wait(&ctrl->status, FUTEX_REQUEST);
#else
    uint32_t cmd = IPC_TERMINATE;
    socket_send(server_fd, (unsigned char*)&cmd, sizeof(cmd));
    close(server_fd);
    server_fd = -1;
#endif

    if (server_pid > 0) { waitpid(server_pid, nullptr, 0); server_pid = -1; }

    if (shm_ptr) { munmap(shm_ptr, SHARED_MEM_SIZE); shm_ptr = nullptr; }
    if (shm_fd >= 0) { close(shm_fd); shm_fd = -1; }
    shared_heap = nullptr;
  }

  template<typename T>
  inline void* impl_get_unsandboxed_pointer(T_PointerType p) const { return p; }

  template<typename T>
  inline T_PointerType impl_get_sandboxed_pointer(const void* p) const
  { return const_cast<T_PointerType>(p); }

  template<typename T>
  static inline void* impl_get_unsandboxed_pointer_no_ctx(
    T_PointerType p,
    const void*,
    RLBOX_IPC_CLASS* (*)(const void*))
  { return p; }

  template<typename T>
  static inline T_PointerType impl_get_sandboxed_pointer_no_ctx(
    const void* p,
    const void*,
    RLBOX_IPC_CLASS* (*)(const void*))
  { return const_cast<T_PointerType>(p); }

  inline T_PointerType impl_malloc_in_sandbox(size_t size)
  { return mspace_malloc(shared_heap, size); }

  inline void impl_free_in_sandbox(T_PointerType p) { mspace_free(shared_heap, p); }

  static inline bool impl_is_in_same_sandbox(const void*, const void*) { return true; }

  inline bool impl_is_pointer_in_sandbox_memory(const void* p)
  {
    uintptr_t addr = reinterpret_cast<uintptr_t>(p);
    uintptr_t base = reinterpret_cast<uintptr_t>(shm_ptr);
    return addr >= base && addr < base + SHARED_MEM_SIZE;
  }

  inline bool impl_is_pointer_in_app_memory(const void* p)
  { return !impl_is_pointer_in_sandbox_memory(p); }

  inline size_t impl_get_total_memory() { return SHARED_MEM_SIZE; }
  inline void* impl_get_memory_location() { return shm_ptr; }

  template<typename T = void>
  void* impl_lookup_symbol(const char*)
  {
    constexpr bool fail = std::is_same_v<T, void>;
    rlbox_detail_static_fail_because(
      fail,
      "The no_op_sandbox uses static calls and thus developers should add\n\n"
      "#define RLBOX_USE_STATIC_CALLS() rlbox_noop_sandbox_lookup_symbol\n\n"
      "to their code, to ensure that static calls are handled correctly.");
    return nullptr;
  }

#define rlbox_ipc_socket_sandbox_lookup_symbol(func_name) \
  reinterpret_cast<void*>(&rlbox::rlbox_ipc_socket_sandbox::ipc_##func_name)

#define rlbox_ipc_futex_sandbox_lookup_symbol(func_name) \
  reinterpret_cast<void*>(&rlbox::rlbox_ipc_futex_sandbox::ipc_##func_name)

  template<typename T, typename T_Converted, typename... T_Args>
  auto impl_invoke_with_func_ptr(T_Converted* func_ptr, T_Args&&... params)
  {
#ifdef RLBOX_EMBEDDER_PROVIDES_TLS_STATIC_VARIABLES
    auto& thread_data = *RLBOX_IPC_GET_TD();
#endif
    auto old_sandbox = thread_data.sandbox;
    thread_data.sandbox = this;
    auto on_exit = detail::make_scope_exit([&] { thread_data.sandbox = old_sandbox; });
    return (*func_ptr)(params...);
  }

  template<typename T_Ret, typename... T_Args>
  inline T_PointerType impl_register_callback(void* key, void* callback)
  {
    RLBOX_ACQUIRE_UNIQUE_GUARD(lock, callback_mutex);
    void* chosen_trampoline = nullptr;
    detail::compile_time_for<MAX_CALLBACKS>([&](auto I) {
      if (!chosen_trampoline && callback_unique_keys[I.value] == nullptr) {
        callback_unique_keys[I.value] = key;
        callbacks[I.value] = callback;
        chosen_trampoline = reinterpret_cast<void*>(
          callback_trampoline<I.value, T_Ret, T_Args...>);
      }
    });
    return reinterpret_cast<T_PointerType>(chosen_trampoline);
  }

  static inline std::pair<RLBOX_IPC_CLASS*, void*>
  impl_get_executed_callback_sandbox_and_key()
  {
#ifdef RLBOX_EMBEDDER_PROVIDES_TLS_STATIC_VARIABLES
    auto& thread_data = *RLBOX_IPC_GET_TD();
#endif
    auto sandbox = thread_data.sandbox;
    auto callback_num = thread_data.last_callback_invoked;
    void* key = sandbox->callback_unique_keys[callback_num];
    return std::make_pair(sandbox, key);
  }

  template<typename T_Ret, typename... T_Args>
  inline void impl_unregister_callback(void* key)
  {
    RLBOX_ACQUIRE_UNIQUE_GUARD(lock, callback_mutex);
    for (uint32_t i = 0; i < MAX_CALLBACKS; i++) {
      if (callback_unique_keys[i] == key) {
        callback_unique_keys[i] = nullptr;
        callbacks[i] = nullptr;
        break;
      }
    }
  }

  template<typename T>
  inline T* impl_grant_access(T* src, size_t num, bool& success)
  { RLBOX_UNUSED(num); success = true; return src; }

  template<typename T>
  inline T* impl_deny_access(T* src, size_t num, bool& success)
  { RLBOX_UNUSED(num); success = true; return src; }
};

} // namespace rlbox
