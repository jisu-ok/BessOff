from __future__ import print_function

import subprocess
import shlex
import time

NUM_VMS = 3
NUM_VMS_CMPT = 2
VERBOSE = 1

CORE_LIST=[7,8,9,10,11,12,19,20,21,22,23,24]
CONTAINER_NAME='dpdk-test'
IMAGE='ubuntu:dpdk-firewall'

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
                corenum,
                cid,
                cid
            )
    print(cmd)
    if VERBOSE:
        out = None  # to screen
    else:
        out = subprocess.PIPE

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

    num_containers = NUM_VMS + NUM_VMS_CMPT

    try:
        procs = [launch(i, 9+i) for i in range(NUM_VMS)]
        procs = [launch(NUM_VMS+i, 28+i) for i in range(NUM_VMS_CMPT)]

        print('Press Ctrl+C to terminate all containers')
        while True:
            time.sleep(100)
    except KeyboardInterrupt:
        pass
    finally:
        for cid in range(num_containers):
            kill(cid)
