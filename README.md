# mTCP-app

This repo contains a epoll server/client communication program built based on mTCP with the support of DPDK 21.11.

## Preparation

1. Make sure the header and library of DPDK 21.11 are installed correctly in your machine [ See <a href="https://zobinhuang.github.io/sec_learning/Tech_System_And_Network/DPDK_1_Installation/">Installation of DPDK 21.11</a> for more details ];

2. Compiling mTCP based on DPDK 21.11, this repo provides the pre-built library under `lib`, you can compile by your self and put the `libmtcp.a` file under `lib` [ See <a href="https://zobinhuang.github.io/sec_learning/Tech_System_And_Network/DPDK_mTCP_Compiled/index.html">Guide to compile mTCP with DPDK 21.11</a> for more details about compiling mTCP with DPDK 21.11]; 

3. Build `libevent-2.1.10`

```bash
# enter the libevent subdirectory
cd third_party/libevent

# make build subdirectory and enter it
mkdir build && cd build

# run cmake to generate makefile
cmake ..

# run make to build libevent
make

# return to the root path of the project
cd ../..
```

## Build this project

To Build this project, run:

```bash
# create a build library
mkdir build
cd build

# run cmake
cmake ..

# build project
make
```

After that the executable will be build and put under `bin`