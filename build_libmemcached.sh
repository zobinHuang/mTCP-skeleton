export mtcp_skeleton_root=$(pwd)

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