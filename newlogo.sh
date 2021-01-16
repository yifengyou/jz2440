#!/bin/bash

set -e
pngtopnm tencentos.png > tencentos.pnm
pnmquant 224 tencentos.pnm > logo.pnm
pnmtoplainpnm logo.pnm > logo_linux_clut224.ppm
rm logo.pnm tencentos.pnm
echo "Done!"

if [ -e drivers/video/logo/logo_linux_cult224.ppm ];then
	cp drivers/video/logo/logo_linux_cult224.ppm drivers/video/logo/logo_linux_cult224.ppm.bak
	echo "backup old logo_linux_cult224.ppm"
fi
cp logo_linux_clut224.ppm drivers/video/logo/logo_linux_cult224.ppm
echo "cp new logo_linux_cult224.ppm"

find ./ -name "logo_linux_clut224.o" |xargs -i rm {} || true
echo "clean logo_linux_clut224.o"

make uImage -j$(cat /proc/cpuinfo|grep processor|wc -l)
echo "All done!"
