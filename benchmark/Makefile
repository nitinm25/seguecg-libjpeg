NOTPARALLEL:

.DEFAULT_GOAL := build

SHELL := /bin/bash

MKFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MKFILE_DIR := $(dir $(MKFILE_PATH))
REPO_PATH=$(MKFILE_DIR)/..
ROOT_PATH=$(REPO_PATH)/..
LIBJPEG_SRC=$(REPO_PATH)/libjpeg-turbo
PARALLEL_COUNT=$(shell nproc)

CC=clang
CXX=clang++ -std=c++17
WASI_SDK_PATH=$(ROOT_PATH)/wasi-sdk
WASI_CC=$(WASI_SDK_PATH)/bin/clang --sysroot $(WASI_SDK_PATH)/share/wasi-sysroot/
WASI_CXX=$(WASI_SDK_PATH)/bin/clang++ --sysroot $(WASI_SDK_PATH)/share/wasi-sysroot/ -std=c++17
WASI_AR=$(WASI_SDK_PATH)/bin/ar
WASI_LD=$(WASI_SDK_PATH)/bin/wasm-ld
WASI_CFLAGS=-I $(MKFILE_DIR)/wasmcompat
WASI_LDFLAGS=-Wl,--export-all -Wl,--stack-first -Wl,-z,stack-size=1048576 -Wl,--no-entry -Wl,--import-memory -Wl,--import-table
WASI_SA_LDFLAGS=-Wl,--stack-first -Wl,-z,stack-size=1048576
WASMTIME=$(ROOT_PATH)/wasmtime/target/release/wasmtime

RLBOX_WASM2C_PATH=$(ROOT_PATH)/rlbox_wasm2c_sandbox

WASM2C_SRC_PATH=$(ROOT_PATH)/seguecg-wasm2c
WASM2C_RUNTIME=$(WASM2C_SRC_PATH)/wasm2c/wasm-rt-impl.c $(RLBOX_WASM2C_PATH)/src/wasm2c_rt_mem.c $(RLBOX_WASM2C_PATH)/src/wasm2c_rt_minwasi.c
WASM2C_SA_RUNTIME=$(WASM2C_SRC_PATH)/wasm2c/wasm-rt-impl.c $(RLBOX_WASM2C_PATH)/src/wasm2c_rt_minwasi.c

SIMDE_CFLAGS=-msimd128 -I$(WASM2C_SRC_PATH)/third_party/simde/simde/arm -DSIMDE_WASM_SIMD128_NATIVE=1 -DSIMDE_ENABLE_NATIVE_ALIASES=1

ifdef DEBUG
BUILD_TYPE_DIR=debug
BUILD_TYPE_CONFIG=Debug
BUILD_TYPE_FLAGS=-O0 -g -DWASM_RT_NOINLINE
BUILD_BINARY_FLAGS=-fsanitize=address
else
BUILD_TYPE_DIR=release
BUILD_TYPE_CONFIG=Release
BUILD_TYPE_FLAGS=-O3
BUILD_BINARY_FLAGS=
endif

LIBJPEG_INCLUDES=-I$(LIBJPEG_SRC)/ -I$(REPO_PATH)/build_$(BUILD_TYPE_DIR)
LIBJPEG_NOSIMD_INCLUDES=-I$(LIBJPEG_SRC)/ -I$(REPO_PATH)/build_nosimd_$(BUILD_TYPE_DIR)
LIBJPEG_NOSIMD_WASM_INCLUDES=-I$(LIBJPEG_SRC)/ -I$(REPO_PATH)/build_nosimd_wasm_$(BUILD_TYPE_DIR)
LIBJPEG_NOSIMD_WASM_SA_INCLUDES=-I$(LIBJPEG_SRC)/ -I$(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)
LIBJPEG_ARM32SIMD_WASM_SA_INCLUDES=-I$(LIBJPEG_SRC)/ -I$(REPO_PATH)/build_arm32simd_wasm_sa_$(BUILD_TYPE_DIR)
LIBJPEG_ARM64SIMD_WASM_SA_INCLUDES=-I$(LIBJPEG_SRC)/ -I$(REPO_PATH)/build_arm64simd_wasm_sa_$(BUILD_TYPE_DIR)
LIBJPEG_LINK=$(REPO_PATH)/build_$(BUILD_TYPE_DIR)/libjpeg.a
LIBJPEG_NOSIMD_LINK=$(REPO_PATH)/build_nosimd_$(BUILD_TYPE_DIR)/libjpeg.a
LIBJPEG_NOSIMD_WASM_LINK=-Wl,--whole-archive $(REPO_PATH)/build_nosimd_wasm_$(BUILD_TYPE_DIR)/libjpeg.a -Wl,--no-whole-archive
LIBJPEG_NOSIMD_WASM_SA_LINK=-Wl,--whole-archive $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.a -Wl,--no-whole-archive
LIBJPEG_ARM32SIMD_WASM_SA_LINK=-Wl,--whole-archive $(REPO_PATH)/build_arm32simd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.a -Wl,--no-whole-archive
LIBJPEG_ARM64SIMD_WASM_SA_LINK=-Wl,--whole-archive $(REPO_PATH)/build_arm64simd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.a -Wl,--no-whole-archive
RLBOX_INCLUDES=-I $(ROOT_PATH)/rlbox/code/include -I $(RLBOX_WASM2C_PATH)/include -DRLBOX_WASM2C_MODULE_NAME=libjpeg
RLBOX_LINK=-lrt
WASM2C_BUILD_INCLUDES=-I $(WASM2C_SRC_PATH)/wasm2c -I$(REPO_PATH)/build_nosimd_wasm_$(BUILD_TYPE_DIR)/ -DWASM_RT_SKIP_SIGNAL_RECOVERY=1 -DWASM_RT_USE_STACK_DEPTH_COUNT=0
WASM2C_NOSIMD_SA_BUILD_INCLUDES=-I $(WASM2C_SRC_PATH)/wasm2c -I$(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/ -DWASM_RT_SKIP_SIGNAL_RECOVERY=1 -DWASM_RT_USE_STACK_DEPTH_COUNT=0
WASM2C_ARM32SIMD_SA_BUILD_INCLUDES=-I $(WASM2C_SRC_PATH)/wasm2c -I$(REPO_PATH)/build_arm32simd_wasm_sa_$(BUILD_TYPE_DIR)/ -DWASM_RT_SKIP_SIGNAL_RECOVERY=1 -DWASM_RT_USE_STACK_DEPTH_COUNT=0 -I$(WASM2C_SRC_PATH)/third_party/simde/
WASM2C_ARM64SIMD_SA_BUILD_INCLUDES=-I $(WASM2C_SRC_PATH)/wasm2c -I$(REPO_PATH)/build_arm64simd_wasm_sa_$(BUILD_TYPE_DIR)/ -DWASM_RT_SKIP_SIGNAL_RECOVERY=1 -DWASM_RT_USE_STACK_DEPTH_COUNT=0 -I$(WASM2C_SRC_PATH)/third_party/simde/

################Deps#####################

test_out.jpeg: test.jpeg ../build_$(BUILD_TYPE_DIR)/libjpeg.a
	$(CC) -O0 -g generate_test_out.c -o ./generate_test_out $(LIBJPEG_INCLUDES) $(LIBJPEG_LINK)
	./generate_test_out test.jpeg test_out.jpeg

test_bytes.h: test.jpeg test_out.jpeg
	if [ ! -e "test_bytes.h" ]; then \
		echo "unsigned char inputData[] = {" > tmp_$@; \
		hexdump -ve '1/1 "0x%.2x, "' test.jpeg >> tmp_$@; \
		echo "0x00 };" >> tmp_$@; \
		echo "unsigned char outputData[] = {" >> tmp_$@; \
		hexdump -ve '1/1 "0x%.2x, "' test_out.jpeg >> tmp_$@; \
		echo "0x00 };" >> tmp_$@; \
		mv tmp_$@ $@; \
	fi

$(WASM2C_SRC_PATH)/build_$(BUILD_TYPE_DIR)/wasm2c: $(WASM2C_SRC_PATH)/src/template/wasm2c.declarations.c $(WASM2C_SRC_PATH)/wasm2c/wasm-rt-impl.c $(WASM2C_SRC_PATH)/wasm2c/wasm-rt.h
	cmake -S $(WASM2C_SRC_PATH) -B $(WASM2C_SRC_PATH)/build_$(BUILD_TYPE_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE_CONFIG)
	cd $(WASM2C_SRC_PATH)/build_$(BUILD_TYPE_DIR) && make -j$(PARALLEL_COUNT)

#####################################

../build_$(BUILD_TYPE_DIR)/libjpeg.a:
	cmake -S $(LIBJPEG_SRC) -B ../build_$(BUILD_TYPE_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE_CONFIG) -DWITH_SIMD=1 -DREQUIRE_SIMD=1
	cd ../build_$(BUILD_TYPE_DIR) && make -j$(PARALLEL_COUNT)

../build_nosimd_$(BUILD_TYPE_DIR)/libjpeg.a:
	cmake -S $(LIBJPEG_SRC) -B ../build_nosimd_$(BUILD_TYPE_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE_CONFIG) -DWITH_SIMD=0
	cd ../build_nosimd_$(BUILD_TYPE_DIR) && make -j$(PARALLEL_COUNT)

../build_nosimd_wasm_$(BUILD_TYPE_DIR)/libjpeg.wasm.c: $(WASM2C_SRC_PATH)/build_$(BUILD_TYPE_DIR)/wasm2c
	CC="$(WASI_CC)" CFLAGS="$(WASI_CFLAGS)" LDFLAGS="$(WASI_LDFLAGS)" \
		CXX="$(WASI_CXX)" CXXFLAGS="$(WASI_CFLAGS)" LDFLAGS="$(WASI_LDFLAGS)" \
		AR="$(WASI_AR)" LD="$(WASI_LD)" \
		cmake -S $(LIBJPEG_SRC) -B ../build_nosimd_wasm_$(BUILD_TYPE_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE_CONFIG) -DENABLE_SHARED=0 -DWITH_SIMD=0
	cd ../build_nosimd_wasm_$(BUILD_TYPE_DIR) && make -j$(PARALLEL_COUNT)
	$(WASI_CC) $(BUILD_TYPE_FLAGS) $(WASI_CFLAGS) $(WASI_LDFLAGS) -o ../build_nosimd_wasm_$(BUILD_TYPE_DIR)/libjpeg.wasm $(RLBOX_WASM2C_PATH)/c_src/wasm2c_sandbox_wrapper.c $(LIBJPEG_NOSIMD_WASM_LINK)
	$(WASM2C_SRC_PATH)/build_$(BUILD_TYPE_DIR)/wasm2c ../build_nosimd_wasm_$(BUILD_TYPE_DIR)/libjpeg.wasm -n libjpeg -o ../build_nosimd_wasm_$(BUILD_TYPE_DIR)/libjpeg.wasm.c

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c: $(WASM2C_SRC_PATH)/build_$(BUILD_TYPE_DIR)/wasm2c image_change_quality.c test_bytes.h
	CC="$(WASI_CC)" CFLAGS="$(WASI_CFLAGS)" LDFLAGS="$(WASI_SA_LDFLAGS)" \
		CXX="$(WASI_CXX)" CXXFLAGS="$(WASI_CFLAGS)" LDFLAGS="$(WASI_SA_LDFLAGS)" \
		AR="$(WASI_AR)" LD="$(WASI_LD)" \
		cmake -S $(LIBJPEG_SRC) -B ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE_CONFIG) -DENABLE_SHARED=0 -DWITH_SIMD=0
	cd ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && make -j$(PARALLEL_COUNT)
	$(WASI_CC) $(BUILD_TYPE_FLAGS) $(WASI_CFLAGS) $(WASI_SA_LDFLAGS) -o ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm image_change_quality.c $(LIBJPEG_NOSIMD_WASM_SA_INCLUDES) $(LIBJPEG_NOSIMD_WASM_SA_LINK)
	$(WASM2C_SRC_PATH)/build_$(BUILD_TYPE_DIR)/wasm2c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm -n libjpeg -o ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c

../build_arm32simd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c: $(WASM2C_SRC_PATH)/build_$(BUILD_TYPE_DIR)/wasm2c image_change_quality.c test_bytes.h
	CC="$(WASI_CC)" CFLAGS="$(WASI_CFLAGS) $(SIMDE_CFLAGS)" LDFLAGS="$(WASI_SA_LDFLAGS)" \
		CXX="$(WASI_CXX)" CXXFLAGS="$(WASI_CFLAGS) $(SIMDE_CFLAGS)" LDFLAGS="$(WASI_SA_LDFLAGS)" \
		AR="$(WASI_AR)" LD="$(WASI_LD)" \
		cmake -S $(LIBJPEG_SRC) -B ../build_arm32simd_wasm_sa_$(BUILD_TYPE_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE_CONFIG) -DENABLE_SHARED=0 -DWITH_SIMD=1 -DREQUIRE_SIMD=1 -DNEON_INTRINSICS=1 -DCPU_TYPE="arm" -DIS_WASM_BUILD=1
	cd ../build_arm32simd_wasm_sa_$(BUILD_TYPE_DIR) && make -j$(PARALLEL_COUNT)
	$(WASI_CC) $(BUILD_TYPE_FLAGS) $(WASI_CFLAGS) $(WASI_SA_LDFLAGS) -o ../build_arm32simd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm image_change_quality.c $(LIBJPEG_ARM32SIMD_WASM_SA_INCLUDES) $(LIBJPEG_ARM32SIMD_WASM_SA_LINK)
	$(WASM2C_SRC_PATH)/build_$(BUILD_TYPE_DIR)/wasm2c ../build_arm32simd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm -n libjpeg -o ../build_arm32simd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c

../build_arm64simd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c: $(WASM2C_SRC_PATH)/build_$(BUILD_TYPE_DIR)/wasm2c image_change_quality.c test_bytes.h
	CC="$(WASI_CC)" CFLAGS="$(WASI_CFLAGS) $(SIMDE_CFLAGS)" LDFLAGS="$(WASI_SA_LDFLAGS)" \
		CXX="$(WASI_CXX)" CXXFLAGS="$(WASI_CFLAGS) $(SIMDE_CFLAGS)" LDFLAGS="$(WASI_SA_LDFLAGS)" \
		AR="$(WASI_AR)" LD="$(WASI_LD)" \
		cmake -S $(LIBJPEG_SRC) -B ../build_arm64simd_wasm_sa_$(BUILD_TYPE_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE_CONFIG) -DENABLE_SHARED=0 -DWITH_SIMD=1 -DREQUIRE_SIMD=1 -DNEON_INTRINSICS=1 -DCPU_TYPE="arm64" -DIS_WASM_BUILD=1
	cd ../build_arm64simd_wasm_sa_$(BUILD_TYPE_DIR) && make -j$(PARALLEL_COUNT)
	$(WASI_CC) $(BUILD_TYPE_FLAGS) $(WASI_CFLAGS) $(WASI_SA_LDFLAGS) -o ../build_arm64simd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm image_change_quality.c $(LIBJPEG_ARM64SIMD_WASM_SA_INCLUDES) $(LIBJPEG_ARM64SIMD_WASM_SA_LINK)
	$(WASM2C_SRC_PATH)/build_$(BUILD_TYPE_DIR)/wasm2c ../build_arm64simd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm -n libjpeg -o ../build_arm64simd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c

#####################################

../build_$(BUILD_TYPE_DIR)/image_change_quality: image_change_quality.c test_bytes.h ../build_$(BUILD_TYPE_DIR)/libjpeg.a
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $< $(LIBJPEG_INCLUDES) $(LIBJPEG_LINK)

../build_nosimd_$(BUILD_TYPE_DIR)/image_change_quality: image_change_quality.c test_bytes.h ../build_nosimd_$(BUILD_TYPE_DIR)/libjpeg.a
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $< $(LIBJPEG_NOSIMD_INCLUDES) $(LIBJPEG_NOSIMD_LINK)

#####################################

../build_nosimd_$(BUILD_TYPE_DIR)/image_change_quality_socket_server: socket/image_change_quality_socket_server.cpp test_bytes.h ../build_nosimd_$(BUILD_TYPE_DIR)/libjpeg.a
	$(CXX) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $< $(LIBJPEG_NOSIMD_INCLUDES) $(LIBJPEG_NOSIMD_LINK)

../build_nosimd_$(BUILD_TYPE_DIR)/image_change_quality_socket_client: socket/image_change_quality_socket_client.cpp test_bytes.h ../build_nosimd_$(BUILD_TYPE_DIR)/libjpeg.a
	$(CXX) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $< $(LIBJPEG_NOSIMD_INCLUDES) $(LIBJPEG_NOSIMD_LINK)

#####################################

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_guardpage: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_GUARD_PAGES=1

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_guardpage_segue: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_GUARD_PAGES=1 -DWASM_RT_USE_SEGUE=1 -mfsgsbase

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_boundscheck: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_BOUNDS_CHECK=1

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_boundscheck_segue: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_BOUNDS_CHECK=1 -DWASM_RT_USE_SEGUE=1 -mfsgsbase

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_boundscheck_spectre: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_BOUNDS_CHECK=1 -DWASM_RT_SPECTREMASK=1

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_boundscheck_trap_len: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_BOUNDS_CHECK_TRAP=1 -DWASM_RT_MEMCHECK_BOUNDS_CHECK_TRAP_SCHEME=1

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_boundscheck_trap_neg1: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_BOUNDS_CHECK_TRAP=1 -DWASM_RT_MEMCHECK_BOUNDS_CHECK_TRAP_SCHEME=2

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_boundscheck_asm: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_BOUNDS_CHECK_ASM=1

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_boundscheck_asm_spectre: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_BOUNDS_CHECK_ASM=1

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowpage_read: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_PAGE=1 -DWASM_RT_MEMCHECK_SHADOW_PAGE_SCHEME=1

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowpage_read_shadowsegue: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_PAGE=1 -DWASM_RT_MEMCHECK_SHADOW_PAGE_SCHEME=1 -DWASM_RT_USE_SHADOW_SEGUE=1 -mfsgsbase

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowpage_read_shadowsegue_spectre: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_PAGE=1 -DWASM_RT_MEMCHECK_SHADOW_PAGE_SCHEME=1 -DWASM_RT_USE_SHADOW_SEGUE=1 -mfsgsbase -DWASM_RT_SPECTREMASK=1

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowpage_1tb_read_shadowsegue: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_PAGE=1 -DWASM_RT_MEMCHECK_SHADOW_PAGE_SCHEME=5 -DWASM_RT_USE_SHADOW_SEGUE=1 -mfsgsbase

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowpage_firstbyte_read: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_PAGE=1 -DWASM_RT_MEMCHECK_SHADOW_PAGE_SCHEME=2

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowpage_write: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_PAGE=1 -DWASM_RT_MEMCHECK_SHADOW_PAGE_SCHEME=3

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowpage_write_shadowsegue: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_PAGE=1 -DWASM_RT_MEMCHECK_SHADOW_PAGE_SCHEME=3 -DWASM_RT_USE_SHADOW_SEGUE=1 -mfsgsbase

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowpage_firstbyte_write: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_PAGE=1 -DWASM_RT_MEMCHECK_SHADOW_PAGE_SCHEME=4

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_preshadowpage_256mb_read: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_PRESHADOW_PAGE=1 -mfsgsbase

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_preshadowpage_256mb_read_segue: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_PRESHADOW_PAGE=1 -DWASM_RT_USE_SEGUE=1 -mfsgsbase

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_preshadowpage_256mb_read_shadowsegue: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_PRESHADOW_PAGE=1 -DWASM_RT_USE_SHADOW_SEGUE=1 -mfsgsbase

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_preshadowpage_256mb_read_segue_shadowsegue: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_PRESHADOW_PAGE=1 -DWASM_RT_USE_SEGUE=1 -DWASM_RT_USE_SHADOW_SEGUE=1 -mfsgsbase

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_read4per4gb: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_BYTES=1 -DWASM_RT_MEMCHECK_SHADOW_BYTES_SCHEME=1

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_read1per4gb: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_BYTES=1 -DWASM_RT_MEMCHECK_SHADOW_BYTES_SCHEME=2

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_read1per64kb: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_BYTES=1 -DWASM_RT_MEMCHECK_SHADOW_BYTES_SCHEME=3

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_read1per16mb: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_BYTES=1 -DWASM_RT_MEMCHECK_SHADOW_BYTES_SCHEME=4

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_read4per4gb_spectre: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_BYTES=1 -DWASM_RT_MEMCHECK_SHADOW_BYTES_SCHEME=1 -DWASM_RT_SPECTREMASK=1

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_read1per4gb_spectre: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_BYTES=1 -DWASM_RT_MEMCHECK_SHADOW_BYTES_SCHEME=2 -DWASM_RT_SPECTREMASK=1

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_write4per4gb: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_BYTES=1 -DWASM_RT_MEMCHECK_SHADOW_BYTES_SCHEME=5

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_write1per4gb: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_BYTES=1 -DWASM_RT_MEMCHECK_SHADOW_BYTES_SCHEME=6

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_write1per64kb: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_BYTES=1 -DWASM_RT_MEMCHECK_SHADOW_BYTES_SCHEME=7

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_write1per16mb: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_BYTES=1 -DWASM_RT_MEMCHECK_SHADOW_BYTES_SCHEME=8

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_read4per4gb_shadowsegue: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_BYTES=1 -DWASM_RT_MEMCHECK_SHADOW_BYTES_SCHEME=1 -DWASM_RT_USE_SHADOW_SEGUE=1 -mfsgsbase

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_read1per4gb_shadowsegue: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_BYTES=1 -DWASM_RT_MEMCHECK_SHADOW_BYTES_SCHEME=2 -DWASM_RT_USE_SHADOW_SEGUE=1 -mfsgsbase

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_read1per64kb_shadowsegue: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_BYTES=1 -DWASM_RT_MEMCHECK_SHADOW_BYTES_SCHEME=3 -DWASM_RT_USE_SHADOW_SEGUE=1 -mfsgsbase

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_read1per16mb_shadowsegue: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_BYTES=1 -DWASM_RT_MEMCHECK_SHADOW_BYTES_SCHEME=4 -DWASM_RT_USE_SHADOW_SEGUE=1 -mfsgsbase

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_read4per4gb_shadowsegue_spectre: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_BYTES=1 -DWASM_RT_MEMCHECK_SHADOW_BYTES_SCHEME=1 -DWASM_RT_USE_SHADOW_SEGUE=1 -mfsgsbase  -DWASM_RT_SPECTREMASK=1

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_read1per4gb_shadowsegue_spectre: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_BYTES=1 -DWASM_RT_MEMCHECK_SHADOW_BYTES_SCHEME=2 -DWASM_RT_USE_SHADOW_SEGUE=1 -mfsgsbase  -DWASM_RT_SPECTREMASK=1

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_write4per4gb_shadowsegue: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_BYTES=1 -DWASM_RT_MEMCHECK_SHADOW_BYTES_SCHEME=5 -DWASM_RT_USE_SHADOW_SEGUE=1 -mfsgsbase

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_write1per4gb_shadowsegue: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_BYTES=1 -DWASM_RT_MEMCHECK_SHADOW_BYTES_SCHEME=6 -DWASM_RT_USE_SHADOW_SEGUE=1 -mfsgsbase

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_write1per64kb_shadowsegue: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_BYTES=1 -DWASM_RT_MEMCHECK_SHADOW_BYTES_SCHEME=7 -DWASM_RT_USE_SHADOW_SEGUE=1 -mfsgsbase

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_write1per16mb_shadowsegue: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_BYTES=1 -DWASM_RT_MEMCHECK_SHADOW_BYTES_SCHEME=8 -DWASM_RT_USE_SHADOW_SEGUE=1 -mfsgsbase

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_preshadowbytes_read1per4gb: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_PRESHADOW_BYTES=1

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_preshadowbytes_read1per4gb_segue: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_PRESHADOW_BYTES=1 -DWASM_RT_USE_SEGUE=1 -mfsgsbase

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_preshadowbytes_read1per4gb_shadowsegue: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_PRESHADOW_BYTES=1 -DWASM_RT_USE_SHADOW_SEGUE=1 -mfsgsbase

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_preshadowbytes_read1per4gb_segue_shadowsegue: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_PRESHADOW_BYTES=1  -DWASM_RT_USE_SEGUE=1 -DWASM_RT_USE_SHADOW_SEGUE=1 -mfsgsbase

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_preshadowbytes_read1per4gb_segue_shadowsegue_spectre: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_PRESHADOW_BYTES=1  -DWASM_RT_USE_SEGUE=1 -DWASM_RT_USE_SHADOW_SEGUE=1 -mfsgsbase -DWASM_RT_SPECTREMASK=1

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_bitscan_read_segue: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_BYTES_BITSCAN=1 -DWASM_RT_MEMCHECK_SHADOW_BYTES_BITSCAN_SCHEME=1 -DWASM_RT_USE_SEGUE=1 -mfsgsbase

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_bitscan_write_segue: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_BYTES_BITSCAN=1 -DWASM_RT_MEMCHECK_SHADOW_BYTES_BITSCAN_SCHEME=2 -DWASM_RT_USE_SEGUE=1 -mfsgsbase

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_watch: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_DEBUG_WATCH=1

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_watch_shadowsegue: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_DEBUG_WATCH=1 -DWASM_RT_USE_SHADOW_SEGUE=1 -mfsgsbase

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_mpx: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_MPX=1

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_mpx_segue: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_MPX=1 -DWASM_RT_USE_SEGUE=1 -mfsgsbase

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_dummy: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_DUMMY=1

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_dummy2: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_DUMMY2=1

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_dummy3: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_DUMMY3=1

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytestag_floatzone: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_BYTES_TAG=1 -DWASM_RT_MEMCHECK_SHADOW_BYTES_TAG_SCHEME=1

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytestag_floatzone_shadowsegue: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_BYTES_TAG=1 -DWASM_RT_MEMCHECK_SHADOW_BYTES_TAG_SCHEME=1 -DWASM_RT_USE_SHADOW_SEGUE=1 -mfsgsbase

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytestag_floatzone_virtpinned: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_BYTES_TAG=1 -DWASM_RT_MEMCHECK_SHADOW_BYTES_TAG_SCHEME=2

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytestag_floatzone_virtpinned_shadowsegue: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SHADOW_BYTES_TAG=1 -DWASM_RT_MEMCHECK_SHADOW_BYTES_TAG_SCHEME=2 -DWASM_RT_USE_SHADOW_SEGUE=1 -mfsgsbase

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_mask_pdep: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_MASK_PDEP=1 -mbmi2

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_mask_pdep_segue: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_MASK_PDEP=1 -mbmi2 -DWASM_RT_USE_SEGUE=1 -mfsgsbase

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_mask_pdep48: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_MASK_PDEP48=1 -mbmi2

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_mask_pdep48_segue: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_MASK_PDEP48=1 -mbmi2 -DWASM_RT_USE_SEGUE=1 -mfsgsbase

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_mask_pext: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_MASK_PEXT=1 -mbmi2

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_mask_pext_segue: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_MASK_PEXT=1 -mbmi2 -DWASM_RT_USE_SEGUE=1 -mfsgsbase

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_sswrite_sim: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_NOSIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_SSWRITE_SIM=1

#####################################

../build_arm32simd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_guardpage: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_arm32simd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_arm32simd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_ARM32SIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_GUARD_PAGES=1

../build_arm64simd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_guardpage: image_change_quality.c test_bytes.h  $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_arm64simd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $(WASM2C_SA_RUNTIME) $(REPO_PATH)/benchmark/wasm-rt-static-runner.c ../build_arm64simd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c $(WASM2C_ARM32SIMD_SA_BUILD_INCLUDES) $(RLBOX_INCLUDES) -DWASM_RT_MEMCHECK_GUARD_PAGES=1

#####################################

../build_$(BUILD_TYPE_DIR)/image_change_quality_rlbox_noop: image_change_quality_rlbox.cpp test_bytes.h ../build_$(BUILD_TYPE_DIR)/libjpeg.a
	$(CXX) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $< $(LIBJPEG_INCLUDES) $(RLBOX_INCLUDES) $(LIBJPEG_LINK) $(RLBOX_LINK)

../build_nosimd_$(BUILD_TYPE_DIR)/image_change_quality_rlbox_noop: image_change_quality_rlbox.cpp test_bytes.h ../build_nosimd_$(BUILD_TYPE_DIR)/libjpeg.a
	$(CXX) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $< $(LIBJPEG_NOSIMD_INCLUDES) $(RLBOX_INCLUDES) $(LIBJPEG_NOSIMD_LINK) $(RLBOX_LINK)

../build_nosimd_$(BUILD_TYPE_DIR)/image_change_quality_rlbox_ipc_shm: image_change_quality_rlbox.cpp test_bytes.h ../build_nosimd_$(BUILD_TYPE_DIR)/libjpeg.a
	$(CXX) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -o $@ $< -DRLBOX_IPC_SHM $(LIBJPEG_NOSIMD_INCLUDES) $(RLBOX_INCLUDES) $(LIBJPEG_NOSIMD_LINK) $(RLBOX_LINK)

../build_nosimd_$(BUILD_TYPE_DIR)/image_change_quality_socket_server_instrument: socket/image_change_quality_socket_server.cpp test_bytes.h ../build_nosimd_$(BUILD_TYPE_DIR)/libjpeg.a
	$(CXX) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -DIPC_INSTRUMENT -o $@ $< $(LIBJPEG_NOSIMD_INCLUDES) $(LIBJPEG_NOSIMD_LINK)

../build_nosimd_$(BUILD_TYPE_DIR)/image_change_quality_rlbox_ipc_shm_instrument: image_change_quality_rlbox.cpp test_bytes.h ../build_nosimd_$(BUILD_TYPE_DIR)/libjpeg.a ../build_nosimd_$(BUILD_TYPE_DIR)/image_change_quality_socket_server_instrument
	$(CXX) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) -DRLBOX_IPC_SHM -DIPC_INSTRUMENT \
	  -DIPC_SERVER_PATH='"../build_nosimd_$(BUILD_TYPE_DIR)/image_change_quality_socket_server_instrument"' \
	  -o $@ $< $(LIBJPEG_NOSIMD_INCLUDES) $(RLBOX_INCLUDES) $(LIBJPEG_NOSIMD_LINK) $(RLBOX_LINK)

build_ipc_benchmark_instrument: test_bytes.h
build_ipc_benchmark_instrument: ../build_nosimd_$(BUILD_TYPE_DIR)/image_change_quality_socket_server_instrument
build_ipc_benchmark_instrument: ../build_nosimd_$(BUILD_TYPE_DIR)/image_change_quality_rlbox_ipc_shm_instrument

../build_nosimd_wasm_$(BUILD_TYPE_DIR)/image_change_quality_rlbox_wasm2c: image_change_quality_rlbox.cpp test_bytes.h ../build_nosimd_wasm_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	rm -rf ../build_nosimd_wasm_$(BUILD_TYPE_DIR)/objs
	mkdir -p ../build_nosimd_wasm_$(BUILD_TYPE_DIR)/objs
	cd ../build_nosimd_wasm_$(BUILD_TYPE_DIR)/objs && \
		$(CC) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) $(WASM2C_RUNTIME) $(RLBOX_INCLUDES) $(REPO_PATH)/build_nosimd_wasm_$(BUILD_TYPE_DIR)/libjpeg.wasm.c -c $(WASM2C_BUILD_INCLUDES)
	$(CXX) $(BUILD_TYPE_FLAGS) $(BUILD_BINARY_FLAGS) ../build_nosimd_wasm_$(BUILD_TYPE_DIR)/objs/*.o -o $@ $< -DRLBOX_WASM2C $(LIBJPEG_NOSIMD_WASM_INCLUDES) $(WASM2C_BUILD_INCLUDES) $(RLBOX_INCLUDES) $(RLBOX_LINK)

#####################################

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasmtime_guardpage.cwasm: image_change_quality.c test_bytes.h ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(WASMTIME) compile -o $@ ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasmtime_boundscheck.cwasm: image_change_quality.c test_bytes.h ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(WASMTIME) compile --cranelift-set enable_heap_access_spectre_mitigation=false --static-memory-guard-size 0 --dynamic-memory-guard-size 0 -o $@ ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm

../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasmtime_boundscheck_spectre.cwasm: image_change_quality.c test_bytes.h ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm.c
	$(WASMTIME) compile --cranelift-set enable_heap_access_spectre_mitigation=true --static-memory-guard-size 0 --dynamic-memory-guard-size 0 -o $@ ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/libjpeg.wasm

#####################################

build: test_bytes.h
build: ../build_$(BUILD_TYPE_DIR)/image_change_quality_rlbox_noop
build: ../build_nosimd_$(BUILD_TYPE_DIR)/image_change_quality_rlbox_noop
build: ../build_nosimd_wasm_$(BUILD_TYPE_DIR)/image_change_quality_rlbox_wasm2c

build: ../build_$(BUILD_TYPE_DIR)/image_change_quality
build: ../build_nosimd_$(BUILD_TYPE_DIR)/image_change_quality

build: ../build_nosimd_ipc_$(BUILD_TYPE_DIR)/image_change_quality_socket_server
build: ../build_nosimd_ipc_$(BUILD_TYPE_DIR)/image_change_quality_socket_client

build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasmtime_guardpage.cwasm
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasmtime_boundscheck.cwasm
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasmtime_boundscheck_spectre.cwasm
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_guardpage
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_guardpage_segue
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_boundscheck
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_boundscheck_segue
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_boundscheck_spectre
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_boundscheck_trap_len
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_boundscheck_trap_neg1
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_boundscheck_asm
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_boundscheck_asm_spectre
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowpage_read
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowpage_read_shadowsegue
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowpage_read_shadowsegue_spectre
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowpage_1tb_read_shadowsegue
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowpage_firstbyte_read
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowpage_write
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowpage_write_shadowsegue
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowpage_firstbyte_write
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_preshadowpage_256mb_read
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_preshadowpage_256mb_read_segue
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_preshadowpage_256mb_read_shadowsegue
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_preshadowpage_256mb_read_segue_shadowsegue
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_read4per4gb
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_read1per4gb
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_read1per64kb
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_read1per16mb
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_read4per4gb_spectre
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_read1per4gb_spectre
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_write4per4gb
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_write1per4gb
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_write1per64kb
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_write1per16mb
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_read4per4gb_shadowsegue
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_read1per4gb_shadowsegue
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_read1per64kb_shadowsegue
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_read1per16mb_shadowsegue
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_read4per4gb_shadowsegue_spectre
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_read1per4gb_shadowsegue_spectre
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_write4per4gb_shadowsegue
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_write1per4gb_shadowsegue
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_write1per64kb_shadowsegue
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_write1per16mb_shadowsegue
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_preshadowbytes_read1per4gb
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_preshadowbytes_read1per4gb_segue
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_preshadowbytes_read1per4gb_shadowsegue
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_preshadowbytes_read1per4gb_segue_shadowsegue
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_preshadowbytes_read1per4gb_segue_shadowsegue_spectre
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_bitscan_read_segue
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytes_bitscan_write_segue
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_watch
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_watch_shadowsegue
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytestag_floatzone
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytestag_floatzone_shadowsegue
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytestag_floatzone_virtpinned
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_shadowbytestag_floatzone_virtpinned_shadowsegue
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_mask_pdep
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_mask_pdep_segue
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_mask_pdep48
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_mask_pdep48_segue
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_mask_pext
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_mask_pext_segue
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_dummy
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_dummy2
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_dummy3
build: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_sswrite_sim

build_mpx: test_bytes.h
build_mpx: ../build_$(BUILD_TYPE_DIR)/image_change_quality
build_mpx: ../build_nosimd_$(BUILD_TYPE_DIR)/image_change_quality
build_mpx: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_guardpage
build_mpx: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_guardpage_segue
build_mpx: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_boundscheck
build_mpx: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_boundscheck_segue
build_mpx: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_mpx
build_mpx: ../build_nosimd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_mpx_segue

build_simdtest: test_bytes.h
build_simdtest: ../build_$(BUILD_TYPE_DIR)/image_change_quality
build_simdtest: ../build_nosimd_$(BUILD_TYPE_DIR)/image_change_quality
build_simdtest: ../build_arm32simd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_guardpage
build_simdtest: ../build_arm64simd_wasm_sa_$(BUILD_TYPE_DIR)/image_change_quality_wasm2c_guardpage

build_ipc_benchmark: test_bytes.h
build_ipc_benchmark: ../build_nosimd_release/image_change_quality
build_ipc_benchmark: ../build_nosimd_release/image_change_quality_rlbox_noop
build_ipc_benchmark: ../build_nosimd_wasm_release/image_change_quality_rlbox_wasm2c
build_ipc_benchmark: ../build_nosimd_wasm_sa_release/image_change_quality_wasm2c_guardpage
build_ipc_benchmark: ../build_nosimd_wasm_sa_release/image_change_quality_wasm2c_boundscheck
build_ipc_benchmark: ../build_nosimd_wasm_sa_release/image_change_quality_wasm2c_watch
build_ipc_benchmark: ../build_nosimd_release/image_change_quality_rlbox_ipc_shm
build_ipc_benchmark: ../build_nosimd_release/image_change_quality_socket_server
build_ipc_benchmark: ../build_nosimd_release/image_change_quality_socket_client

build_ipc_benchmark_minimal: test_bytes.h
build_ipc_benchmark_minimal: ../build_nosimd_release/image_change_quality_rlbox_ipc_shm
# build_ipc_benchmark_minimal: ../build_nosimd_release/image_change_quality_socket_client
build_ipc_benchmark_minimal: ../build_nosimd_release/image_change_quality_socket_server

test_rlbox:
	cd $(REPO_PATH)/build_$(BUILD_TYPE_DIR) && echo -n "Stock " && ./image_change_quality_rlbox_noop
	sleep 1
	cd $(REPO_PATH)/build_nosimd_$(BUILD_TYPE_DIR) && echo -n "Stock NoSIMD " && ./image_change_quality_rlbox_noop
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_$(BUILD_TYPE_DIR) && echo -n "Wasm2c " && ./image_change_quality_rlbox_wasm2c

test_simdtest:
	cd $(REPO_PATH)/build_$(BUILD_TYPE_DIR) && echo -n "Stock " && ./image_change_quality
	sleep 1
	cd $(REPO_PATH)/build_nosimd_$(BUILD_TYPE_DIR) && echo -n "Stock NoSIMD " && ./image_change_quality
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Guard pages " && ./image_change_quality_wasm2c_guardpage
	sleep 1
	cd $(REPO_PATH)/build_arm32simd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Guard pages ARM32 simd " && ./image_change_quality_wasm2c_guardpage
	sleep 1
	# cd $(REPO_PATH)/build_arm64simd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Guard pages ARM64 simd " && ./image_change_quality_wasm2c_guardpage
	# sleep 1

test_mpx:
	cd $(REPO_PATH)/build_$(BUILD_TYPE_DIR) && echo -n "Stock " && ./image_change_quality
	sleep 1
	cd $(REPO_PATH)/build_nosimd_$(BUILD_TYPE_DIR) && echo -n "Stock NoSIMD " && ./image_change_quality
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Guard pages " && ./image_change_quality_wasm2c_guardpage
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Guard pages segue " && ./image_change_quality_wasm2c_guardpage_segue
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Bounds checks " && ./image_change_quality_wasm2c_boundscheck
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Bounds checks segue " && ./image_change_quality_wasm2c_boundscheck_segue
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c MPX " && ./image_change_quality_wasm2c_mpx
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c MPX segue " && ./image_change_quality_wasm2c_mpx_segue
	sleep 1

test:
	cd $(REPO_PATH)/build_$(BUILD_TYPE_DIR) && echo -n "Stock " && ./image_change_quality
	sleep 1
	cd $(REPO_PATH)/build_nosimd_$(BUILD_TYPE_DIR) && echo -n "Stock NoSIMD " && ./image_change_quality
	sleep 1
	echo ""
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Guard pages " && ./image_change_quality_wasm2c_guardpage
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Guard pages segue " && ./image_change_quality_wasm2c_guardpage_segue
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Bounds checks " && ./image_change_quality_wasm2c_boundscheck
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Bounds checks segue " && ./image_change_quality_wasm2c_boundscheck_segue
	sleep 1
	echo ""
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c mask pdep48 " && ./image_change_quality_wasm2c_mask_pdep48
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c mask pdep48 segue " && ./image_change_quality_wasm2c_mask_pdep48_segue
	echo ""
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c shadowbytes bitscan read segue " && ./image_change_quality_wasm2c_shadowbytes_bitscan_read_segue
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c shadowbytes bitscan write segue " && ./image_change_quality_wasm2c_shadowbytes_bitscan_write_segue
	sleep 1

tests_old:
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasmtime Guard pages " && $(WASMTIME) run --allow-precompiled ./image_change_quality_wasmtime_guardpage.cwasm
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasmtime Bounds checks (nospectre) " && WASMTIME_NEW_CLI=0 $(WASMTIME) run --allow-precompiled --cranelift-set enable_heap_access_spectre_mitigation=false --static-memory-guard-size 0 --dynamic-memory-guard-size 0 ./image_change_quality_wasmtime_boundscheck.cwasm
	sleep 1
	echo ""
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Bounds checks asm " && ./image_change_quality_wasm2c_boundscheck_asm
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Bounds checks asm spectre " && ./image_change_quality_wasm2c_boundscheck_asm_spectre
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Bounds checks trap - Access len " && ./image_change_quality_wasm2c_boundscheck_trap_len
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Bounds checks trap - Access -1 " && ./image_change_quality_wasm2c_boundscheck_trap_neg1
	sleep 1
	echo ""
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow bytes - read1per4gb shadowsegue " && ./image_change_quality_wasm2c_shadowbytes_read1per4gb_shadowsegue
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow bytes - read4per4gb shadowsegue " && ./image_change_quality_wasm2c_shadowbytes_read4per4gb_shadowsegue
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow bytes - write1per4gb shadowsegue " && ./image_change_quality_wasm2c_shadowbytes_write1per4gb_shadowsegue
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow bytes - write4per4gb shadowsegue " && ./image_change_quality_wasm2c_shadowbytes_write4per4gb_shadowsegue
	sleep 1
	echo ""
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c PreShadow page read4per256mb  " && ./image_change_quality_wasm2c_preshadowpage_256mb_read
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c PreShadow page read4per256mb segue " && ./image_change_quality_wasm2c_preshadowpage_256mb_read_segue
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c PreShadow page read4per256mb shadowsegue " && ./image_change_quality_wasm2c_preshadowpage_256mb_read_shadowsegue
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c PreShadow page read4per256mb segue shadowsegue " && ./image_change_quality_wasm2c_preshadowpage_256mb_read_segue_shadowsegue
	sleep 1

	echo ""
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow page read " && ./image_change_quality_wasm2c_shadowpage_read
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow page read shadowsegue " && ./image_change_quality_wasm2c_shadowpage_read_shadowsegue
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow page read shadowsegue spectre " && ./image_change_quality_wasm2c_shadowpage_read_shadowsegue_spectre
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow page read shadowsegue spectre " && ./image_change_quality_wasm2c_shadowpage_1tb_read_shadowsegue
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow page read first byte " && ./image_change_quality_wasm2c_shadowpage_firstbyte_read
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow page write " && ./image_change_quality_wasm2c_shadowpage_write
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow page write shadowsegue " && ./image_change_quality_wasm2c_shadowpage_write_shadowsegue
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow page write first byte " && ./image_change_quality_wasm2c_shadowpage_firstbyte_write
	sleep 1
	echo ""
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c PreShadow bytes - read1per4gb " && ./image_change_quality_wasm2c_preshadowbytes_read1per4gb
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c PreShadow bytes - read1per4gb segue " && ./image_change_quality_wasm2c_preshadowbytes_read1per4gb_segue
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c PreShadow bytes - read1per4gb shadowsegue " && ./image_change_quality_wasm2c_preshadowbytes_read1per4gb_shadowsegue
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c PreShadow bytes - read1per4gb segue shadowsegue " && ./image_change_quality_wasm2c_preshadowbytes_read1per4gb_segue_shadowsegue
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c PreShadow bytes - read1per4gb segue shadowsegue spectre " && ./image_change_quality_wasm2c_preshadowbytes_read1per4gb_segue_shadowsegue_spectre
	sleep 1
	echo ""
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow bytes - read1per64kb " && ./image_change_quality_wasm2c_shadowbytes_read1per64kb
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow bytes - read1per16mb " && ./image_change_quality_wasm2c_shadowbytes_read1per16mb
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow bytes - write1per64kb " && ./image_change_quality_wasm2c_shadowbytes_write1per64kb
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow bytes - write1per16mb " && ./image_change_quality_wasm2c_shadowbytes_write1per16mb
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow bytes - read1per4gb " && ./image_change_quality_wasm2c_shadowbytes_read1per4gb
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow bytes - read4per4gb " && ./image_change_quality_wasm2c_shadowbytes_read4per4gb
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow bytes - write1per4gb " && ./image_change_quality_wasm2c_shadowbytes_write1per4gb
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow bytes - write4per4gb " && ./image_change_quality_wasm2c_shadowbytes_write4per4gb
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow bytes - read1per64kb shadowsegue " && ./image_change_quality_wasm2c_shadowbytes_read1per64kb_shadowsegue
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow bytes - read1per16mb shadowsegue " && ./image_change_quality_wasm2c_shadowbytes_read1per16mb_shadowsegue
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow bytes - write1per64kb shadowsegue " && ./image_change_quality_wasm2c_shadowbytes_write1per64kb_shadowsegue
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow bytes - write1per16mb shadowsegue " && ./image_change_quality_wasm2c_shadowbytes_write1per16mb_shadowsegue
	sleep 1
	echo ""
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c watch " && ./image_change_quality_wasm2c_watch
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c watch shadowsegue " && ./image_change_quality_wasm2c_watch_shadowsegue
	sleep 1
	echo ""
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow bytes tag (floatzone) virtpinned " && ./image_change_quality_wasm2c_shadowbytestag_floatzone_virtpinned
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow bytes tag (floatzone) virtpinned shadowsegue " && ./image_change_quality_wasm2c_shadowbytestag_floatzone_virtpinned_shadowsegue
	sleep 1
	echo ""
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c mask pdep " && ./image_change_quality_wasm2c_mask_pdep
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c mask pdep segue " && ./image_change_quality_wasm2c_mask_pdep_segue
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c mask pext " && ./image_change_quality_wasm2c_mask_pext
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c mask pext segue " && ./image_change_quality_wasm2c_mask_pext_segue
	sleep 1
	echo ""
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Bounds check dummy " && ./image_change_quality_wasm2c_dummy
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Bounds check dummy2 " && ./image_change_quality_wasm2c_dummy2
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Bounds check dummy2 " && ./image_change_quality_wasm2c_dummy3
	echo ""
	echo "Spectre protections tests"
	echo "-------------------------"
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasmtime Bounds checks spectre " && $(WASMTIME) run --allow-precompiled --cranelift-set enable_heap_access_spectre_mitigation=true --static-memory-guard-size 0 --dynamic-memory-guard-size 0 ./image_change_quality_wasmtime_boundscheck_spectre.cwasm
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Bounds checks spectre " && ./image_change_quality_wasm2c_boundscheck_spectre
	sleep 1
	echo ""
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow bytes - read1per4gb spectre " && ./image_change_quality_wasm2c_shadowbytes_read1per4gb_spectre
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow bytes - read4per4gb spectre " && ./image_change_quality_wasm2c_shadowbytes_read4per4gb_spectre
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow bytes - read1per4gb shadowsegue spectre " && ./image_change_quality_wasm2c_shadowbytes_read1per4gb_shadowsegue_spectre
	sleep 1
	cd $(REPO_PATH)/build_nosimd_wasm_sa_$(BUILD_TYPE_DIR) && echo -n "Wasm2c Shadow bytes - read4per4gb shadowsegue spectre " && ./image_change_quality_wasm2c_shadowbytes_read4per4gb_shadowsegue_spectre
	sleep 1

clean:
	rm -f test_out.jpeg
	rm -f test_bytes.h
	rm -rf ../build_*/
	rm -rf $(WASM2C_SRC_PATH)/build_*