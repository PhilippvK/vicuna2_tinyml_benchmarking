#!/bin/bash

#Top level script to setup dependencies/toolchain.  Each has a helper script to setup which is called here
#
#Necessary dependencies:
#   -verilator:  verilator is built from source.
#   -llvm 18  :  LLVM 18 is built from source
#   -GCC      :  RISCV GCC headers are needed for each supported architecture.  TODO: add these to the sync-and-share system used for muRISCV-nn.  currently built from source
#   -spike    :  the riscv-isa-sim is used to validate vicuna results.  built from source

set -e



######
# make Toolchain directory
######

SCRIPT_DIR=$(dirname $(readlink -f $0))
echo "SCRIPT_DIR=$SCRIPT_DIR"

cd $SCRIPT_DIR

cd ..
if [ -d $PWD/Toolchain ]; then
    echo "Toolchain Directory already exists"
else
    echo "Making Toolchain Directory"
    mkdir Toolchain
fi
cd -
######
#   Verilator setup
######

./build_verilator.sh

######
#   llvm setup
######

./build_llvm.sh


######
#   GCC setup
######

./build_gcc.sh rv32im ilp32
./build_gcc.sh rv32imf ilp32f
./build_gcc.sh rv32imf_zfh ilp32f
./build_gcc.sh rv32im_zve32x ilp32
./build_gcc.sh rv32imf_zve32f ilp32f
./build_gcc.sh rv32imf_zfh_zve32f_zvfh ilp32f

######
#   tflm setup
######

./download_tflm.sh
