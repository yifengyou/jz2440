#!/bin/bash

set -e
[ -d linux-2.6.22.6 ] && rm -rf linux-2.6.22.6
echo "clean done"
tar -xf linux-2.6.22.6.tar.bz2
echo "unpack done"
cd linux-2.6.22.6
patch -p1 < ../linux-2.6.22.6_jz2440_v2v3.patch
echo "patch done"

cp config_ok .config
echo "config done"

make uImage -j$(cat /proc/cpuinfo |grep processor|wc -l)
echo "make done"
echo "oem script"
