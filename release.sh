#!/bin/bash

set -e

[ -e /work/nfs_root ] || mkdir -p /work/nfs_root

cd linux-2.6.22.6
if [ -f arch/arm/boot/uImage ];then
	ls -alh arch/arm/boot/uImage
	md5sum arch/arm/boot/uImage
	cp arch/arm/boot/uImage /work/nfs_root/
	echo "replace /work/nfs_root/uImage"
	md5sum /work/nfs_root/uImage
	ls -alh /work/nfs_root/uImage
else
	echo "uImage not found!"
fi
