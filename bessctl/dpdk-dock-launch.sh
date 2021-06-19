#numactl -m 0 docker run --privileged -it --rm --name dpdk-test0 -v /dev/hugepages:/dev/hugepages -v /tmp/bessd:/tmp/bessd ubuntu:dpdk-numa /root/dpdk/build/app/dpdk-testpmd --in-memory --no-pci -m 256 -l 0,1-1 --vdev=virtio_user0,path=/tmp/bessd/vhost_user0_0.sock,queues=1  -- -i --txd=1024 --rxd=1024 --txq=1 --rxq=1 --total-num-mbufs=65536


numactl -m 0 docker run --privileged -it --rm --name dpdk-test0 -v /dev/hugepages:/dev/hugepages -v /tmp/bessd:/tmp/bessd ubuntu:dpdk-numa /root/dpdk/build/app/dpdk-testpmd --in-memory --no-pci -m 1024 -l 10-11 --vdev=virtio_user0,path=/tmp/bessd/vhost_user0_0.sock,queues=1 --vdev=virtio_user1,path=/tmp/bessd/vhost_user0_1.sock,queues=1 -- -i --txd=1024 --rxd=1024 --txq=1 --rxq=1  --total-num-mbufs=65536 
