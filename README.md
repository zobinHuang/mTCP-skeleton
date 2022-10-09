# mTCP-skeleton

This repo provides skeleton application based on mTCP-DPDK 21.11, sample applications are also provided to demonstrate the code flow.

## Preparation

### 0. Install dependencies

```bash
sudo apt-get install automake
```

### 1. Build, intsall and config DPDK 21.11 

Make sure the header and library of DPDK 21.11 are installed correctly in your machine [ See <a href="https://zobinhuang.github.io/sec_learning/Tech_System_And_Network/DPDK_1_Installation/">Installation of DPDK 21.11</a> for more details ];

### 2. Compiling mTCP based on DPDK 21.11

To build mTCP, run:

```bash
# enter the mtcp subdirectory
cd third_party/mtcp

# confige build process
./configure --with-dpdk-lib=$RTE_SDK/$RTE_TARGET

# build
make
```

We have modified some code of mTCP, see <a href="https://zobinhuang.github.io/sec_learning/Tech_System_And_Network/DPDK_mTCP_Compiled/index.html">Guide to compile mTCP with DPDK 21.11</a> for more details about making mTCP capable with DPDK 21.11.

To clean all build files of mtcp, run:

```bash
make distclean
```

### 3. Build `libevent-2.1.10`

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

### 4. Build `dpdk-iface-kmod` kernel module

```bash
# move to the root path of dpdk-iface-kmod
cd third_party/dpdk-iface-kmod

# add environment variable
export RTE_SDK=/usr/src/dpdk-21.11

# build
make

# install kernel module
sudo insmod dpdk_iface.ko

# run the binary for testing
sudo ./dpdk_iface_main

# config interface ip address (for Intel NIC)
sudo ifconfig dpdk0 10.0.20.2 netmask 255.255.255.0 up

# delete interface ip (for Intel NIC)
sudo ip addr flush dev dpdk0
```

### 5. Build this project

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

### 6. Configure file for mtcp

We need to setup the configuration for routin and arp for MTCP

```bash
# get into bin
cd bin

# create config directory
mkdir config

# create config file
touch route.conf
touch arp.conf
```

Inside `route.conf`, we add:

```bash
# This file is routing table example of our testbed machine
# Copy this file to route.conf and give appropriate routes
# Please save this file as config/route.conf. Put the config/
# directory in the same directory where the binary lies.
#
# (Destination address)/(Prefix) (Device name)
#
#
# Add entry for default gateway route as:
# w.x.y.z/0 dpdk0
# Always put the default gateway route as the last entry.
# Make sure that the mask (Prefix) is set to 0. For example,
# if the default gateway IP address is 10.0.0.10, then the
# entry will be:
# 10.0.0.10/0 dpdk0
#

ROUTES 1
#192.168.3.1/24 dpdk0
10.0.20.0/24 0000:06:11.0
#10.0.0.2/24 ens38f1np1
```

Inside `arp.conf`, we add:

```bash
# This file is to configure static arp tables.
# Rename this file to arp.conf and set the appropriate values.
# Please save this file as config/arp.conf. Put the config/
# directory in the same directory where the binary lies.
#
# (Destination IP address/IP_prefix) (Destination MAC address)

ARP_ENTRY 2
#192.168.3.1/24 ac:1f:6b:b3:17:5f
10.0.20.1/24 b8:59:9f:c6:50:bb
10.0.20.2/24 00:1b:21:bc:26:84
```

## Run

Take `sample_mtcp_epoll_server` sample as an example, to run it we type:

```bash
# get into the binary path
cd bin

# run epoll server
sudo ./sample_mtcp_epoll_server -f ../conf/epserver.conf -n 8
```