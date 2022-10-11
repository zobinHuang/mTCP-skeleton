#!/bin/sh

# =========== we only need to modify ================
host="epoll_server"
binary_name="sample_mtcp_epoll_server"
# ===================================================

binary_file_relative_dir="../bin/."
binary_file_dir=$(cd "$(dirname "$binary_file_relative_dir")"; pwd)
binary_file="$binary_file_dir/$binary_name"

build_path="./build"

conf_files_relative_dir="./conf/$host/."
conf_files_dir=$(cd "$(dirname "$conf_files_relative_dir")"; pwd)
conf_files="$conf_files_dir/*"

echo $conf_files

# make sure the binary exist
if [ ! -f "$binary_file" ]; then
echo could not found binary file under $binary_file, please compile the project first!
exit -1
fi

# copy binary file to build subdirectory
if [ ! -d "$build_path" ]; then
mkdir $build_path
fi
cd $build_path

if [ ! -d "$host" ]; then
mkdir $host
fi

cd $host
cp $binary_file ./binary

# copy config file to build subdirectory
cp -r $conf_files .

# run the binary
./binary -f ./main.conf -n 8