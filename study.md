# 虚拟机docker配置

## 安装docker与环境配置

下载安装 docker ：

```
sudo apt install docker.io -y
```

#验证自己下载的安装包是否完整

```
luckfox@luckfox:~$ md5sum luckfox_pico_docker.tar
56c6e0d5cdc28f9f05c983c3af1b2486  luckfox_pico_docker.tar
```

#docker镜像安装

```
sudo docker load -i  ./luckfox_pico_docker.tar 
```

运行以下命令将当前用户添加到docker组中：

```
sudo usermod -aG docker $USER
```

注销并重新登录，或者运行以下命令使更改生效

```
sudo service docker restart
newgrp docker
```

确保用户 luckfox 已经成功添加到 docker 组。可以使用以下命令检查：

```shell
id luckfox
```

获取已经配置好的官方 Docker 镜像。

```shell
sudo docker pull luckfoxtech/luckfox_pico:1.0
```

**这一步一般会有错误，做如下修改可以解决：**

配置加速地址

```
touch /etc/docker/daemon.json
vi /etc/docker/daemon.json
```

添加以下内容

```
{
 "registry-mirrors": ["https://docker.1panel.live"]
} 
```

重启docker

```
systemctl daemon-reload
systemctl restart  docker
```

获取最新的的 SDK使用如下指令下载 。

```shell
git clone https://gitee.com/LuckfoxTECH/luckfox-pico.git
cd luckfox-pico
```

启动一个交互式的容器，使用名为 "luckfox"，并将本地主机上的 SDK 目录映射到容器内的 /home 目录，最后以 Bash shell 运行。

```shell
#第一次运行docker环境
sudo docker run -it --name shangyaqi --privileged -v /home/shang/luckfox-git/luckfox-pico:/home luckfoxtech/luckfox_pico:1.0 /bin/bash

#第二次运行docker环境
sudo docker start -ai shangyaqi

#退出
exit
```

安装交叉编译工具链。

```shell
cd tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/
source env_install_toolchain.sh
```

进入/home 目录，清除编译。

```shell
root@b165d8d4c29b:# cd /home
root@b165d8d4c29b:/home# ./build.sh clean
```

## Docker 常用命令

以下是 Docker 的一部分命令。要查看完整的命令列表和帮助信息，请运行 `docker --help`。

镜像相关命令：

```sh
docker pull luckfoxtech/luckfox_pico:1.0      #从 Docker Hub 下载镜像。
docker images                                 #列出本地所有的镜像。
docker rmi luckfoxtech/luckfox_pico:1.0       #删除本地一个或多个镜像。
docker search [image]                         #在 Docker Hub 上搜索镜像。
```

容器相关命令：

```sh
docker ps                                     #列出当前运行的容器。
docker ps -a                                  #列出所有的容器，包括停止的。

luckfox@luckfox:~$ docker ps -a
CONTAINER ID   IMAGE                          COMMAND       CREATED       STATUS                   
1758ef6954f6   luckfoxtech/luckfox_pico:1.0   "/bin/bash"   2 weeks ago   Exited (137) 39 minutes ago
```

容器操作：

```sh
docker start 1758                            #启动一个停止的容器。
docker stop 1758                             #停止一个运行中的容器。
docker restart 1758                          #重启一个容器。
docker exec -it 1758 ls                      #在运行中的容器中执行命令。
docker exec -it 1758 bash                    #进入一个已经打开的容器
exit                                         #退出
docker rm 1758                               #删除一个或多个容器。
```

运行容器：

```shell
sudo docker run -it --name luckfox --privileged -v /home/ubuntu/luckfox-pico:/home luckfoxtech/luckfox_pico:1.0 /bin/bash
```

- `-it`运行一个交互式容器
- `--name`可以为容器指定一个名称，使得容器更易于识别和管理
- `-v` 选项可以将主机上的目录或文件挂载到容器内部。上述例子中，将主机上的 `/home/ubuntu/luckfox-pico` 目录挂载到容器内的 `/home` 目录

系统信息：

```sh
docker logs 1758                             #查看容器的日志。
docker inspect 1758                          #查看容器的详细信息。
docker top 1758                              #查看容器中运行的进程。
```

- 注：通常你只需要提供容器的前几位字符作为容器的 ID，由于 Docker 容器的 ID 是唯一的。前几位字符通常足以唯一标识一个容器。使用命令时请替换自己的实际容器 ID。

# 工具使用

## vscode使用

### markdown如何查看

ctrl+shift+v可以预览

### 初始化配置

chinese语言设置，github theme颜色配置

### 运行c语言

下载c/c++组件，下载mingw，[x86_64-15.1.0-release-win32-seh-msvcrt-rt_v12-rev0.7z](https://github.com/niXman/mingw-builds-binaries/releases/download/15.1.0-rt_v12-rev0/x86_64-15.1.0-release-win32-seh-msvcrt-rt_v12-rev0.7z)

https://github.com/niXman/mingw-builds-binaries/releases

下载完添加到环境变量中

点击调试c/c++组件

## Topyra

### 设置大纲折叠

文件-》偏好设置-》外观-》允许折叠大纲视图

## GIT使用

配置git全局账户和Email
注意：Email和name设置成自己的，Email后面用得上

```
git config --global user.email “1136345380@qq.com”
git config --global user.name “shanggelaile”
```

设置完成后查看配置是否成功

```
git config --global -l
```

生成密钥,打开cmd 输入生成密钥命令，按3下回车

```
 ssh-keygen -t rsa -C "1136345380@qq.com"
```

Gitee导入公钥
用记事本打开刚刚生成的公钥：D:\Users\22030078.ssh\id_rsa.pub 。全选复制下来

```c
git push origin main
```

```
git commit -m "xxxxxx"
```





# KO文件编译

makefile文件demo

```makefile
obj-m += helloworld.o
KDIR:=/home/ubuntu/Luckfox/sdk-1015/luckfox-pico/sysdrv/source/kernel
PWD?=$(shell pwd)
MAKE := make
ARCH := arm
CROSS_COMPILE := /home/ubuntu/Luckfox/sdk-1015/luckfox-pico/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin/arm-rockchip830-linux-uclibcgnueabihf-
KBUILD_OUTPUT := $(abspath $(dir $(lastword $(KDIR))))/objs_kernel
all:
        $(MAKE) O=$(KBUILD_OUTPUT) -C $(KDIR) M=$(PWD) modules \
ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)
        echo $(PWD)
clean:
        rm -f *.ko *.o *.mod *.mod.o *.mod.c *.symvers *.order
```



# Linux常用命令

## 压缩与解压缩

将test1压缩

```
tar -vcjf test1.tar.bz2 test1
tar -vczf test1.tar.gz test1
zip -rv test1.zip test1
```

解压缩

```
tar -vxjf test1.tar.bz2
tar -vxzf test1.tar.gz
unzip test1.zip
```

## 挂载debugfs

```
mount -t debugfs none /sys/kernel/debug
```

# C语言注意事项

## C语言指针

### 指针赋值

```
注意（char *）pi是指针，*pi是变量
```

#### NULL指针

```
#define NULL ((void *)0)
```

```c
int num;
int *pi = &num;
pi = 0; pi = NULL; //都是正确的
```

#### void指针

void指针与char指针具有相同的形式和内存对齐格式

### 指针加减法

```c
char c;
char *pc = &c;
pc++;
```

#### *pc++和++pc区别

```c
*pc++
*(pc); pc = pc + 1;
*++pc
pc++；*pc
```

### 字符串处理

```c
int main(int argc, char **argv)
int main(int argc, char *argv[])
//这两个是等价的
```

### 常用字符串处理函数

strcpy，strcmp（两个字符串相等返回0），

char *strcat(char *s1, const char *s2);



### 指针预定义类型

size_t表示c语言中任何对象能达到的最大长度

### 二级数组指针

```C
char *name[] = {"123","1234","123456"};
printf("%c\n",*(*(name+1)+2));
printf("%c\n",name[1][2]);
```

### 打印

%p：表示指针，以16进制大写为主

%zu%lu%u：表示无符号型整数

```c
printf("%p");
```

## C语言常用处理函数

```c
(int)round(0.55);//四舍五入
int rounded = (int)(num + 0.5); // 适用于正数
```

# 代码优化

## 优化逻辑

性能优化和代码优化是提高程序执行效率和资源利用率的重要方面。以下是一些关于C语言性能优化和代码优化的技巧和指导原则：

1. 选择合适的算法和数据结构：在设计和实现程序时，选择适合问题需求的高效算法和数据结构。了解不同算法的时间复杂度和空间复杂度，并根据实际情况进行选择。

   ```C
   // 优化前（浪费4字节填充）
   struct Data { char a; int b; };  // 占用8字节（4+1+3）
   
   // 优化后（紧凑排列）
   struct Data { int b; char a; };  // 占用5字节（4+1）
   ```

2. 减少循环和迭代次数：避免不必要的循环和迭代操作。通过合并循环、使用适当的循环条件和跳出条件，减少循环次数和迭代次数，提高程序效率。

   常量要放到外面去，比如sizeof一个固定长度数组

   ```c
   // 优化前
   struct Data { char a; int b; };  // 占用8字节（4+1+3）
   for (int i=0; i<N; i++) {
       sum += data[i] * sizeof(Data);  // factor 若为常数，应外提
   }
   
   // 优化后
   int datasize = sizeof(Data);
   for (int i=0; i<N; i++) {
       sum += data[i] * datasize;
   }
   ```

3. 减少内存分配和释放：避免频繁的内存分配和释放操作，尽量重用已分配的内存空间。合理管理内存，避免内存泄漏和内存碎片化。

   ```c
   // 优化前（1000次malloc）
   int *arr[1000];
   for (int i=0; i<1000; i++) arr[i] = malloc(sizeof(int));
   
   // 优化后（1次malloc）
   int *buf = malloc(1000*sizeof(int));
   int *arr[1000];
   for (int i=0; i<1000; i++) arr[i] = buf + i;
   ```

4. 减少函数调用开销：函数调用会带来一定的开销，包括参数传递、栈帧的创建和销毁等。减少不必要的函数调用，尽量内联短小的函数，优化函数调用的性能。

   ```c
   static inline int square(int x) { return x * x; }  // 编译期内联
   // 优化前（传递结构体值，开销大）
   void process(struct LargeStruct data);
   // 优化后（传递指针）
   void process(struct LargeStruct *data);
   ```

5. 避免过多的变量副本：避免创建不必要的临时变量和副本。使用指针或引用传递数据，避免频繁的数据拷贝操作。

   ```c
   //优化前
   memcpy(ptr1,ptr2,length);
   //优化后
   ptr2 = ptr1;
   ```

6. 使用适当的数据类型：选择适当的数据类型来存储数据，避免过大或过小的数据类型导致的内存浪费或溢出问题。合理使用位运算和位操作来提高效率。

   ```c
   x *= 8;  // 等价于 x <<= 3
   y /= 16; // 等价于 y >>= 4
   //当数据非负时，用 uint32_t 替代 int，避免符号位检查（部分架构优化）。
   ```

   

7. 缓存友好性优化：了解计算机的内存层次结构，充分利用缓存，减少缓存失效带来的性能影响。优化数据访问模式，使得数据的访问更加连续和局部性化。

   ```c
   // 缓存友好（行优先）
   for (int i=0; i<N; i++) {
       for (int j=0; j<M; j++) {
           sum += arr[i][j];
       }
   }
   
   // 缓存不友好（列优先）
   for (int j=0; j<M; j++) {
       for (int i=0; i<N; i++) {
           sum += arr[i][j];  // 跨行访问，缓存失效多
       }
   }
   ```

8. 并行和并发优化：对于适合并行执行的任务，使用多线程或并发编程技术进行优化。合理利用多核处理器和并行计算资源，提高程序的性能。

   ```c
   //在高频竞争场景中，用原子操作（atomic_flag）替代锁，减少线程阻塞。
   ```

9. 使用适当的编译选项：根据目标平台和需求，使用适当的编译选项来优化代码生成和执行效率。例如，启用优化选项、调整内存对齐方式等。

10. 测试和性能分析：使用性能分析工具（如Profiling工具）对程序进行测试和分析，找出性能瓶颈和热点，针对性地进行优化。

    ```c
    早优化陷阱：优先保证代码正确性和可维护性，过早优化可能导致代码复杂度过高。
    局部优化：对热点函数（如循环内代码）重点优化，非热点代码无需过度优化。
    先 profiling 后优化：用工具定位瓶颈，避免盲目优化。
    小步验证：每次优化后测试性能变化，确保优化有效。
    版本控制：保存优化前代码，便于回滚对比。
    ```

11. 不断学习和改进：了解最新的优化技术和工具，学习其他开发者的经验

# 内核优化

## 内存读写

### wmb函数

现代处理器为了提高性能，会对内存操作进行 **重排序**（包括编译器优化和 CPU 硬件优化）。例如：

```c
a = 1;    // 写操作1
b = 2;    // 写操作2
wmb();    // 内存屏障
c = 3;    // 写操作3
d = 4;    // 写操作4
```

若无 `wmb()`，处理器可能将写操作重排序为：`a=1 → c=3 → d=4 → b=2`。
`wmb()` 强制要求：**`a=1` 和 `b=2` 必须在 `c=3` 和 `d=4` 之前完成**。

## 数据结构

### 链表学习

Linux 内核使用了一种 **双向循环链表** 作为基础的[数据结构](https://so.csdn.net/so/search?q=数据结构&spm=1001.2101.3001.7020)，主要用于管理各种内核对象，如进程、任务、设备等。它的实现位于 `include/linux/list.h` 头文件中。

## 同步管理

| 同步机制 | 适用场景                       | 特点                             |
| -------- | ------------------------------ | -------------------------------- |
| 自旋锁   | 短临界区，不可睡眠上下文       | 忙等待，不引起调度               |
| 互斥量   | 长临界区，可睡眠               | 会引起进程调度，不能在中断中使用 |
| 读写锁   | 读多写少场景                   | 允许多个读者，互斥写者           |
| 顺序锁   | 读多写少且读者不需要绝对一致性 | 写优先，读不阻塞写               |

### 原子操作

由于锁的开销较大，单变量就使用原子操作，用的少先不用记录

### 内存屏障

在smp场景中，可能会引起读写顺序不一致问题，需要使用内存屏障来保障读写顺序

```c
mb();//读写都可以
rmb();//保护读
wmb();//保护写
smp_mb();smp_rmb();smp_wmb();
```

### 互斥锁

互斥锁可以使用可能阻塞的函数，如link = kzalloc(sizeof(*link), GFP_KERNEL);

```c
#include <linux/mutex.h>
static DEFINE_MUTEX(fwnode_link_lock);
//或者mutex_init(&fwnode_link_lock);
mutex_lock(&fwnode_link_lock);
mutex_unlock(&fwnode_link_lock);
```

### 自旋锁

必须时间短，不能休眠与等待，不能使用malloc等引起睡眠的操作，多用于在链表操作上

```c
// 基本自旋锁定义
spinlock_t lock;
 
// 初始化方式
spin_lock_init(&lock);  // 动态初始化
DEFINE_SPINLOCK(lock);  // 静态初始化
 
// 常用操作函数
spin_lock(&lock);       // 获取锁
spin_unlock(&lock);     // 释放锁
 
// 中断安全版本
spin_lock_irqsave(&lock, flags);    // 保存中断状态并禁用中断
spin_unlock_irqrestore(&lock, flags); // 恢复中断状态
 
// 其他变体
spin_lock_irq(&lock);   // 禁用本地中断并获取锁
spin_unlock_irq(&lock); // 释放锁并启用本地中断
```

## 调试技术

最近在验证芯片功能的过程中发现了一个好用的内核调试接口，print_hex_dump，除了直接打印16进制和ascii外，还支持动态调试打印，uboot和内核中都有该函数的实现。

1.函数功能与用途
print_hex_dump是 Linux 内核中的一个函数，用于以十六进制和 ASCII 码格式打印内存数据块。它提供了一种方便的方式来查看和调试内核中的二进制数据。下面是函数原型：

void print_hex_dump(const char *level, const char *prefix_str, int prefix_type,
		    int rowsize, int groupsize,
		    const void *buf, size_t len, bool ascii)
{
	const u8 *ptr = buf;
	int i, linelen, remaining = len;
	unsigned char linebuf[32 * 3 + 2 + 32 + 1];

	if (rowsize != 16 && rowsize != 32)
		rowsize = 16;
	 
	for (i = 0; i < len; i += rowsize) {
		linelen = min(remaining, rowsize);
		remaining -= rowsize;
	 
		hex_dump_to_buffer(ptr + i, linelen, rowsize, groupsize,
				   linebuf, sizeof(linebuf), ascii);
	 
		switch (prefix_type) {
		case DUMP_PREFIX_ADDRESS:
			printk("%s%s%p: %s\n",
			       level, prefix_str, ptr + i, linebuf);
			break;
		case DUMP_PREFIX_OFFSET:
			printk("%s%s%.8x: %s\n", level, prefix_str, i, linebuf);
			break;
		default:
			printk("%s%s%s\n", level, prefix_str, linebuf);
			break;
		}
	}
}
AI写代码

参数解释：

@level: 内核log级别 (e.g. KERN_DEBUG)

@prefix_str: 打印的前缀字符串

@prefix_type: 地址的输出格式是按照偏移、绝对地址或者不输出地址 (%DUMP_PREFIX_OFFSET, %DUMP_PREFIX_ADDRESS, %DUMP_PREFIX_NONE)

@rowsize: 每行印输出字节数; must be 16 or 32

@groupsize: 每次打印多少字节 (1, 2, 4, 8; default = 1)

@buf: 打印的数据地址

@len: 要打印数据长度

@ascii: 是否输出ascii码

2. 示例
在调试内核模块间传递的数据时，通过打印数据的十六进制和 ASCII 码表示，能够更直观地分析数据内容和格式是否符合预期。

一次打印1字节，共打印32字节：

print_hex_dump(KERN_DEBUG, "show: ", DUMP_PREFIX_ADDRESS, 16, 1,bar_addr, 32, true);

 输出打印现象：



一次打印2字节，共打印64字节：

print_hex_dump(KERN_DEBUG, "show: ", DUMP_PREFIX_ADDRESS, 16, 2,bar_addr, 64, true);

 

3.动态调试
除此之外print_hex_dump_debug还支持动态调试，在这之前我们先了解下什么是动态调试。

这功能牛逼极了，我们不再需要为了添加了调试代码，而重新编译安装内核/驱动。你可以指定 CONDIF_DYNAMIC_DEBUG 选项打开动态调试功能，然后通过 /sys/kernel/debug/dynamic_debug/control 接口指定要打印哪些调试日志。

1. mkdir /mnt/dbg

2. mount -t debugfs none /mnt/dbg 默认挂载在/sys/kernel/debug/下

3.控制某个文件所有输出dev_dbg()、print_hex_dump_debug等

echo -n "file xxx.c +p" > /mnt/dbg/dynamic_debug/control

4.控制某个函数所有dev_dbg()，print_hex_dump_debug等

echo -n "func xxx +p" > /mnt/dbg/dynamic_debug/control

运行程序，使用dmesg则可以看到相应dev_dbg()的输出信息。

5.当调试结束，不再想输出dev_dbg()信息了，使用下面命令关闭即可

echo -n "file xxx.c -p" > /mnt/dbg/dynamic_debug/control

echo -n "func xxx -p" > /mnt/dbg/dynamic_debug/control

————————————————

![img](https://i-blog.csdnimg.cn/direct/ac1014e056b24969958199d2821b7f89.png)



![img](https://i-blog.csdnimg.cn/direct/9af07353ff7c48bc9ea141e0e6a88a11.png)

                            本文为博主原创文章，未经博主允许不得转载。

原文链接：https://blog.csdn.net/muaxi8/article/details/143186067
