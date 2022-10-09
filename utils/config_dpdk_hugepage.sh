# reserve hugepage
echo 1024 > /sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages
echo 1 > /sys/devices/system/node/node0/hugepages/hugepages-1048576kB/nr_hugepages

# mount hugepage
mkdir /mnt/huge
mount -t hugetlbfs pagesize=2MB /mnt/huge
mount -t hugetlbfs pagesize=1GB /mnt/huge