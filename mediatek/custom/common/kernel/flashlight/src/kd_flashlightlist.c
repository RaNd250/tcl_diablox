#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/time.h>
#include <linux/timer.h>
#include "kd_flashlight.h"
#include <asm/io.h>
#include <asm/uaccess.h>
#include "kd_camera_hw.h"

#define USE_UNLOCKED_IOCTL

#define AS3647_REG_ChipID 0x00
#define AS3647_REG_Current_Set_LED 0x01
#define AS3647_REG_TXMask 0x03
#define AS3647_REG_Low_Voltage 0x04
#define AS3647_REG_Flash_Timer 0x05
#define AS3647_REG_Control 0x06
#define AS3647_REG_Strobe_Sigalling 0x07
#define AS3647_REG_Fault 0x08
#define AS3647_REG_PWM_and_Indicator 0x09
//#define AS3647_REG_ChipID 0x0E
//#define AS3647_REG_ChipID 0x0F


//add new flashlight driver here
//export funtions
MUINT32 defaultFlashlightInit(PFLASHLIGHT_FUNCTION_STRUCT *pfFunc);
MUINT32 dummyFlashlightInit(PFLASHLIGHT_FUNCTION_STRUCT *pfFunc);
MUINT32 peakFlashlightInit(PFLASHLIGHT_FUNCTION_STRUCT *pfFunc);
MUINT32 torchFlashlightInit(PFLASHLIGHT_FUNCTION_STRUCT *pfFunc);
MUINT32 constantFlashlightInit(PFLASHLIGHT_FUNCTION_STRUCT *pfFunc);


KD_FLASHLIGHT_INIT_FUNCTION_STRUCT kdFlashlightList[] =
{
    {KD_DEFAULT_FLASHLIGHT_ID, defaultFlashlightInit},
#if defined(DUMMY_FLASHLIGHT)
	{KD_DUMMY_FLASHLIGHT_ID, dummyFlashlightInit},
#endif
#if defined(PEAK_FLASHLIGHT)
	{KD_PEAK_FLASHLIGHT_ID, peakFlashlightInit},
#endif
#if defined(TORCH_FLASHLIGHT)
	{KD_TORCH_FLASHLIGHT_ID, torchFlashlightInit},
#endif
#if defined(CONSTANT_FLASHLIGHT)
	{KD_CONSTANT_FLASHLIGHT_ID, constantFlashlightInit},
#endif


/*  ADD flashlight driver before this line */
    {0,NULL}, //end of list
};
//e_add new flashlight driver here
/******************************************************************************
 * Definition
******************************************************************************/
#ifndef TRUE
#define TRUE KAL_TRUE
#endif
#ifndef FALSE
#define FALSE KAL_FALSE
#endif
static FLASHLIGHT_FUNCTION_STRUCT *g_pFlashlightFunc = NULL; //zhfan move here for compile OK pr458716


/*=======================================================================
  * FL sysfs zhfan begin pr458716
  * zhfan PR459644
  * =>modify sysfs permission from all WR to other user cannot write
  *=======================================================================*/
#define FL_SYSFS_SUPPORT
#ifdef FL_SYSFS_SUPPORT
static ssize_t FL_value_show(struct kobject *kobj, struct kobj_attribute *attr,
			  char *buf)
{
	char *s = buf;
    int ret;
    U8 val[4];


	return (s - buf);
}


static ssize_t FL_value_store(struct kobject *kobj, struct kobj_attribute *attr,
			   const char *buf, size_t n)
{
  char *s = buf;
  int retval;
  ssize_t ret_val;
  char cmd_str[16] = {0};
  int val;
  sscanf(buf, "%s %d", (char *)&cmd_str, &val);

  if(strcmp(cmd_str, "torch") == 0)
  {
      g_pFlashlightFunc->flashlight_ioctl(FLASHLIGHT_TORCH_ENG,val);
  }

  if(strcmp(cmd_str, "flash") == 0)
  {
      g_pFlashlightFunc->flashlight_ioctl(FLASHLIGHT_FLASHLIGHT_ENG,val);
  }
	return n;
}



static DEVICE_ATTR(FL_DEBUG, 0644,  FL_value_show, FL_value_store);  //0666->0664

static struct attribute *FL_sysfs_attrs[] = {
    &dev_attr_FL_DEBUG.attr,
    NULL,
};
static struct attribute_group FL_attr_group = {
        .attrs = FL_sysfs_attrs,
};

//add sysfs
struct kobject *FL_ctrl_kobj;
static int FL_sysfs_init(void)
{
	FL_ctrl_kobj = kobject_create_and_add("FL", NULL);
	if (!FL_ctrl_kobj)
		return -ENOMEM;

	return sysfs_create_group(FL_ctrl_kobj, &FL_attr_group);
}
//remove sysfs
static void FL_sysfs_exit(void)
{
	sysfs_remove_group(FL_ctrl_kobj, &FL_attr_group);

	kobject_put(FL_ctrl_kobj);
}
#endif
//FL sys End


/*************For as3647 flash ic S******************/
static struct i2c_client * g_pstAS3647_I2Cclient = NULL;

static int AS3647_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id);

int s4AS3647_read_byte(u8 addr, u8 *data)
{
#if 1
	u8 buf;
	int ret = 0;

	buf = addr;
	ret = i2c_master_send(g_pstAS3647_I2Cclient, &buf, 1);
	if (ret < 0) {
		printk("s4AS3647_read_byte send command error!!  ret=%d\n",ret);
		return -EFAULT;
	}
	ret = i2c_master_recv(g_pstAS3647_I2Cclient, &buf, 1);
	if (ret < 0) {
		printk("s4AS3647_read_byte reads data error!!  ret=%d\n",ret);
		return -EFAULT;
	}

	*data = buf;
	return 0;
	#else
	int  i4RetValue = 0;
	i4RetValue = iReadReg(addr,data,OV8850AF_VCM_WRITE_ID);
	if (i4RetValue < 0) 	{
		OV8850AFDB("s4OV8850AF_read_byte failed--Mbyte!! \n");
		return -1;
	}

	return 0;
	#endif
}

int s4AS3647_write_byte(u8 addr, u8 data)
{
#if 1
	u8 buf[] = {addr, data};
	int ret = 0;

	ret = i2c_master_send(g_pstAS3647_I2Cclient, (const char*)buf, sizeof(buf));
	if (ret < 0) {
	    printk("s4AS3647_write_byte send data error!!   ret=%d\n",ret);
	    return -EFAULT;
	}

	return 0;
#else
	int  i4RetValue = 0;
	i4RetValue = iWriteReg(addr,(u32)data,1,OV8850AF_VCM_WRITE_ID);
	if (i4RetValue < 0) {
	    OV8850AFDB("s4OV8850AF_write_byte  send data error!!   i4RetValue=%d\n",i4RetValue);
	    return -EFAULT;
	}
	return 0;
#endif
}
/*************For as3647 flash ic E******************/

/* device name and major number */
#define FLASHLIGHT_DEVNAME            "kd_camera_flashlight"
/*For A3647 FLASHLED IC register*/
#define LENS_I2C_BUSNUM 4
#define AS3647_DEVNAME "AS3647" 
static struct i2c_board_info __initdata flashled_dev={ I2C_BOARD_INFO(AS3647_DEVNAME, 0x60>>1)};


#define DELAY_MS(ms) {mdelay(ms);}//unit: ms(10^-3)
#define DELAY_US(us) {mdelay(us);}//unit: us(10^-6)
#define DELAY_NS(ns) {mdelay(ns);}//unit: ns(10^-9)
/*
    non-busy dealy(/kernel/timer.c)(CANNOT be used in interrupt context):
        ssleep(sec)
        msleep(msec)
        msleep_interruptible(msec)

    kernel timer


*/
/******************************************************************************
 * Debug configuration
******************************************************************************/
#define PFX "[KD_CAMERA_FLASHLIGHT]"
#define PK_DBG_NONE(fmt, arg...)    do {} while (0)
#define PK_DBG_FUNC(fmt, arg...)    printk(KERN_INFO PFX "%s: " fmt, __FUNCTION__ ,##arg)

#define PK_WARN(fmt, arg...)        printk(KERN_WARNING PFX "%s: " fmt, __FUNCTION__ ,##arg)
#define PK_NOTICE(fmt, arg...)      printk(KERN_NOTICE PFX "%s: " fmt, __FUNCTION__ ,##arg)
#define PK_INFO(fmt, arg...)        printk(KERN_INFO PFX "%s: " fmt, __FUNCTION__ ,##arg)
#define PK_TRC_FUNC(f)              printk(PFX "<%s>\n", __FUNCTION__);
#define PK_TRC_VERBOSE(fmt, arg...) printk(PFX fmt, ##arg)

#define DEBUG_KD_CAMERA_FLASHLIGHT
#ifdef DEBUG_KD_CAMERA_FLASHLIGHT
#define PK_DBG PK_DBG_FUNC
#define PK_TRC PK_DBG_NONE //PK_TRC_FUNC
#define PK_VER PK_DBG_NONE //PK_TRC_VERBOSE
#define PK_ERR(fmt, arg...)         printk(KERN_ERR PFX "%s: " fmt, __FUNCTION__ ,##arg)
#else
#define PK_DBG(a,...)
#define PK_TRC(a,...)
#define PK_VER(a,...)
#define PK_ERR(a,...)
#endif
/*****************************************************************************

*****************************************************************************/
/*****************************************************************************

*****************************************************************************/
MINT32 default_flashlight_open(void *pArg) {
    PK_DBG("[default_flashlight_open] E\n");
    return 0;
}
MINT32 default_flashlight_release(void *pArg) {
    PK_DBG("[default_flashlight_release] E\n");
    return 0;
}
MINT32 default_flashlight_ioctl(MUINT32 cmd, MUINT32 arg) {
    int i4RetValue = 0;
    int iFlashType = (int)FLASHLIGHT_NONE;

    switch(cmd)
    {
        case FLASHLIGHTIOC_G_FLASHTYPE:
            iFlashType = FLASHLIGHT_NONE;
            if(copy_to_user((void __user *) arg , (void*)&iFlashType , _IOC_SIZE(cmd)))
            {
                PK_DBG("[default_flashlight_ioctl] ioctl copy to user failed\n");
                return -EFAULT;
            }
            break;
    	default :
    		PK_DBG("[default_flashlight_ioctl]\n");
    		break;
    }
    return i4RetValue;
}

FLASHLIGHT_FUNCTION_STRUCT	defaultFlashlightFunc=
{
	default_flashlight_open,
	default_flashlight_release,
	default_flashlight_ioctl,
};

UINT32 defaultFlashlightInit(PFLASHLIGHT_FUNCTION_STRUCT *pfFunc) { 
    if (pfFunc!=NULL) {
        *pfFunc=&defaultFlashlightFunc;
    }
    return 0;
}
/*******************************************************************************
* kdSetDriver
********************************************************************************/
int kdSetFlashlightDrv(unsigned int *pFlashlightIdx)
{
unsigned int flashlightIdx = *pFlashlightIdx;
    PK_DBG("[kdSetFlashlightDrv] flashlightIdx: %d \n",flashlightIdx);
    
    if (NULL != kdFlashlightList[flashlightIdx].flashlightInit) {
        kdFlashlightList[flashlightIdx].flashlightInit(&g_pFlashlightFunc);
        if (NULL == g_pFlashlightFunc) {
            PK_DBG("[kdSetFlashlightDrv] flashlightIdx init fail\n");
            if (NULL != kdFlashlightList[KD_DEFAULT_FLASHLIGHT_INDEX].flashlightInit) {
                kdFlashlightList[KD_DEFAULT_FLASHLIGHT_INDEX].flashlightInit(&g_pFlashlightFunc);
                if (NULL == g_pFlashlightFunc) {
                    PK_DBG("[kdSetFlashlightDrv] KD_DEFAULT_FLASHLIGHT_INDEX init fail\n");
                    return -EIO;
                }
            }
        }
    }

    //open flashlight driver
    if (g_pFlashlightFunc) {
       g_pFlashlightFunc->flashlight_open(0);
    }

	return 0;
}
/*****************************************************************************

*****************************************************************************/
#ifdef USE_UNLOCKED_IOCTL
static long flashlight_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
#else
static int flashlight_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
#endif
{
    int i4RetValue = 0;

    //PK_DBG("%x, %x \n",cmd,arg);

    switch(cmd)
    {
        case FLASHLIGHTIOC_X_SET_DRIVER:
            i4RetValue = kdSetFlashlightDrv((unsigned int*)&arg);
            break;
    	default :
    	    if (g_pFlashlightFunc) {
    	        i4RetValue = g_pFlashlightFunc->flashlight_ioctl(cmd,arg);
    	    }
    		break;
    }

    return i4RetValue;
}

static int flashlight_open(struct inode *inode, struct file *file)
{	
	
    int i4RetValue = 0;
    PK_DBG("[flashlight_open] E\n");
	//hwPowerOn(MT65XX_POWER_LDO_VCAM_IO, VOL_1800,"flashlight");
    return i4RetValue;
}

static int flashlight_release(struct inode *inode, struct file *file)
{
    PK_DBG("[flashlight_release] E\n");

    if (g_pFlashlightFunc) {
        g_pFlashlightFunc->flashlight_release(0);
        g_pFlashlightFunc = NULL;
    }

    return 0;
}

/*****************************************************************************/
/* Kernel interface */
static struct file_operations flashlight_fops = {
    .owner      = THIS_MODULE,
#ifdef USE_UNLOCKED_IOCTL
    .unlocked_ioctl      = flashlight_ioctl,
#else
    .ioctl      = flashlight_ioctl,
#endif
    .open       = flashlight_open,
    .release    = flashlight_release,
};

/*****************************************************************************
Driver interface
*****************************************************************************/
struct flashlight_data{
    spinlock_t lock;
    wait_queue_head_t read_wait;
    struct semaphore sem;
};
static struct class *flashlight_class = NULL;
static struct device *flashlight_device = NULL;
static struct flashlight_data flashlight_private;
static dev_t flashlight_devno;
static struct cdev flashlight_cdev;
/*****************************************************************************
AS3647 driver define
*****************************************************************************/
static int AS3647_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	u8 as3647id = 0;
	u8 tmp = 0;
	u8 id1 = 0;
	int i4RetValue = 0;
	
	printk("[AS3647_i2c_probe]  Attach I2C   \n");

	printk("AS3647_i2c_probe client->timing=%x,addr=%x \n",client->timing,client->addr);

	g_pstAS3647_I2Cclient = client;	
	g_pstAS3647_I2Cclient->ext_flag=((g_pstAS3647_I2Cclient->ext_flag ) & I2C_MASK_FLAG ) | I2C_DIRECTION_FLAG;

	s4AS3647_read_byte(0x00, &as3647id);
	
	printk("xxxxxxxxxxxxxxxregister 0x00  = %d\n xxxxxxxxxxxxxx",as3647id);

	return 0;
}

static int AS3647_i2c_remove()
{
	return 0;
}
static const struct i2c_device_id AS3647_i2c_id[] = {{AS3647_DEVNAME,0},{}};   
struct i2c_driver AS3647_i2c_driver = {                       
    .probe = AS3647_i2c_probe,                                   
    .remove = AS3647_i2c_remove,                           
    .driver.name = AS3647_DEVNAME,                 
    .id_table = AS3647_i2c_id,                             
};  
/****************************************************************************/
#define ALLOC_DEVNO
static int flashlight_probe(struct platform_device *dev)
{
    int ret = 0, err = 0;

	PK_DBG("[flashlight_probe] start\n");
	/*AS3647 I2C add diver*/
	if(i2c_add_driver(&AS3647_i2c_driver)!=0)
   		 {
       		 printk("[fan5405_init] failed to register fan5405 i2c driver.\n");
    		}
    	else
   		 {
       		 printk("[fan5405_init] Success to register fan5405 i2c driver.\n");
    		}

#ifdef ALLOC_DEVNO
    ret = alloc_chrdev_region(&flashlight_devno, 0, 1, FLASHLIGHT_DEVNAME);
    if (ret) {
        PK_ERR("[flashlight_probe] alloc_chrdev_region fail: %d\n", ret);
        goto flashlight_probe_error;
    } else {
        PK_DBG("[flashlight_probe] major: %d, minor: %d\n", MAJOR(flashlight_devno), MINOR(flashlight_devno));
    }
    cdev_init(&flashlight_cdev, &flashlight_fops);
    flashlight_cdev.owner = THIS_MODULE;
    err = cdev_add(&flashlight_cdev, flashlight_devno, 1);
    if (err) {
        PK_ERR("[flashlight_probe] cdev_add fail: %d\n", err);
        goto flashlight_probe_error;
    }
#else
    #define FLASHLIGHT_MAJOR 242
    ret = register_chrdev(FLASHLIGHT_MAJOR, FLASHLIGHT_DEVNAME, &flashlight_fops);
    if (ret != 0) {
        PK_ERR("[flashlight_probe] Unable to register chardev on major=%d (%d)\n", FLASHLIGHT_MAJOR, ret);
        return ret;
    }
    flashlight_devno = MKDEV(FLASHLIGHT_MAJOR, 0);
#endif


    flashlight_class = class_create(THIS_MODULE, "flashlightdrv");
    if (IS_ERR(flashlight_class)) {
        PK_ERR("[flashlight_probe] Unable to create class, err = %d\n", (int)PTR_ERR(flashlight_class));
        goto flashlight_probe_error;
    }

    flashlight_device = device_create(flashlight_class, NULL, flashlight_devno, NULL, FLASHLIGHT_DEVNAME);
    if(NULL == flashlight_device){
        PK_ERR("[flashlight_probe] device_create fail\n");
        goto flashlight_probe_error;
    }

    /*initialize members*/
    spin_lock_init(&flashlight_private.lock);
    init_waitqueue_head(&flashlight_private.read_wait);
    //init_MUTEX(&flashlight_private.sem);
    sema_init(&flashlight_private.sem, 1);

    PK_DBG("[flashlight_probe] Done\n");
    return 0;

flashlight_probe_error:
#ifdef ALLOC_DEVNO
    if (err == 0)
        cdev_del(&flashlight_cdev);
    if (ret == 0)
        unregister_chrdev_region(flashlight_devno, 1);
#else
    if (ret == 0)
        unregister_chrdev(MAJOR(flashlight_devno), FLASHLIGHT_DEVNAME);
#endif
    return -1;
}

static int flashlight_remove(struct platform_device *dev)
{

    PK_DBG("[flashlight_probe] start\n");
    i2c_del_driver(&AS3647_i2c_driver);
	
#ifdef ALLOC_DEVNO
    cdev_del(&flashlight_cdev);
    unregister_chrdev_region(flashlight_devno, 1);
#else
    unregister_chrdev(MAJOR(flashlight_devno), FLASHLIGHT_DEVNAME);
#endif
    device_destroy(flashlight_class, flashlight_devno);
    class_destroy(flashlight_class);

    PK_DBG("[flashlight_probe] Done\n");
    return 0;
}


static struct platform_driver flashlight_platform_driver =
{
    .probe      = flashlight_probe,
    .remove     = flashlight_remove,
    .driver     = {
        .name = FLASHLIGHT_DEVNAME,
		.owner	= THIS_MODULE,
    },
};

static struct platform_device flashlight_platform_device = {
    .name = FLASHLIGHT_DEVNAME,
    .id = 0,
    .dev = {
    }
};

static int __init flashlight_init(void)
{
    int ret = 0;

    PK_DBG("[flashlight_probe] start\n");
	
	i2c_register_board_info(LENS_I2C_BUSNUM, &flashled_dev, 1);

    FL_sysfs_init(); //zhfan FL sys pr 458716

	ret = platform_device_register (&flashlight_platform_device);
	if (ret) {
        PK_ERR("[flashlight_probe] platform_device_register fail\n");
        return ret;
	}

    ret = platform_driver_register(&flashlight_platform_driver);
	if(ret){
		PK_ERR("[flashlight_probe] platform_driver_register fail\n");
		return ret;
	}

	PK_DBG("[flashlight_probe] done!\n");
    return ret;
}

static void __exit flashlight_exit(void)
{
    PK_DBG("[flashlight_probe] start\n");
    platform_driver_unregister(&flashlight_platform_driver);
    //to flush work queue
    //flush_scheduled_work();
    PK_DBG("[flashlight_probe] done!\n");
}

/*****************************************************************************/
module_init(flashlight_init);
module_exit(flashlight_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jackie Su <jackie.su@mediatek.com>");
MODULE_DESCRIPTION("Flashlight control Driver");


