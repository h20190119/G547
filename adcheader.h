#ifndef CHARDEV_H
#define CHARDEV_H

#include<linux/ioctl.h> 

#define DEVICE_NUM 63

#define IOCTL_CHANNEL_SELECTION _IOR(DEVICE_NUM,0, int*) //adc channel selection

#define IOCTL_ALIGNMENT _IOR(DEVICE_NUM,1,char*) //adc alignment selection

#define FILE_NAME "/dev/adc8" //defined mydevice

#endif


