## TSC-VEE
TSC-VEE is a trustzone-based smart contract virtual execution environment. 


### Description

This is the code repository for our paper "TSC-VEE: A Trustzone-based Smart Contract Virtual Execution Environment". (TPDS-2022-09-0589.R1, under review)

The currently published version has been verified on the ERC20 contract(``/exp/ERC20.sol``), involving 47 of the 150 instructions. The verification of other instructions is in progress. If there is any bug, please contact us.

In addition, as far as we know, the version ``on REE/`` is the first C language evm implementation. We will modify it to conform to the ``evmc`` interface and try to contribute it to the ethereum community.


### Environment

For execution 
- Development board: Raspberry pi 3B+ (ARM Cortex A53 * 4, 1GB RAM)
- REE OS: Linux-for-arm 4.14.56
- TEE OS: OP-TEE 3.8.0
- Simulator: QEMU v8
 
For compilation 
- Host OS for compiling OP-TEE: Ubuntu-22.04, 64bit, x86_64
- GCC version：11.3.0
- toolchain: aarch64-linux-gnu-gcc 10.2.1

### Setup

1. dependencies

```
$ sudo apt install adb acpica-tools autoconf automake bc bison build-essential \
ccache cscope curl device-tree-compiler e2tools expect fastboot flex ftp-upload \
gdisk libattr1-dev libcap-dev libfdt-dev libftdi-dev libglib2.0-dev libgmp3-dev \
libhidapi-dev libmpc-dev libncurses5-dev libpixman-1-dev libslirp-dev libssl-dev \
libtool libusb-1.0-0-dev make mtools netcat ninja-build python3-cryptography \
python3-pip python3-pyelftools python3-serial  python-is-python3 rsync swig unzip \
uuid-dev xdg-utils xterm xz-utils zlib1g-dev
```

2. get source code

```
$ mkdir -p optee
$ cd optee

# for qemu v8
$ repo init -u https://github.com/OP-TEE/manifest.git -m qemu_v8.xml -b 3.8.0
# for Raspberry pi 3B+
$ repo init -u https://github.com/OP-TEE/manifest.git -m rpi3.xml -b 3.8.0

$ repo sync -j4 --no-clone-bundle
```

3. get toolchain
```
$ cd optee/build
$ make -j2 toolchains
```

4. build optee
```
$ make -j `nproc`
```

5. run

For QEMU v8:
- Place the trust applications(TAs) in the ``optee-examples/`` folder
- Execute ``make run`` in the build folder
- Run the TA.

For Raspberry pi 3B+:
- Place the TAs in the ``optee-examples/`` folder
- Execute ``make optee-examples-common`` in the build folder
- Copy the ``${UUID}.ta`` file to the ``/lib/optee_armtz/`` folder on the development board through the U-disk
- Run the TA.

6. secure memort adjustment
- ``CFG_TZDRAM_SIZE(optee_os/core/arch/arm/plat-rpi3/conf.mk)``: the size of physical memory corresponding to the secure memory
- ``PGT_CACHE_SIZE(optee_os/core/include/mm/pgt_cache.h)``: the number of page tables for virtual memory

### File description
- ``evmone/``: [evmone](https://github.com/ethereum/evmone) runs on REE
- ``on REE/``: the C version implementation for evmone, runs on REE
- ``TSC-VEE/``: a trusted execution environment for Solidity smart contract, runs on TEE
- ``exp/``: the experimental data of our paper

### Contact

After the paper is accepted, we will add a link and the citation to our paper. 

If you have any questions, please contact us.

Zhaolong Jian: jianzhaolong@mail.nankai.edu.cn
