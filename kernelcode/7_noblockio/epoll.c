/***************************************************************
文件名              : epoll.c
作者                : shangyaqi
版本                : V1.0
描述                : epoll实验
其他                : 无
使用方法            : ./epoll /dev/key
***************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <sys/epoll.h>


/*
 * @description		: main主程序
 * @param – argc		: argv数组元素个数
 * @param – argv		: 具体参数
 * @return			: 0 成功;其他 失败
 */
int main(int argc, char *argv[])
{
	int epfd, waitret;
    int fd, ret;
    int key_val;

    /* 判断传参个数是否正确 */
    if(2 != argc) {
        printf("Usage:\n"
             "\t./keyApp /dev/key\n"
            );
        return -1;
    }

    /* 打开设备 */
    fd = open(argv[1], O_RDONLY | O_NONBLOCK);
    if(0 > fd) {
        printf("ERROR: %s file open failed!\n", argv[1]);
        return -1;
    }


	epfd = epoll_create(2);
	if (ret < 0) {
		printf("epoll creat is  failed!\n");
	}

	static struct epoll_event epollevent;
	epollevent.events = EPOLLIN;
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &epollevent);

    /* 循环读取按键数据 */
    for ( ; ; ) {
		waitret = epoll_wait(epfd, &epollevent, 2, -1);
		if (waitret == -1) {
			printf("epoll_wait error\n");
		} else {
			read(fd, &key_val, sizeof(int));
	        if (0 == key_val)
	            printf("Key Press\n");
	        else if (1 == key_val)
	            printf("Key Release\n");
		}
    }

    /* 关闭设备 */
    close(fd);
    return 0;
}

