/*
 *
 * Use of source code is subject to the terms of the MorningCore license agreement under
 * which you licensed source code. If you did not accept the terms of the license agreement,
 * you are not authorized to use source code. For the terms of the license, please see the
 * license agreement between you and MoringCore.
 *
 * Copyright (c) 2010-2019      MorningCoreTech Corp.
 *
 *      PURPOSE: This files contains the cha driver.
 *
 *      CHANGE HISTORY:
 *
 *      Version Date            Author          Description
 *      1.0.0   2022-07-18      Shang Yaqi         created
 *
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/workqueue.h>


#include <linux/device.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <asm/uaccess.h>
#include <comip/hardware.h>
#include <comip/comip-pmic.h>
#include <linux/of_address.h>
#include <linux/of.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include <linux/notifier.h>

#include <linux/errno.h>
#include <linux/cdev.h>
//#include <asm/mach/map.h>
#include <asm/io.h>


#define DEVICE_CNT			1			/* 设备号个数 */
#define DEVICE_NAME			"mychar"	/* 名字 */

static char readbuf[100];		/* 读缓冲区 */
static char writebuf[100];		/* 写缓冲区 */
static char kerneldata[] = {"kernel data!"};

/* char设备结构体 */
struct charbase_dev{
	dev_t devid;			/* 设备号 	 */
	struct cdev cdev;		/* cdev 	*/
	struct class *class;	/* 类 		*/
	struct device *device;	/* 设备 	 */
	int major;				/* 主设备号	  */
	int minor;				/* 次设备号   */
	struct timer_list my_timer;	/*test timer*/
	struct workqueue_struct *mywork_queue;	/*work queue*/
	struct work_struct mywork_struct;	/*work struct*/
	int mydata;				/* test data param */
};

struct charbase_dev my_char;	/* char device */

static DEFINE_SPINLOCK(mfp_lock);	  	/* define a    spinlock */


/*
 * @description		: timer callback function
 * @param - data 	: tarnsmission data
 */
void my_work_handle(struct work_struct *work)
{
	struct charbase_dev *my_char = container_of(work, struct charbase_dev, mywork_struct);
	printk(KERN_ALERT "%s called (%d).\n", __FUNCTION__, my_char->mydata);
}


/*
 * @description		: timer callback function
 * @param - data 	: tarnsmission data
 */
void my_timer_callback(unsigned long data)
{
	printk(KERN_ALERT "%s called (%ld).\n", __FUNCTION__, jiffies);
	printk(KERN_ALERT "hello delay.\n");
	//mod_timer(&my_char.my_timer, jiffies + msecs_to_jiffies(3000));
}


/*
 * @description		: 打开设备
 * @param - inode 	: 传递给驱动的inode
 * @param - filp 	: 设备文件，file结构体有个叫做private_data的成员变量
 * 					  一般在open的时候将private_data指向设备结构体。
 * @return 			: 0 成功;其他 失败
 */
static int char_open(struct inode *inode, struct file *filp)
{
	printk("chrdevbase open!\r\n");
	return 0;
}

/*
 * @description		: 从设备读取数据 
 * @param - filp 	: 要打开的设备文件(文件描述符)
 * @param - buf 	: 返回给用户空间的数据缓冲区
 * @param - cnt 	: 要读取的数据长度
 * @param - offt 	: 相对于文件首地址的偏移
 * @return 			: 读取的字节数，如果为负值，表示读取失败
 */
static ssize_t char_read(struct file *filp, char __user *buf, size_t cnt, loff_t *offt)
{
	int retvalue = 0;
	
	/* 向用户空间发送数据 */
	memcpy(readbuf, kerneldata, sizeof(kerneldata));
	retvalue = copy_to_user(buf, readbuf, cnt);
	if(retvalue == 0){
		printk("kernel senddata ok!\r\n");
	}else{
		printk("kernel senddata failed!\r\n");
	}
	
	//printk("chrdevbase read!\r\n");
	return 0;

}

/*
 * @description		: 向设备写数据 
 * @param - filp 	: 设备文件，表示打开的文件描述符
 * @param - buf 	: 要写给设备写入的数据
 * @param - cnt 	: 要写入的数据长度
 * @param - offt 	: 相对于文件首地址的偏移
 * @return 			: 写入的字节数，如果为负值，表示写入失败
 */
static ssize_t char_write(struct file *filp, const char __user *buf, size_t cnt, loff_t *offt)
{
	int retvalue = 0;
	/* 接收用户空间传递给内核的数据并且打印出来 */
	retvalue = copy_from_user(writebuf, buf, cnt);
	if(retvalue == 0){
		printk("kernel recevdata:%s\r\n", writebuf);
	}else{
		printk("kernel recevdata failed!\r\n");
	}
	
	//printk("chrdevbase write!\r\n");
	return 0;

}

/*
 * @description		: 关闭/释放设备
 * @param - filp 	: 要关闭的设备文件(文件描述符)
 * @return 			: 0 成功;其他 失败
 */
static int char_release(struct inode *inode, struct file *filp)
{
	printk("chrdevbase release！\r\n");
	return 0;
}


/* 设备操作函数 */
static struct file_operations my_char_fops = {
	.owner = THIS_MODULE,
	.open = char_open,
	.read = char_read,
	.write = char_write,
	.release = 	char_release,
};


/* 驱动入口函数 */
static int __init chrdevbase_init(void)
{
	int ret = 0;
	unsigned long flags;
	/* 入口函数具体内容 */
	printk(KERN_ALERT "hello world!");
	/* 注册字符设备驱动 */
	/* 1、创建设备号 */
	if (my_char.major) {		/*	定义了设备号 */
		my_char.devid = MKDEV(my_char.major, 0);
		ret = register_chrdev_region(my_char.devid, DEVICE_CNT, DEVICE_NAME);
		if(ret < 0) {
			pr_err("cannot register %s char driver [ret=%d]\n", DEVICE_NAME, DEVICE_CNT);
			goto free_gpio;
		}
	} else {						/* 没有定义设备号 */
		ret = alloc_chrdev_region(&my_char.devid, 0, DEVICE_CNT, DEVICE_NAME); /* 申请设备号 */
		if(ret < 0) {
			pr_err("%s Couldn't alloc_chrdev_region, ret=%d\r\n", DEVICE_NAME, ret);
			goto free_gpio;
		}
		my_char.major = MAJOR(my_char.devid); /* 获取分配号的主设备号 */
		my_char.minor = MINOR(my_char.devid); /* 获取分配号的次设备号 */
	}
	printk("beep major=%d,minor=%d\r\n",my_char.major, my_char.minor);	
	
	/* 2、初始化cdev */
	my_char.cdev.owner = THIS_MODULE;
	cdev_init(&my_char.cdev, &my_char_fops);
	
	/* 3、添加一个cdev */
	cdev_add(&my_char.cdev, my_char.devid, DEVICE_CNT);
	if(ret < 0)
		goto del_unregister;
		
	/* 4、创建类 */
	my_char.class = class_create(THIS_MODULE, DEVICE_NAME);
	if (IS_ERR(my_char.class)) {
		goto del_cdev;
	}

	/* 5、创建设备 */
	my_char.device = device_create(my_char.class, NULL, my_char.devid, NULL, DEVICE_NAME);
	if (IS_ERR(my_char.device)) {
		goto destroy_class;
	}


	/* init timer */
	printk(KERN_ALERT "hello world!\n");
	init_timer(&my_char.my_timer);
	my_char.my_timer.function = my_timer_callback;
	spin_lock_irqsave(&mfp_lock, flags);
	my_char.my_timer.expires = jiffies + msecs_to_jiffies(3000);
	spin_unlock_irqrestore(&mfp_lock, flags);
	add_timer(&my_char.my_timer);
	printk(KERN_ALERT "add_timer\n");


	/* init workqueue */
	my_char.mywork_queue = create_singlethread_workqueue("my_workqueue_thread");
	INIT_WORK(&my_char.mywork_struct, my_work_handle);
	queue_work(my_char.mywork_queue, &my_char.mywork_struct);
	
	return 0;
	
	
destroy_class:
	class_destroy(my_char.class);
del_cdev:
	cdev_del(&my_char.cdev);
del_unregister:
	unregister_chrdev_region(my_char.devid, DEVICE_CNT);
free_gpio:
	//gpio_free(my_char.beep_gpio);
	return -EIO;
}

/* 驱动出口函数 */
static void __exit chrdevbase_exit(void)
{
	/* 出口函数具体内容 */
	/* 注销字符设备驱动 */
	del_timer_sync(&my_char.my_timer);
	flush_workqueue(my_char.mywork_queue);
	destroy_workqueue(my_char.mywork_queue);
	cdev_del(&my_char.cdev);/*  删除cdev */
	unregister_chrdev_region(my_char.devid, DEVICE_CNT); /* 注销设备号 */
	device_destroy(my_char.class, my_char.devid);/* 注销设备 */
	class_destroy(my_char.class);/* 注销类 */
}



/* 将上面两个函数指定为驱动的入口和出口函数 */
module_init(chrdevbase_init);
module_exit(chrdevbase_exit);
MODULE_DESCRIPTION("test char driver");
MODULE_LICENSE("GPL");

