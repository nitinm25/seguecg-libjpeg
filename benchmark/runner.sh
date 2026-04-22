ITERATIONS=${1:-1}  # Iteration count

multi_exec() {    
    echo "[$1]:"
    for ((i=1; i<=ITERATIONS; i++)); do
        ./"$2"
        sleep 5
    done
    echo ""
}

multi_exec_socket() {    
    echo "[ipc_socket]:"
    for ((i=1; i<=ITERATIONS; i++)); do
        ./"$1" &
        sleep 1
        ./"$2"

        # sleep 5
    done
    echo ""
}

BASIC_NOSIMD='../build_nosimd_release/image_change_quality'
RLBOX_NOOP='../build_nosimd_release/image_change_quality_rlbox_noop'
RLBOX_WASM2C='../build_nosimd_wasm_release/image_change_quality_rlbox_wasm2c'
SA_WASM2C_GUARDPAGE='../build_nosimd_wasm_sa_release/image_change_quality_wasm2c_guardpage'
SA_WASM2C_BOUNDSCHECK='../build_nosimd_wasm_sa_release/image_change_quality_wasm2c_boundscheck'
SA_WASM2C_WATCH='../build_nosimd_wasm_sa_release/image_change_quality_wasm2c_watch'
IPC_SOCKET_SERVER='../build_nosimd_release/image_change_quality_socket_server'
IPC_SOCKET_CLIENT='../build_nosimd_release/image_change_quality_socket_client'
RLBOX_IPC_SHM='../build_nosimd_release/image_change_quality_rlbox_ipc_shm'
RLBOX_IPC_SHM_INSTRUMENT='../build_nosimd_release/image_change_quality_rlbox_ipc_shm_instrument'

# Build
# make clean
make build_ipc_benchmark > /dev/null
make build_ipc_benchmark_instrument > /dev/null
# make build_ipc_benchmark_minimal > /dev/null

# Basic
# multi_exec "basic_nosimd" "$BASIC_NOSIMD"

# RLBox 
# multi_exec "rlbox_noop" "$RLBOX_NOOP"
multi_exec "rlbox_wasm2c" "$RLBOX_WASM2C"

multi_exec "rlbox_ipc_shm" "$RLBOX_IPC_SHM"

multi_exec "rlbox_ipc_shm_instrument" "$RLBOX_IPC_SHM_INSTRUMENT"

# wasm2c standalone
# multi_exec "sa_wasm2c_guardpage" "$SA_WASM2C_GUARDPAGE"
# multi_exec "sa_wasm2c_boundscheck" "$SA_WASM2C_BOUNDSCHECK"
# multi_exec "sa_wasm2c_watch" "$SA_WASM2C_WATCH"

# IPC socket
multi_exec_socket "$IPC_SOCKET_SERVER" "$IPC_SOCKET_CLIENT"
