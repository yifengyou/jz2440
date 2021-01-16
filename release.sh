#!/bin/bash

set -e

[ -e /work/nfs_root ] || mkdir -p /work/nfs_root

if [ -f arch/arm/boot/uImage ];then
	cp arch/arm/boot/uImage /work/nfs_root/
	ls -alh /work/nfs_root/uImage
else
	echo "uImage not found!"
fi
