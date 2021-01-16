#!/bin/bash

[ -e gcc-3.4.5-glibc-2.3.6 ] && rm -rf gcc-3.4.5-glibc-2.3.6

echo "unpack..."
tar -xf arm-linux-gcc-3.4.5-glibc-2.3.6.tar.bz2
#tar -xvf arm-linux-gcc-3.4.5-glibc-2.3.6.tar.bz2

ln -svf `pwd`/gcc-3.4.5-glibc-2.3.6 /gcc

if cat ~/.bashrc |grep "/gcc";then
	echo "find /gcc/bin/ config"
else
	echo "export PATH=\$PATH:/gcc/bin" >> ~/.bashrc
	echo "add confid to ~/.bashrc"
fi
echo "Please run source ~/.bashrc"
echo "All done!"
