#!/bin/bash

set -e
patch -p1 < ./linux-2.6.22.6_jz2440_v2v3.patch
echo "patch done"

cp config_ok .config
echo "config done"

make uImage -j$(cat /proc/cpuinfo |grep processor|wc -l)
echo "make done"
