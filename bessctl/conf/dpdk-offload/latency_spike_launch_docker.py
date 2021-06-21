from __future__ import print_function

import subprocess
import shlex
import time

NUM_VMS = 1
VERBOSE = 1

CORE_LIST_DICTIONARY = {1:[11], 2:[34,11], 3:[10,34,11],
			 4:[10,34,11,35],
			 6:[9,33,10,34,11,35],
			 #6:[32,9,33,10,34,11],
			 8:[8,32,9,33,10,34,11,35],
			 9:[7,31,8,32,9,33,10,34,11], 12:[6,30,7,31,8,32,9,33,10,34,11,35]}
PMD_CORE_LIST=CORE_LIST_DICTIONARY[NUM_VMS]

CONTAINER_NAME='dpdk-test'
IMAGE='ubuntu:dpdk-numa'
#IMAGE='ubuntu:dpdk-numa'

def launch(cid, corenum):
    cmd = "numactl -m 0 " \
            "docker run --privileged -i --rm --name {} -v /dev/hugepages:/dev/hugepages " \
            "-v /tmp/bessd:/tmp/bessd {} " \
            "/root/dpdk/build/app/dpdk-testpmd --in-memory --no-pci -m 1024 -l 0,{} " \
            "--vdev=virtio_user0,path=/tmp/bessd/vhost_user{}_0.sock,queues=1 " \
            "--vdev=virtio_user1,path=/tmp/bessd/vhost_user{}_1.sock,queues=1 " \
            "-- -i --txd=1024 --rxd=1024 --txq=1 --rxq=1  --total-num-mbufs=65536".format(
                "{}{:02d}".format(CONTAINER_NAME, cid),
                IMAGE,
                PMD_CORE_LIST[cid],
                cid,
                cid
            )

    if VERBOSE:
        out = None  # to screen
    else:
        out = subprocess.PIPE

    print(cmd)
    proc = subprocess.Popen(shlex.split(cmd), stdin=subprocess.PIPE,
                            stdout=out, stderr=subprocess.STDOUT,
                            universal_newlines=True)
    # print('set fwd {}'.format(FWD_MODE), file=proc.stdin)
    # print('set txpkts {}'.format(PKT_SIZE), file=proc.stdin)
    print('start', file=proc.stdin, flush=True)
    return proc

def kill(cid):
    print('Terminating container {} '.format(cid))

    cmd = 'docker kill {name}'.format(name="{}{:02d}".format(CONTAINER_NAME, cid))

    if VERBOSE:
        print(cmd)

    try:
        proc = subprocess.check_call(shlex.split(cmd), stdout=subprocess.PIPE)
    except subprocess.CalledProcessError:
        pass


if __name__ == '__main__':

    num_containers = NUM_VMS 
    try:
        procs = [launch(i, 2+i) for i in range(NUM_VMS)]

        print('Press Ctrl+C to terminate all containers')
        while True:
            time.sleep(100)
    except KeyboardInterrupt:
        pass
    finally:
        for cid in range(num_containers):
            kill(cid)
