/*
 *
 * Use of source code is subject to the terms of the MorningCore license agreement under
 * which you licensed source code. If you did not accept the terms of the license agreement,
 * you are not authorized to use source code. For the terms of the license, please see the
 * license agreement between you and MoringCore.
 *
 * Copyright (c) 2010-2019      MorningCoreTech Corp.
 *
 *      PURPOSE: This files contains the char driver app test.
 *
 *      CHANGE HISTORY:
 *
 *      Version Date            Author          Description
 *      1.0.0   2022-07-18      Shang Yaqi         created
 *
 */



#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdlib.h"
#include "string.h"
#include <sys/ioctl.h>



/* 命令值 */
#define CLOSE_CMD 		(_IO(0XEF, 0x1))	/* 关闭定时器 */
#define OPEN_CMD		(_IO(0XEF, 0x2))	/* 打开定时器 */

static char usrdata[] = {"usr data!"};


int main(int argc, char *argv[])
{
	int fd, retvalue;
	char *filename;
	char readbuf[100], writebuf[100];

	if(argc != 3){
		printf("Error Usage!\r\n");
		return -1;
	}

	filename = argv[1];

	/* 打开驱动文件 */
	fd  = open(filename, O_RDWR);
	if(fd < 0){
		printf("Can't open file %s\r\n", filename);
		return -1;
	}

	if(atoi(argv[2]) == 1){ /* 从驱动文件读取数据 */
		retvalue = read(fd, readbuf, 50);
		if(retvalue < 0){
			printf("read file %s failed!\r\n", filename);
		}else{
			/*  读取成功，打印出读取成功的数据 */
			printf("read data:%s\r\n",readbuf);
		}
	}

	if(atoi(argv[2]) == 2){
 	/* 向设备驱动写数据 */
		memcpy(writebuf, usrdata, sizeof(usrdata));
		retvalue = write(fd, writebuf, 50);
		if(retvalue < 0){
			printf("write file %s failed!\r\n", filename);
		}
	}


	if(atoi(argv[2]) == 3){
 	/* 向设备驱动写数据 */
		ioctl(fd, CLOSE_CMD, 0);
		printf("close cmd!\r\n");
	}

	if(atoi(argv[2]) == 4){
 	/* 向设备驱动写数据 */
		ioctl(fd, OPEN_CMD, 0);
		printf("open cmd!\r\n");
	}
	
	/* 关闭设备 */
	retvalue = close(fd);
	if(retvalue < 0){
		printf("Can't close file %s\r\n", filename);
		return -1;
	}

	return 0;
}




