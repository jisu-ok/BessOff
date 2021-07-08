# For single-node systems
#sudo sysctl vm.nr_hugepages=1024

# For multi-node (NUMA) systems - 2KB hugepage
# echo 1024 | sudo tee /sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages
# echo 1024 | sudo tee /sys/devices/system/node/node1/hugepages/hugepages-2048kB/nr_hugepages

# For multi-node (NUMA) systems - 1GB hugepage
echo 8 | sudo tee /sys/devices/system/node/node0/hugepages/hugepages-1048576kB/nr_hugepages
echo 8 | sudo tee /sys/devices/system/node/node1/hugepages/hugepages-1048576kB/nr_hugepages


# Enable no-IOMMU mode
sudo modprobe vfio-pci
echo 1 | sudo tee /sys/module/vfio/parameters/enable_unsafe_noiommu_mode

sudo bin/dpdk-devbind.py --status

