export mtcp_skeleton_root=$(pwd)

cd $mtcp_skeleton_root/third_party

# build mtcp
echo "Build MTCP..."
cd $mtcp_skeleton_root/third_party/mtcp
./configure --with-dpdk-lib=$RTE_SDK/$RTE_TARGET
make

# install mtcp
cp $mtcp_skeleton_root/third_party/mtcp/mtcp/include/mtcp*.h /usr/local/include/
cp $mtcp_skeleton_root/third_party/mtcp/mtcp/lib/libmtcp.a /usr/local/lib/libmtcp.a

# build dpdk-iface
echo "Register network interface dpdk0"
cd $mtcp_skeleton_root/third_party/dpdk-iface-kmod
export RTE_SDK=/usr/src/dpdk-21.11
make
insmod dpdk_iface.ko
./dpdk_iface_main
ifconfig dpdk0 10.0.109.3 netmask 255.255.255.0 up

# build libmemcached
echo "Build libmemcached..."
cd $mtcp_skeleton_root/third_party/libmemcached
if [ -d "$mtcp_skeleton_root/third_party/libmemcached/build-libmemcached" ]
then
    rm -rf $mtcp_skeleton_root/third_party/libmemcached/build-libmemcached
fi
mkdir build-libmemcached && cd build-libmemcached
cmake ..
make
make install
ldconfig