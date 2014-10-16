#!/bin/bash -ex

# target1=$PWD/SRC/build-tempo-target-debug/tempo
# target2=$PWD/SRC/build-enroll-target-debug/enroll
# scp -Cp $target1 $target2 root@192.168.1.200:"/usr/local/bin/"

# target1=$PWD/SRC/build-tempo-target-debug/tempo
# target2=$PWD/SRC/build-enrollment-target-debug/enrollment
# target3=$PWD/SRC/tempo/Resources/
# target4=$PWD/SRC/build-enroll-target-debug/enroll
# scp -Cpr $target1 $target2 root@192.168.1.252:"/usr/local/bin/"

target1=$PWD/FIRMWARE/firmware.tar.gz
target2=$PWD/FIRMWARE/firmware.tar.gz.md5
# target3=$PWD/FIRMWARE/app.ini
# target4=/home/jvillasante/Desktop/iengine.db
scp -Cpr $target1 $target2 root@192.168.1.252:"/usr/local/"
# scp -Cpr $target3 $target4 root@192.168.1.252:"/mnt/jffs2/"

