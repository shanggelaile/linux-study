#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(void)
{
	pid_t pid;
	int fd;
	int i;
	pid = fork();
	switch (pid) {
		case -1:
			perror("fork error");
			exit(-1);
		case 0:
			/* 子进程 */
			printf("这是子进程打印信息<pid: %d, 父进程 pid: %d>\n",
				getpid(), getppid());
			fd = open("hello.txt", O_RDWR | O_CREAT, S_IRWXU | S_IRGRP | S_IROTH);
			if (0 > fd) {
				perror("open error");
				_exit(-1);
			}
			for (i = 0; i < 4; i++) //循环写入 4 次
				write(fd, "1122", 4);
			close(fd);
			_exit(0);
		default:
			/* 父进程 */
			printf("这是父进程打印信息<pid: %d, 子进程 pid: %d>\n",
				getpid(), pid);
			fd = open("hello.txt", O_RDWR | O_CREAT, S_IRWXU | S_IRGRP | S_IROTH);
			if (0 > fd) {
				perror("open error");
				exit(-1);
			}
			for (i = 0; i < 4; i++) //循环写入 4 次
				write(fd, "AABB", 4);
			close(fd);
			exit(0);
	}
}




