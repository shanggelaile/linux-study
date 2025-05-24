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

