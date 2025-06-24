#!/bin/bash

set -e

# Arguments: $1 - architecture for gcc
#            $2 - ABI

SCRIPT_DIR=$(dirname $0)
echo "SCRIPT_DIR=$SCRIPT_DIR"

cd $SCRIPT_DIR

cd ../Toolchain

INSTALL_PATH=$PWD/GCC/$1

if [ ! -d $PWD/GCC ]; then
    echo "Toolchain/GCC directory doesnt exist.  Creating it"
    mkdir GCC
fi

#Build GCC
echo "Downloading GCC"
if [ -d $PWD/riscv-gnu-toolchain ]; then
    echo "GCC source already downloaded. Cleaning it up"
    cd riscv-gnu-toolchain
    make clean
else
    git clone https://github.com/riscv-collab/riscv-gnu-toolchain.git
    cd riscv-gnu-toolchain
fi

echo "making GCC for $1 $2"
./configure --prefix=$INSTALL_PATH --with-arch=$1 --with-abi=$2
make -j$(nproc)
