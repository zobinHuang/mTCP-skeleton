cmd_/home/zobin/projects/mTCP-app/third_party/dpdk-iface-kmod/modules.order := {   echo /home/zobin/projects/mTCP-app/third_party/dpdk-iface-kmod/dpdk_iface.ko; :; } | awk '!x[$$0]++' - > /home/zobin/projects/mTCP-app/third_party/dpdk-iface-kmod/modules.order
