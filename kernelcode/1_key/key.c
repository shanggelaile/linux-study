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
//#include <asm/mach/map.h>
#include <asm/io.h>
#include <linux/irq.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/fs.h>
#include <linux/fcntl.h>



/* 定义按键值 */
#define KEY0VALUE		0XF0	/* 按键值 		*/
#define INVAKEY			0X00	/* 无效的按键值  */

#define DEVICE_CNT			1			/* 设备号个数 */
#define DEVICE_NAME			"key"	/* 名字 */


/*设备结构体 */
struct key_dev{
	dev_t devid;			/* 设备号 	 */
	struct cdev cdev;		/* cdev 	*/
	struct class *class;	/* 类 		*/
	struct device *device;	/* 设备 	 */
	struct device_node	*nd; /* 设备节点 */
	int major;				/* 主设备号	  */
	int minor;				/* 次设备号   */
	int gpio_key;			/*gpio number*/
	int keyvalue;		/* 按键值 		*/	
};

struct key_dev mykeydev;	/* key device */


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
	filp->private_data = &mykeydev; 	/* 设置私有数据 */
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
static ssize_t dev_read(struct file *filp, char __user *buf, size_t cnt, loff_t *offt)
{
	struct key_dev *dev = filp->private_data;
	int ret = 0;
	int value = 0;

	printk(KERN_ALERT "dev read!\r\n");
	if (gpio_get_value(dev->gpio_key) == 0) { 		/* key0按下 */
		while(!gpio_get_value(dev->gpio_key));		/* 等待按键释放 */
		dev->keyvalue = KEY0VALUE;	
	} else {	
		dev->keyvalue = INVAKEY;		/* 无效的按键值 */
	}
	printk(KERN_ALERT "dev read copy befor!\r\n");
	value = dev->keyvalue;
	ret = copy_to_user(buf, &value, sizeof(value));
	printk(KERN_ALERT "dev read copy after!\r\n");
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
 * @description		: 关闭/释放设备
 * @param - filp 	: 要关闭的设备文件(文件描述符)
 * @return 			: 0 成功;其他 失败
 */
static int dev_release(struct inode *inode, struct file *filp)
{
	//printk(KERN_ALERT "dev release！\r\n");
	return 0;
}


/* 设备操作函数 */
static struct file_operations mykeydev_fops = {
	.owner = THIS_MODULE,
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.release = 	dev_release,
};


/*
 * @description		: find devices-tree parameter
 * @param - filp 	: 要关闭的设备文件(文件描述符)
 * @return 			: 0 成功;其他 失败
 */
static int dev_init(void)
{
	int ret;
	
	/* 设置LED所使用的GPIO */
	/* 1、获取设备节点：keydev */
	mykeydev.nd = of_find_node_by_path("/mykeypad");
	if(mykeydev.nd == NULL) {
		printk("keydev node not find!\r\n");
		return -EINVAL;
	}
	mykeydev.gpio_key = of_get_named_gpio(mykeydev.nd, "gpio_key", 0);
	if(mykeydev.gpio_key < 0) {
        	printk(KERN_ERR "device: Failed to get gpio\n");
        	return -EINVAL;
    }
	
	/* 申请使用GPIO */
	ret = gpio_request(mykeydev.gpio_key, "KEY0");
    if (ret) {
        printk(KERN_ERR "led: Failed to request led-gpio\n");
        return ret;
	}
	
	/* 6、设置PG3输入模式 */
	ret = gpio_direction_input(mykeydev.gpio_key);
	if(ret < 0) {
		printk(KERN_ERR "can't set gpio!\r\n");
		return ret;
	}
	
	return 0;
}

/*
 * @description		: flatform驱动的probe函数，当驱动与
 * 					  设备匹配以后此函数就会执行
 * @param - pdev 	: platform设备
 * @return 			: 0，成功;其他负值,失败
 */
static int gpio_key_probe(struct platform_device *pdev)
{
	int ret;
	printk("key driver and device was matched!\r\n");
	dev_init();

	/* 注册字符设备驱动 */
	/* 1、创建设备号 */
	if (mykeydev.major) {		/*	定义了设备号 */
		mykeydev.devid = MKDEV(mykeydev.major, 0);
		ret = register_chrdev_region(mykeydev.devid, DEVICE_CNT, DEVICE_NAME);
		if(ret < 0) {
			pr_err("cannot register %s device driver [ret=%d]\n", DEVICE_NAME, DEVICE_CNT);
			goto free_gpio;
		}
	} else {						/* 没有定义设备号 */
		ret = alloc_chrdev_region(&mykeydev.devid, 0, DEVICE_CNT, DEVICE_NAME); /* 申请设备号 */
		if(ret < 0) {
			pr_err("%s Couldn't alloc_dev_region, ret=%d\r\n", DEVICE_NAME, ret);
			goto free_gpio;
		}
		mykeydev.major = MAJOR(mykeydev.devid); /* 获取分配号的主设备号 */
		mykeydev.minor = MINOR(mykeydev.devid); /* 获取分配号的次设备号 */
	}
	printk("devices major=%d,minor=%d\r\n",mykeydev.major, mykeydev.minor);	
	
	/* 2、初始化cdev */
	mykeydev.cdev.owner = THIS_MODULE;
	cdev_init(&mykeydev.cdev, &mykeydev_fops);
	
	/* 3、添加一个cdev */
	cdev_add(&mykeydev.cdev, mykeydev.devid, DEVICE_CNT);
	if(ret < 0)
		goto del_unregister;
		
	/* 4、创建类 */
	mykeydev.class = class_create(THIS_MODULE, DEVICE_NAME);
	if (IS_ERR(mykeydev.class)) {
		goto del_cdev;
	}

	/* 5、创建设备 */
	mykeydev.device = device_create(mykeydev.class, NULL, mykeydev.devid, NULL, DEVICE_NAME);
	if (IS_ERR(mykeydev.device)) {
		goto destroy_class;
	}

	return 0;
	
	
destroy_class:
	class_destroy(mykeydev.class);
del_cdev:
	cdev_del(&mykeydev.cdev);
del_unregister:
	unregister_chrdev_region(mykeydev.devid, DEVICE_CNT);
free_gpio:
	gpio_free(mykeydev.gpio_key);
	return -EIO;
}

/*
 * @description		: platform驱动的remove函数，移除platform驱动的时候此函数会执行
 * @param - dev 	: platform设备
 * @return 			: 0，成功;其他负值,失败
 */
static int gpio_key_remove(struct platform_device *pdev)
{
	
	/* 出口函数具体内容 */
	/* 注销字符设备驱动 */
	gpio_free(mykeydev.gpio_key);
	cdev_del(&mykeydev.cdev);/*  删除cdev */
	unregister_chrdev_region(mykeydev.devid, DEVICE_CNT); /* 注销设备号 */
	device_destroy(mykeydev.class, mykeydev.devid);/* 注销设备 */
	class_destroy(mykeydev.class);/* 注销类 */
	return 0;
}


/* 匹配列表 */
static const struct of_device_id of_gpio_keys_match[] = {
    { .compatible = "mygpio-keys" },
    { /* Sentinel */ }
};

/* platform驱动结构体 */
static struct platform_driver mykey_driver = {
	.probe		= gpio_key_probe,
	.remove		= gpio_key_remove,
	.driver		= {
		.name	= "mykey_driver",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(of_gpio_keys_match),
	},
};


/* 将上面两个函数指定为驱动的入口和出口函数 */
module_platform_driver(mykey_driver);


MODULE_AUTHOR("shang yaqi <shangyaqi@morningcore.com>");
MODULE_DESCRIPTION("GPIO KEY driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:key-gpio");


