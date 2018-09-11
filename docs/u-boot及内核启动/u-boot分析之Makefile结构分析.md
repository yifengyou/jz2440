# u-boot分析之Makefile结构分析

Linux项目分析工程结构最好的办法就是分析Makefile

![1536408928029.png](image/1536408928029.png)

Linux惯用套路，先配置再编译

![1536408960050.png](image/1536408960050.png)


## 分析配置过程

```
make 100ask24x0_config
```

![1536409170828.png](image/1536409170828.png)

在Makefile中肯定是个目标

```
MKCONFIG	:= $(SRCTREE)/mkconfig
export MKCONFIG

...

100ask24x0_config	:	unconfig
	@$(MKCONFIG) $(@:_config=) arm arm920t 100ask24x0 NULL s3c24x0

```

这里没有几个Makefile的知识

1. ‘@’开头

** @表示在执行命令时不输出命令本身，只输出结果。**这个符串通常用在“规则”行中，表示不显示命令本身，而只显示它的结果，例如Makefile中的内容为：
```
DIR_OBJ=./obj
CMD_MKOBJDIR=if [ -d ${DIR_OBJ} ]; then exit 0; else mkdir ${DIR_OBJ}; fi

mkobjdir:
	@${CMD_MKOBJDIR}
```
命令行执行如下：
```
make mkobjdir
```
此时不会显示在命令行不会显示出if [ -d ${DIR_OBJ} ]; then exit 0; else mkdir ${DIR_OBJ}; fi，但如果规则行的TAB后没有以@开头，则会显示，不信可以试试。

2. Makefile自定义变量

	- Makefile自定义的变量引用方式是${VarName}
	- Makefile中引用shell环境变量方式是 $${VarName}

3. $(@:_config=)

	- ```$(@:_config=) ```其中@表示目标100ask24x0_config，整个表示把目标中的_config替换成空
	- 替换之后变量$@的值就变成了临时变量的100ask24x0
	- makefile的变量替换规则，**$(VAR:A=B)表示替换变量$VAR中的A为B**，即把$@中的_config替换为空变成100ask24x0
	- **$@ 目标文件，$^ 所有的依赖文件，$< 第一个依赖文件。**



```
MKCONFIG	:= $(SRCTREE)/mkconfig
export MKCONFIG

...

100ask24x0_config	:	unconfig
	@$(MKCONFIG) $(@:_config=) arm arm920t 100ask24x0 NULL s3c24x0

```
这句话本质就是执行命令
```
mkconfig 100ask24x0  arm arm920t 100ask24x0 NULL s3c24x0
```
所谓配置本质就是执行了这个命令，要具体了解配置干了啥，就得去看mkconfig命令的实现。
只有八十来行，直接贴出阿里。是个实实在在的shell脚本文件

```
     1	#!/bin/sh -e
     2
     3	# Script to create header files and links to configure
     4	# U-Boot for a specific board.
     5	#
     6	# Parameters:  Target  Architecture  CPU  Board [VENDOR] [SOC]
     7	#
     8	# (C) 2002-2006 DENX Software Engineering, Wolfgang Denk <wd@denx.de>
     9	#
    10
    11	APPEND=no	# Default: Create new config file
    12	BOARD_NAME=""	# Name to print in make output
    13
    14	while [ $# -gt 0 ] ; do  # $#表示参数个数 gt 表示greater than，大于
    15		case "$1" in
    16		--) shift ; break ;;
    17		-a) shift ; APPEND=yes ;;
    18		-n) shift ; BOARD_NAME="${1%%_config}" ; shift ;;
    19		*)  break ;;
    20		esac
    21	done
    22
    23	[ "${BOARD_NAME}" ] || BOARD_NAME="$1" # 如果参数定义了执行赋值，否则不动
    24
    25	[ $# -lt 4 ] && exit 1 # 参数至少有[4,6]个
    26	[ $# -gt 6 ] && exit 1
    27
    28	echo "Configuring for ${BOARD_NAME} board..."
    29
    30	#
    31	# Create link to architecture specific headers
    32	#
    33	if [ "$SRCTREE" != "$OBJTREE" ] ; then    
    34		mkdir -p ${OBJTREE}/include      # SRCTREE := $(CURDIR)
    35		mkdir -p ${OBJTREE}/include2     # OBJTREE := $(if $(BUILD_DIR),$(BUILD_DIR),$(CURDIR)) 三目运算，如果没有定义$(BUILD_DIR)那就把$(BUILD_DIR)赋值为$(CURDIR)，否则不动
    36		cd ${OBJTREE}/include2
    37		rm -f asm
    38		ln -s ${SRCTREE}/include/asm-$2 asm
    39		LNPREFIX="../../include2/asm/"
    40		cd ../include
    41		rm -rf asm-$2
    42		rm -f asm
    43		mkdir asm-$2
    44		ln -s asm-$2 asm
    45	else
    46		cd ./include
    47		rm -f asm
    48		ln -s asm-$2 asm # 建立链接文件 $2 = arm
    49	fi
    50
    51	rm -f asm-$2/arch # 移除asm-arm目录
    52  # mkconfig 100ask24x0  arm arm920t 100ask24x0 NULL s3c24x0
    53	if [ -z "$6" -o "$6" = "NULL" ] ; then
    54		ln -s ${LNPREFIX}arch-$3 asm-$2/arch
    55	else
    56		ln -s ${LNPREFIX}arch-$6 asm-$2/arch # ln -s arch-s3c24x0 asm-arm/arch
    57	fi
    58   
    59	if [ "$2" = "arm" ] ; then
    60		rm -f asm-$2/proc
    61		ln -s ${LNPREFIX}proc-armv asm-$2/proc # ln -s proc-armv asm-arm/proc
    62	fi
    63
    64	#
    65	# Create include file for Make 生成配置文件
    66	# mkconfig 100ask24x0  arm arm920t 100ask24x0 NULL s3c24x0
    67	echo "ARCH   = $2" >  config.mk
    68	echo "CPU    = $3" >> config.mk
    69	echo "BOARD  = $4" >> config.mk
    70
    71	[ "$5" ] && [ "$5" != "NULL" ] && echo "VENDOR = $5" >> config.mk
    72
    73	[ "$6" ] && [ "$6" != "NULL" ] && echo "SOC    = $6" >> config.mk
    74
    75	#
    76	# Create board specific header file
    77	#
    78	if [ "$APPEND" = "yes" ]	# Append to existing config file
    79	then
    80		echo >> config.h
    81	else
    82		> config.h		# Create new config file
    83	fi
    84	echo "/* Automatically generated - do not edit */" >>config.h
    85	echo "#include <configs/$1.h>" >>config.h
    86
    87	exit 0
```

![1536546067733.png](image/1536546067733.png)

![1536546142328.png](image/1536546142328.png)

配置文件，uboot支持哪些命令就是在这里修改



## 参考资料

Makefile基本用法想看Github博客<http://github.com/yifengyou/learn-makefile>

```
视频里面讲的其实也不是很详细，只是点到为止，没有做梳理总结，你需要数量掌握Makefile的
身为一个Linux程序员尤其是搞嵌入式的，如果Makefile不熟悉，呵呵
知识点总结人可没功夫替你弄好，你需要自己来，把别人的东西彻底消化
```
