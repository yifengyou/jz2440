<!-- TOC depthFrom:1 depthTo:6 withLinks:1 updateOnSave:1 orderedList:0 -->

- [u-boot分析之编译体验](#u-boot分析之编译体验)
	- [u-boot打补丁](#u-boot打补丁)
	- [u-boot编译](#u-boot编译)
	- [烧写u-boot编译](#烧写u-boot编译)
	- [u-boot存在的意义？](#u-boot存在的意义)
	- [u-boot-1.1.6目录结构](#u-boot-116目录结构)

<!-- /TOC -->
# u-boot分析之编译体验

![1536285777833.png](image/1536285777833.png)

![1536285877811.png](image/1536285877811.png)

bootloader = BIOS，bootloader最终目的是启动内核

## u-boot打补丁

![1536286114498.png](image/1536286114498.png)

材料如上

```
-rw-r--r--  1 root      root      637K 8月  23 00:58 u-boot-1.1.6_jz2440.patch
-rw-r--r--  1 root      root      6.3M 8月  23 00:58 u-boot-1.1.6.tar.bz2
```

解压u-boot-1.1.6.tar.bz2，打补丁

```
patch -p1 < u-boot-1.1.6_jz2440.patch
```

参数p1指的是去掉目标目录最上级。
如果当前目录是在解压后的u-boot-1.1.6目录下，需要根据补丁文件中给出的路径配置参数

![1536289706814.png](image/1536289706814.png)

例如当前目录是在u-boot-1.1.6目录平级的目录，但是当前在u-boot-1.1.6目录下，那么就要去掉补丁中u-boot-1.1.6开头路径，这就是参数p的作用

![1536289886565.png](image/1536289886565.png)

## u-boot编译

```
make 100askjz2x0_config
make
```

![1536290149764.png](image/1536290149764.png)

![1536290179263.png](image/1536290179263.png)

![1536290190303.png](image/1536290190303.png)

## 烧写u-boot编译

![1536290197508.png](image/1536290197508.png)

![1536290223860.png](image/1536290223860.png)

获取u-boot提供的命令

![1536290257256.png](image/1536290257256.png)

获取命令帮助

```
? 命令
```

![1536290270213.png](image/1536290270213.png)

获取u-boot环境变量

![1536290383999.png](image/1536290383999.png)

设定环境变量必须通过save保存才能生效

![1536290420749.png](image/1536290420749.png)

菜单命令是编程提供，输入menu即可显示

![1536290299430.png](image/1536290299430.png)

可以usb下载线，快速上传文件到开发板。相比eop快多了

## u-boot存在的意义？

![1536291044322.png](image/1536291044322.png)

这里其实可以看的出来韦东山老师对PC机启动过程不甚了解，
PC机的bios+bootloader才等价于嵌入式的bootloader。

嵌入式系统从Flash读出内核放到SDRAM中

uboot可以理解为一个单片机程序，只是功能比较复杂

![1536312767627.png](image/1536312767627.png)

为了开发方便，一般uboot支持从网络启动内核，无非就是网络加载内核到SDRAM，也可从USB加载内核等

![1536312807534.png](image/1536312807534.png)

![1536312890125.png](image/1536312890125.png)


## u-boot-1.1.6目录结构


![1536312906136.png](image/1536312906136.png)


* board:单板相关
* common：通用
* CPU：处理器先关
* net：网卡相关
* drivers：驱动相关，网卡，USB等

如何分析结构？**分析Makefile**


![1536312985819.png](image/1536312985819.png)
