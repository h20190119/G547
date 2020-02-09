#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/types.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
#include<linux/device.h>
#include<linux/cdev.h>
#include<linux/init.h>
#include<linux/random.h>
#include<linux/version.h>
#include<linux/uaccess.h>
#include "adcheader.h"

static dev_t adc8; //variable for device number

static struct cdev c_dev; //variable for character device structure
static struct class *cls; //variable for device class
static uint16_t channel=0; //default channel value is zero
static int alignment=0; //default alignment is LSB



//step4 : Driver callback functions

static int my_open(struct inode *i, struct file *f)
{
	printk(KERN_INFO "mychar: open()\n");
	return 0;
}

static int my_close(struct inode *i, struct file *f)
{
	printk(KERN_INFO "mychar: close()\n");
	return 0;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
	uint16_t adc;	
	get_random_bytes(&adc,sizeof(adc));
	adc%=1023;
	
	if(alignment==1)// 0=LSB & 1=MSB
	{
		adc=adc*64;
	}

	printk(KERN_INFO "Channel Selected=%u\n,ALIGNMENT=%d\n, And random number is %i",channel, alignment,adc);

	if(copy_to_user(buf,&adc,sizeof(adc)))
	{
		return(sizeof(adc));
	}

	return(sizeof(adc));
}

static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{

	switch(cmd)
	{
		case IOCTL_CHANNEL_SELECTION:
			channel=arg;
			printk(KERN_INFO "Channel Selected=%u",channel);
			break;

		case IOCTL_ALIGNMENT:
			alignment=arg;
			printk(KERN_INFO "Alignment=%d",alignment);
			break;
	}
	return 0;
}

static struct file_operations fops =
				{
					.owner         = THIS_MODULE,
					.open          = my_open,
					.release       = my_close,
					.read          = my_read,
					.unlocked_ioctl= my_ioctl
				};
static int __init mychar_init(void)
{
	printk(KERN_INFO "mychar driver registered\n");
	
	//step1: reserve major and minor number
	if(alloc_chrdev_region(&adc8,0,1, "BITS.PILANI") < 0)
	{
		return -1;
	}

	//step2: creation of device file
	if((cls=class_create(THIS_MODULE, "chardrv"))==NULL)
	{
		unregister_chrdev_region(adc8,1);
		return -1; 
	}
	if(device_create(cls, NULL, adc8, NULL, "adc8")==NULL)
	{
		class_destroy(cls);
		unregister_chrdev_region(adc8,1);
		return -1; 
	}
	//STEP3: Link fops and cdev to the device node
	cdev_init(&c_dev, &fops);
	if(cdev_add(&c_dev, adc8, 1)==-1)
	{
		device_destroy(cls, adc8);
		class_destroy(cls);
		unregister_chrdev_region(adc8,1);
		return -1; 
	}
	return 0;
}

static void __exit mychar_exit(void)
{

	cdev_del(&c_dev);
	device_destroy(cls, adc8);
	class_destroy(cls);
	unregister_chrdev_region(adc8,1);
	printk(KERN_INFO "Bye: mychar driver unregistered\n\n");
}

module_init(mychar_init);
module_exit(mychar_exit);

MODULE_DESCRIPTION("Our first module");
MODULE_AUTHOR("Rishav Singh<rishavsinghh30@gmail.com>");
MODULE_LICENSE("GPL");
MODULE_INFO(SupportedChips, "PCA9685, FT232RL");
	

