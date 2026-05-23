#!/bin/sh

set -e

JOBS=$(nproc)

export ARCH=riscv
export CROSS_COMPILE=riscv64-linux-gnu-

make -C opensbi \
    CROSS_COMPILE=${CROSS_COMPILE} \
    PLATFORM=generic \
    -j${JOBS}

make -C linux \
    ARCH=${ARCH} \
    CROSS_COMPILE=${CROSS_COMPILE} \
    defconfig

make -C linux \
    ARCH=${ARCH} \
    CROSS_COMPILE=${CROSS_COMPILE} \
    -j${JOBS}

mkdir -p output

${CROSS_COMPILE}gcc -O2 -g \
    tests/test_random.c \
    -o output/test_random

${CROSS_COMPILE}gcc -O2 -g \
    tests/test_cst_mmode.c \
    -o output/test_csr_mmode

${CROSS_COMPILE}gcc -O2 -g \
    tests/test_cpu_info.c \
    -o output/test_cpu_info
