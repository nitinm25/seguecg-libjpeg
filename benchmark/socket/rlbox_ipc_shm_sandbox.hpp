#pragma once

#include <cstdint>
#include <cstdlib>
#include <mutex>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>

#include "common.cpp"
#include "socket.cpp"

#ifndef RLBOX_USE_CUSTOM_SHARED_LOCK
#  include <shared_mutex>
#endif
#include <utility>

#include "rlbox_helpers.hpp"

#ifndef IPC_SERVER_PATH
#  define IPC_SERVER_PATH "../build_nosimd_release/image_change_quality_socket_server"
#endif

namespace rlbox {

class rlbox_ipc_shm_sandbox;

struct rlbox_ipc_shm_sandbox_thread_data
{
  rlbox_ipc_shm_sandbox* sandbox;
  uint32_t last_callback_invoked;
};

#ifdef RLBOX_EMBEDDER_PROVIDES_TLS_STATIC_VARIABLES

rlbox_ipc_shm_sandbox_thread_data* get_rlbox_ipc_shm_sandbox_thread_data();
#  define RLBOX_IPC_SHM_SANDBOX_STATIC_VARIABLES()                             \
    thread_local rlbox::rlbox_ipc_shm_sandbox_thread_data                      \
      rlbox_ipc_shm_sandbox_thread_info{ 0, 0 };                               \
    namespace rlbox {                                                          \
      rlbox_ipc_shm_sandbox_thread_data* get_rlbox_ipc_shm_sandbox_thread_data() \
      {                                                                        \
        return &rlbox_ipc_shm_sandbox_thread_info;                             \
      }                                                                        \
    }                                                                          \
    static_assert(true, "Enforce semi-colon")

#endif

/**
 * @brief Class that implements the IPC shared memory sandbox.
 */
class rlbox_ipc_shm_sandbox
{
public:
  // Stick with the system defaults
  using T_LongLongType = long long;
  using T_LongType = long;
  using T_IntType = int;
  using T_PointerType = void*;
  using T_ShortType = short;
  // no-op sandbox can transfer buffers as there is no sandboxings
  // Thus transfer is a noop
  using can_grant_deny_access = void;

private:
  RLBOX_SHARED_LOCK(callback_mutex);
  static inline const uint32_t MAX_CALLBACKS = 64;
  void* callback_unique_keys[MAX_CALLBACKS]{ 0 };
  void* callbacks[MAX_CALLBACKS]{ 0 };

  int server_fd = -1;
  int shm_fd = -1;
  void* shm_ptr = nullptr;
  mspace shared_heap = nullptr;
  pid_t server_pid = -1;

#ifndef RLBOX_EMBEDDER_PROVIDES_TLS_STATIC_VARIABLES
  thread_local static inline rlbox_ipc_shm_sandbox_thread_data thread_data{ 0, 0 };
#endif

  template<uint32_t N, typename T_Ret, typename... T_Args>
  static T_Ret callback_trampoline(T_Args... params)
  {
#ifdef RLBOX_EMBEDDER_PROVIDES_TLS_STATIC_VARIABLES
    auto& thread_data = *get_rlbox_ipc_shm_sandbox_thread_data();
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

  template<typename... Args>
  static void ipc_call_void(int fd, uint32_t cmd, const Args&... args) {
    socket_send(fd, (unsigned char*)&cmd, sizeof(cmd));
    (socket_send(fd, (unsigned char*)&args, sizeof(args)), ...);
    uint32_t ack;
    socket_recv(fd, (unsigned char*)&ack, sizeof(ack));
  }

  template<typename RetT, typename... Args>
  static RetT ipc_call_ret(int fd, uint32_t cmd, const Args&... args) {
    socket_send(fd, (unsigned char*)&cmd, sizeof(cmd));
    (socket_send(fd, (unsigned char*)&args, sizeof(args)), ...);
    RetT result;
    socket_recv(fd, (unsigned char*)&result, sizeof(result));
    return result;
  }

public:
  int get_server_fd() const { return server_fd; }

  static rlbox_ipc_shm_sandbox* get_current_sandbox()
  {
#ifdef RLBOX_EMBEDDER_PROVIDES_TLS_STATIC_VARIABLES
    auto& thread_data = *get_rlbox_ipc_shm_sandbox_thread_data();
#endif
    return thread_data.sandbox;
  }

  static jpeg_error_mgr* ipc_jpeg_std_error(jpeg_error_mgr* err) {
    return ipc_call_ret<jpeg_error_mgr*>(get_current_sandbox()->server_fd, IPC_JPEG_STD_ERROR, err);
  }

  static void ipc_jpeg_CreateDecompress(j_decompress_ptr cinfo, int, size_t) {
    ipc_call_void(get_current_sandbox()->server_fd, IPC_JPEG_CREATE_DECOMPRESS, cinfo);
  }

  static void ipc_jpeg_mem_src(j_decompress_ptr cinfo, const unsigned char* buffer, unsigned long size) {
    ipc_call_void(get_current_sandbox()->server_fd, IPC_JPEG_MEM_SRC, cinfo, const_cast<unsigned char*>(buffer), size);
  }

  static int ipc_jpeg_read_header(j_decompress_ptr cinfo, boolean require_image) {
    ipc_call_void(get_current_sandbox()->server_fd, IPC_JPEG_READ_HEADER, cinfo, require_image);
    return 0;
  }

  static boolean ipc_jpeg_start_decompress(j_decompress_ptr cinfo) {
    ipc_call_void(get_current_sandbox()->server_fd, IPC_JPEG_START_DECOMPRESS, cinfo);
    return TRUE;
  }

  static JDIMENSION ipc_jpeg_read_scanlines(j_decompress_ptr cinfo, JSAMPARRAY buffer, JDIMENSION max_lines) {
    ipc_call_void(get_current_sandbox()->server_fd, IPC_JPEG_READ_SCANLINES, cinfo, buffer, max_lines);
    return 0;
  }

  static boolean ipc_jpeg_finish_decompress(j_decompress_ptr cinfo) {
    ipc_call_void(get_current_sandbox()->server_fd, IPC_JPEG_FINISH_DECOMPRESS, cinfo);
    return TRUE;
  }

  static void ipc_jpeg_destroy_decompress(j_decompress_ptr cinfo) {
    ipc_call_void(get_current_sandbox()->server_fd, IPC_JPEG_DESTROY_DECOMPRESS, cinfo);
  }

  static void ipc_jpeg_CreateCompress(j_compress_ptr cinfo, int, size_t) {
    ipc_call_void(get_current_sandbox()->server_fd, IPC_JPEG_CREATE_COMPRESS, cinfo);
  }

  static void ipc_jpeg_mem_dest(j_compress_ptr cinfo, unsigned char** outbuffer, unsigned long* outsize) {
    static const unsigned long OUTPUT_BUF_SIZE = 126705;
    auto* sbx = get_current_sandbox();
    if (*outbuffer == nullptr || *outsize == 0) {
      *outbuffer = (unsigned char*)mspace_malloc(sbx->shared_heap, OUTPUT_BUF_SIZE);
      *outsize = OUTPUT_BUF_SIZE;
    }
    ipc_call_void(sbx->server_fd, IPC_JPEG_MEM_DEST, cinfo, outbuffer, outsize);
  }

  static void ipc_jpeg_set_defaults(j_compress_ptr cinfo) {
    ipc_call_void(get_current_sandbox()->server_fd, IPC_JPEG_SET_DEFAULTS, cinfo);
  }

  static void ipc_jpeg_set_quality(j_compress_ptr cinfo, int quality, boolean force_baseline) {
    ipc_call_void(get_current_sandbox()->server_fd, IPC_JPEG_SET_QUALITY, cinfo, quality, force_baseline);
  }

  static void ipc_jpeg_start_compress(j_compress_ptr cinfo, boolean write_all_tables) {
    ipc_call_void(get_current_sandbox()->server_fd, IPC_JPEG_START_COMPRESS, cinfo, write_all_tables);
  }

  static JDIMENSION ipc_jpeg_write_scanlines(j_compress_ptr cinfo, JSAMPARRAY scanlines, JDIMENSION num_lines) {
    return ipc_call_ret<JDIMENSION>(get_current_sandbox()->server_fd, IPC_JPEG_WRITE_SCANLINES, cinfo, scanlines, num_lines);
  }

  static void ipc_jpeg_finish_compress(j_compress_ptr cinfo) {
    ipc_call_void(get_current_sandbox()->server_fd, IPC_JPEG_FINISH_COMPRESS, cinfo);
  }

  static void ipc_jpeg_destroy_compress(j_compress_ptr cinfo) {
    ipc_call_void(get_current_sandbox()->server_fd, IPC_JPEG_DESTROY_COMPRESS, cinfo);
  }

protected:
  inline void impl_create_sandbox()
  {
    // Fork the server process
    server_pid = fork();
    if (server_pid < 0) {
      perror("fork");
      abort();
    }
    if (server_pid == 0) {
      // Child: exec the server binary
      execl(IPC_SERVER_PATH,
            "image_change_quality_socket_server", nullptr);
      perror("execl");
      _exit(1);
    }

    // Parent: give server a moment to start listening
    usleep(100000); // 100ms

    // Set up shared memory (as non-creator; server creates it)
    shared_heap = shared_memory_setup(shm_fd, shm_ptr, false);

    // Connect to server via Unix domain socket
    server_fd = socket_setup_client();
  }

  inline void impl_destroy_sandbox()
  {
    // Tell server to terminate
    uint32_t cmd = IPC_TERMINATE;
    socket_send(server_fd, (unsigned char*)&cmd, sizeof(cmd));

    close(server_fd);
    server_fd = -1;

    // Wait for server process
    if (server_pid > 0) {
      waitpid(server_pid, nullptr, 0);
      server_pid = -1;
    }

    // Clean up shared memory
    if (shm_ptr) {
      munmap(shm_ptr, SHARED_MEM_SIZE);
      shm_ptr = nullptr;
    }
    if (shm_fd >= 0) {
      close(shm_fd);
      shm_fd = -1;
    }
    shared_heap = nullptr;
  }

  template<typename T>
  inline void* impl_get_unsandboxed_pointer(T_PointerType p) const
  {
    return p;
  }

  template<typename T>
  inline T_PointerType impl_get_sandboxed_pointer(const void* p) const
  {
    return const_cast<T_PointerType>(p);
  }

  template<typename T>
  static inline void* impl_get_unsandboxed_pointer_no_ctx(
    T_PointerType p,
    const void* /* example_unsandboxed_ptr */,
    rlbox_ipc_shm_sandbox* (* // Func ptr
                         /* param: expensive_sandbox_finder */)(
      const void* example_unsandboxed_ptr))
  {
    return p;
  }

  template<typename T>
  static inline T_PointerType impl_get_sandboxed_pointer_no_ctx(
    const void* p,
    const void* /* example_unsandboxed_ptr */,
    rlbox_ipc_shm_sandbox* (* // Func ptr
                         /* param: expensive_sandbox_finder */)(
      const void* example_unsandboxed_ptr))
  {
    return const_cast<T_PointerType>(p);
  }

  inline T_PointerType impl_malloc_in_sandbox(size_t size)
  {
    void* p = mspace_malloc(shared_heap, size);
    return p;
  }

  inline void impl_free_in_sandbox(T_PointerType p) { mspace_free(shared_heap, p); }

  static inline bool impl_is_in_same_sandbox(const void*, const void*)
  {
    return true;
  }

  inline bool impl_is_pointer_in_sandbox_memory(const void* p)
  {
    uintptr_t addr = reinterpret_cast<uintptr_t>(p);
    uintptr_t base = reinterpret_cast<uintptr_t>(shm_ptr);
    return addr >= base && addr < base + SHARED_MEM_SIZE;
  }

  inline bool impl_is_pointer_in_app_memory(const void* p)
  {
    return !impl_is_pointer_in_sandbox_memory(p);
  }

  inline size_t impl_get_total_memory()
  {
    return SHARED_MEM_SIZE;
  }

  inline void* impl_get_memory_location()
  {
    return shm_ptr;
  }

  // adding a template so that we can use static_assert to fire only if this
  // function is invoked
  template<typename T = void>
  void* impl_lookup_symbol(const char* /* func_name */)
  {
    // Will fire if this impl_lookup_symbol is ever called for the static
    // sandbox
    constexpr bool fail = std::is_same_v<T, void>;
    rlbox_detail_static_fail_because(
      fail,
      "The no_op_sandbox uses static calls and thus developers should add\n\n"
      "#define RLBOX_USE_STATIC_CALLS() rlbox_noop_sandbox_lookup_symbol\n\n"
      "to their code, to ensure that static calls are handled correctly.");

    return nullptr;
  }

#define rlbox_ipc_shm_sandbox_lookup_symbol(func_name)                         \
  reinterpret_cast<void*>(&rlbox::rlbox_ipc_shm_sandbox::ipc_##func_name) /* NOLINT */

  template<typename T, typename T_Converted, typename... T_Args>
  auto impl_invoke_with_func_ptr(T_Converted* func_ptr, T_Args&&... params)
  {
#ifdef RLBOX_EMBEDDER_PROVIDES_TLS_STATIC_VARIABLES
    auto& thread_data = *get_rlbox_ipc_shm_sandbox_thread_data();
#endif
    auto old_sandbox = thread_data.sandbox;
    thread_data.sandbox = this;
    auto on_exit =
      detail::make_scope_exit([&] { thread_data.sandbox = old_sandbox; });
    return (*func_ptr)(params...);
  }

  template<typename T_Ret, typename... T_Args>
  inline T_PointerType impl_register_callback(void* key, void* callback)
  {
    RLBOX_ACQUIRE_UNIQUE_GUARD(lock, callback_mutex);

    void* chosen_trampoline = nullptr;

    // need a compile time for loop as we we need I to be a compile time value
    // this is because we are returning the I'th callback trampoline
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

  static inline std::pair<rlbox_ipc_shm_sandbox*, void*>
  impl_get_executed_callback_sandbox_and_key()
  {
#ifdef RLBOX_EMBEDDER_PROVIDES_TLS_STATIC_VARIABLES
    auto& thread_data = *get_rlbox_ipc_shm_sandbox_thread_data();
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
  {
    RLBOX_UNUSED(num);
    success = true;
    return src;
  }

  template<typename T>
  inline T* impl_deny_access(T* src, size_t num, bool& success)
  {
    RLBOX_UNUSED(num);
    success = true;
    return src;
  }
};

}
