#!/bin/bash

set -e
begin=`date`
#patch -p1 < ./u-boot-1.1.6_jz2440.patch
make   100ask24x0_config
make -j$(cat /proc/cpuinfo |grep processor|wc -l)

if [ $? -eq 0 ];then
	echo "Success"
	ls -alh u-boot.bin
else
	echo "Failed"
	exit 1
fi
end=`date`

echo $begin " start time"
echo $end " end time"
