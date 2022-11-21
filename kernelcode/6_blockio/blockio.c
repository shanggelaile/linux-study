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
#include <linux/semaphore.h>

#include <linux/errno.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <linux/irq.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/fs.h>
#include <linux/fcntl.h>


/* 定义按键值 */
#define DEVICE_CNT			1			/* 设备号个数 */
#define DEVICE_NAME			"key"	/* 名字 */

/* 命令值 */
#define CLOSE_CMD 		(_IO(0XEF, 0x1))	/* 关闭 */
#define OPEN_CMD		(_IO(0XEF, 0x2))	/* 打开 */


/* 定义按键状态 */
enum key_status {
    KEY_PRESS = 0,      /* 按键按下 */ 
    KEY_RELEASE,        /* 按键松开 */ 
    KEY_KEEP,           /* 按键状态保持 */ 
};

/*设备结构体 */
struct key_dev{
	dev_t devid;			/* 设备号 	 */
	struct cdev cdev;		/* cdev 	*/
	struct class *class;	/* 类 		*/
	struct device *device;	/* 设备 	 */
	struct device_node *nd; /* 设备节点 */
	int major;				/* 主设备号	  */
	int minor;				/* 次设备号   */
	int gpionum;			/*gpio number*/
	int keyvalue;			/* 按键值 		*/	
	int irqnum;
};

struct key_dev mydev;	/* key device */

static atomic_t status;  	/* 按键状态 */
static struct timer_list timer;	/* 按键值 		*/
static spinlock_t myspinlock;		/* 自旋锁		*/
struct workqueue_struct *mywork_queue;	/*work queue*/
//struct work_struct mywork_struct;	/*work struct*/
struct delayed_work mywork_struct;	/*delaywork struct*/
wait_queue_head_t r_wait; /* 读等待队列头 */

/*
 * @description	: GPIO中断处理函数
 * @param irq	: 无
 * @param dev_id: 无
 * @return irqreturn_t: 
 */
static irqreturn_t dev_interrupt(int irq, void *dev_id)
{
	/* 按键防抖处理，开启定时器延时15ms */
	mod_timer(&timer, jiffies + msecs_to_jiffies(15));
    return IRQ_HANDLED;
}

/*
 * @description		: timer callback function
 * @param - data 	: tarnsmission data
 */
void my_work_handle(struct work_struct *work)
{
	printk(KERN_ALERT "%s called (%d).\n", __FUNCTION__, mydev.gpionum);
}

/*
 * @description	: 解析设备树初始化按键IO，open函数打开驱动的时候
 * 				  初始化按键所使用的GPIO引脚。
 * @param 		: 无
 * @return 		: 无
 */
static int dev_parse_dt(void)
{
	int ret;
	unsigned long irq_flags;

	/* 设置LED所使用的GPIO */
	/* 获取设备节点：key */
	mydev.nd = of_find_node_by_path("/mykeypad");
	if(mydev.nd == NULL) {
		printk("key node not find!\r\n");
		return -EINVAL;
	}

	/* 获取设备树中的gpio属性，得到KEY0所使用的KYE编号 */
	mydev.gpionum = of_get_named_gpio(mydev.nd, "gpio_key", 0);
	if(mydev.gpionum < 0) {
		printk("can't get key-gpio");
		return -EINVAL;
	}

    /* 获取GPIO对应的中断号 */
	mydev.irqnum = gpio_to_irq(mydev.gpionum);
	printk(KERN_ERR "irq number is %d\n",mydev.irqnum);
    //mydev.irqnum = irq_of_parse_and_map(mydev.nd, 0);
    if(!mydev.irqnum){
        return -EINVAL;
    }

	printk("key-gpio num = %d\r\n", mydev.gpionum);

	ret = gpio_request(mydev.gpionum, "KEY0");
    if (ret) {
        printk(KERN_ERR "key: Failed to request key-gpio\n");
        return ret;
	}	
	
	/* 将GPIO设置为输入模式 */
    gpio_direction_input(mydev.gpionum);

    /* 设置按键中断触发类型 */
	irq_flags = IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING;
	
	/* 申请中断 */
	ret = request_irq(mydev.irqnum, dev_interrupt, irq_flags, "Key0_IRQ", NULL);
	if (ret) {
        gpio_free(mydev.gpionum);
        return ret;
    }
	return 0;
}

/* 定义按键值 */
static void dev_timer_function(unsigned long data)
{
    static int last_val = 1;
    int current_val;


    /* 读取按键值并判断按键当前状态 */
    current_val = gpio_get_value(mydev.gpionum);
    if (0 == current_val && last_val) {       /* 按下 */ 
		atomic_set(&status, KEY_PRESS); /* 按下 */
		wake_up_interruptible(&r_wait); /* 唤醒 */
	}
    else if (1 == current_val && !last_val) {
		atomic_set(&status, KEY_RELEASE); /* 按下 */
		wake_up_interruptible(&r_wait); /* 唤醒 */
    }	
    else
		atomic_set(&status, KEY_KEEP); /* 状态保持 */ 

    last_val = current_val;

}

/*
 * @description		: 打开设备
 * @param - inode 	: 传递给驱动的inode
 * @param - filp 	: 设备文件，file结构体有个叫做private_data的成员变量
 * 					  一般在open的时候将private_data指向设备结构体。
 * @return 			: 0 成功;其他 失败
 */
static int dev_open(struct inode *inode, struct file *filp)
{
	printk(KERN_ALERT "dev open!\r\n");
	filp->private_data = &mydev; 	/* 设置私有数据 */
	return 0;
}

/*
 * @description		: 关闭/释放设备
 * @param - filp 	: 要关闭的设备文件(文件描述符)
 * @return 			: 0 成功;其他 失败
 */
static int dev_release(struct inode *inode, struct file *filp)
{
	printk(KERN_ALERT "dev release！\r\n");
	return 0;
}

/*
 * @description     : 从设备读取数据 
 * @param – filp        : 要打开的设备文件(文件描述符)
 * @param – buf     : 返回给用户空间的数据缓冲区
 * @param – cnt     : 要读取的数据长度
 * @param – offt        : 相对于文件首地址的偏移
 * @return          : 读取的字节数，如果为负值，表示读取失败
 */
static ssize_t dev_read(struct file *filp, char __user *buf,
            size_t cnt, loff_t *offt)
{
	int ret;
	ret = wait_event_interruptible(r_wait, KEY_KEEP != atomic_read(&status));


	/* 将按键状态信息发送给应用程序 */
	ret = copy_to_user(buf, &status, sizeof(int));

	/* 状态重置 */
	atomic_set(&status, KEY_KEEP); /* 状态保持 */

	return ret;
}

/*
 * @description		: 向设备写数据 
 * @param - filp 	: 设备文件，表示打开的文件描述符
 * @param - buf 	: 要写给设备写入的数据
 * @param - cnt 	: 要写入的数据长度
 * @param - offt 	: 相对于文件首地址的偏移
 * @return 			: 写入的字节数，如果为负值，表示写入失败
 */
static ssize_t dev_write(struct file *filp, const char __user *buf, size_t cnt, loff_t *offt)
{
	return 0;
}

/*
 * @description		: ioctl系统编程与内核驱动的交互
 * @param - filp 	: 设备文件，表示打开的文件描述符
 * @param - cmd 	: 要写入的命令
 * @param - arg 	: 传入参数
 * @return 			: 写入的字节数，如果为负值，表示写入失败
 */
static long dev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
		case CLOSE_CMD:
			printk("close devices!\r\n");
			break;
		case OPEN_CMD:
			printk("open devices!\r\n");
			break;
		default:
			break;
	}
	return 0;
}

/* 设备操作函数 */
static struct file_operations dev_fops = {
	.owner = THIS_MODULE,
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.release = dev_release,
	.unlocked_ioctl = dev_ioctl,
};

/*
 * @description		: flatform驱动的probe函数，当驱动与
 * 					  设备匹配以后此函数就会执行
 * @param - pdev 	: platform设备
 * @return 			: 0，成功;其他负值,失败
 */
static int dev_probe(struct platform_device *pdev)
{
	int ret;

	/* 初始化自旋锁 */
	spin_lock_init(&myspinlock);

	/* 初始化等待队列头 */
	init_waitqueue_head(&r_wait);
	
	/* 初始化按键状态 */
	atomic_set(&status, KEY_KEEP); 
	
	/* 设备树解析 */
	ret = dev_parse_dt();
	if(ret)
		return ret;

	/* 注册字符设备驱动 */
	/* 1、创建设备号 */
	ret = alloc_chrdev_region(&mydev.devid, 0, DEVICE_CNT, DEVICE_NAME);	/* 申请设备号 */
	if(ret < 0) {
		pr_err("%s Couldn't alloc_chrdev_region, ret=%d\r\n", DEVICE_NAME, ret);
		goto free_gpio;
	}
	mydev.major = MAJOR(mydev.devid); /* 获取分配号的主设备号 */
	mydev.minor = MINOR(mydev.devid); /* 获取分配号的次设备号 */
	printk("devices major=%d,minor=%d\r\n",mydev.major, mydev.minor);	
	
	/* 2、初始化cdev */
	mydev.cdev.owner = THIS_MODULE;
	cdev_init(&mydev.cdev, &dev_fops);
	
	/* 3、添加一个cdev */
	ret = cdev_add(&mydev.cdev, mydev.devid, DEVICE_CNT);
	if(ret < 0)
		goto del_unregister;
		
	/* 4、创建类 */
	mydev.class = class_create(THIS_MODULE, DEVICE_NAME);
	if (IS_ERR(mydev.class)) {
		goto del_cdev;
	}

	/* 5、创建设备 */
	mydev.device = device_create(mydev.class, NULL, mydev.devid, NULL, DEVICE_NAME);
	if (IS_ERR(mydev.device)) {
		goto destroy_class;
	}
	
	/* 6、初始化timer，设置定时器处理函数,还未设置周期，所有不会激活定时器 */
	//timer_setup(&key.timer, key_timer_function, 0);
	init_timer(&timer);
	timer.function = dev_timer_function;
	//key.timer.expires = jiffies + msecs_to_jiffies(15);
	//add_timer(&my_char.my_timer);


	/* init workqueue */
	mywork_queue = create_singlethread_workqueue("my_workqueue_thread");
	//INIT_WORK(&mywork_struct, my_work_handle);
	INIT_DELAYED_WORK(&mywork_struct, my_work_handle);
	queue_delayed_work(mywork_queue, &mywork_struct,msecs_to_jiffies(3000));
	return 0;

destroy_class:
	class_destroy(mydev.class);
del_cdev:
	cdev_del(&mydev.cdev);
del_unregister:
	unregister_chrdev_region(mydev.devid, DEVICE_CNT);
free_gpio:
	free_irq(mydev.irqnum, NULL);
	gpio_free(mydev.gpionum);
	return -EIO;
}

/*
 * @description		: platform驱动的remove函数，移除platform驱动的时候此函数会执行
 * @param - dev 	: platform设备
 * @return 			: 0，成功;其他负值,失败
 */
static int dev_remove(struct platform_device *pdev)
{
	/* 注销字符设备驱动 */
	flush_workqueue(mywork_queue);
	destroy_workqueue(mywork_queue);
	del_timer_sync(&timer); 	/* 删除timer */

	free_irq(mydev.irqnum, NULL);	/* 释放中断 */
	gpio_free(mydev.gpionum);		/* 释放IO */

	cdev_del(&mydev.cdev);/*  删除cdev */
	unregister_chrdev_region(mydev.devid, DEVICE_CNT); /* 注销设备号 */	
	device_destroy(mydev.class, mydev.devid);/*注销设备 */
	class_destroy(mydev.class); 		/* 注销类 */
	return 0;
}

/* 匹配列表 */
static const struct of_device_id of_dev_match[] = {
    { .compatible = "mygpio-keys" },
    { /* Sentinel */ }
};

/* platform驱动结构体 */
static struct platform_driver mydev_driver = {
	.probe		= dev_probe,
	.remove		= dev_remove,
	.driver		= {
		.name	= "mykey_driver",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(of_dev_match),
	},
};


/* 将上面两个函数指定为驱动的入口和出口函数 */
module_platform_driver(mydev_driver);

MODULE_AUTHOR("shang yaqi <shangyaqi@morningcore.com>");
MODULE_DESCRIPTION("GPIO KEY driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:key-gpio");
