BASIC_NOSIMD='../build_nosimd_release/image_change_quality'

RLBOX_NOOP='../build_nosimd_release/image_change_quality_rlbox_noop'
RLBOX_WASM2C='../build_nosimd_wasm_release/image_change_quality_rlbox_wasm2c'

SA_WASM2C_GUARDPAGE='../build_nosimd_wasm_sa_release/image_change_quality_wasm2c_guardpage'
SA_WASM2C_BOUNDSCHECK='../build_nosimd_wasm_sa_release/image_change_quality_wasm2c_boundscheck'
SA_WASM2C_WATCH='../build_nosimd_wasm_sa_release/image_change_quality_wasm2c_watch'


build_and_run() {
    make "$2" > /dev/null
    
    echo -n "[$1] "
    ./"$2"
}

# Basic
build_and_run "basic_nosimd" "$BASIC_NOSIMD"

# RLBox 
build_and_run "rlbox_noop" "$RLBOX_NOOP"
build_and_run "rlbox_wasm2c" "$RLBOX_WASM2C"

# wasm2c standalone
build_and_run "sa_wasm2c_guardpage" "$SA_WASM2C_GUARDPAGE"
build_and_run "sa_wasm2c_boundscheck" "$SA_WASM2C_BOUNDSCHECK"
build_and_run "sa_wasm2c_watch" "$SA_WASM2C_WATCH"
