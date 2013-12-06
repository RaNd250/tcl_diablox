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
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/time.h>
#include "kd_flashlight.h"
#include <asm/io.h>
#include <asm/uaccess.h>
#include "kd_camera_hw.h"
#include <cust_gpio_usage.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/xlog.h>
#include <linux/version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))
#include <linux/mutex.h>
#else
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
#include <linux/semaphore.h>
#else
#include <asm/semaphore.h>
#endif
#endif



/******************************************************************************
 * Debug configuration
******************************************************************************/
// availible parameter
// ANDROID_LOG_ASSERT
// ANDROID_LOG_ERROR
// ANDROID_LOG_WARNING
// ANDROID_LOG_INFO
// ANDROID_LOG_DEBUG
// ANDROID_LOG_VERBOSE
#define TAG_NAME "leds_strobe.c"
#define PK_DBG_NONE(fmt, arg...)    do {} while (0)
#define PK_DBG_FUNC(fmt, arg...)    xlog_printk(ANDROID_LOG_DEBUG  , TAG_NAME, KERN_INFO  "%s: " fmt, __FUNCTION__ ,##arg)
#define PK_WARN(fmt, arg...)        xlog_printk(ANDROID_LOG_WARNING, TAG_NAME, KERN_WARNING  "%s: " fmt, __FUNCTION__ ,##arg)
#define PK_NOTICE(fmt, arg...)      xlog_printk(ANDROID_LOG_DEBUG  , TAG_NAME, KERN_NOTICE  "%s: " fmt, __FUNCTION__ ,##arg)
#define PK_INFO(fmt, arg...)        xlog_printk(ANDROID_LOG_INFO   , TAG_NAME, KERN_INFO  "%s: " fmt, __FUNCTION__ ,##arg)
#define PK_TRC_FUNC(f)              xlog_printk(ANDROID_LOG_DEBUG  , TAG_NAME,  "<%s>\n", __FUNCTION__);
#define PK_TRC_VERBOSE(fmt, arg...) xlog_printk(ANDROID_LOG_VERBOSE, TAG_NAME,  fmt, ##arg)
#define PK_ERROR(fmt, arg...)       xlog_printk(ANDROID_LOG_ERROR  , TAG_NAME, KERN_ERR "%s: " fmt, __FUNCTION__ ,##arg)


#define DEBUG_LEDS_STROBE
#ifdef  DEBUG_LEDS_STROBE
	#define PK_DBG PK_DBG_FUNC
	#define PK_VER PK_TRC_VERBOSE
	#define PK_ERR PK_ERROR
#else
	#define PK_DBG(a,...)
	#define PK_VER(a,...)
	#define PK_ERR(a,...)
#endif

/******************************************************************************
 * local variables
******************************************************************************/

static DEFINE_SPINLOCK(g_strobeSMPLock); /* cotta-- SMP proection */


static u32 strobe_Res = 0;
static u32 strobe_Timeus = 0;
static BOOL g_strobe_On = 0;
static u32 strobe_width = 100; //0 is disable

static int g_duty=-1;
static int g_timeOutTimeMs=0;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))
static DEFINE_MUTEX(g_strobeSem);
#else
static DECLARE_MUTEX(g_strobeSem);
#endif


#define STROBE_DEVICE_ID 0x60


static struct work_struct workTimeOut;

/*****************************************************************************
Functions
*****************************************************************************/
extern int iWriteRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u16 i2cId);
extern int iReadRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u8 * a_pRecvData, u16 a_sizeRecvData, u16 i2cId);
extern int s4AS3647_read_byte(u8 addr, u8 *data);
extern int s4AS3647_write_byte(u8 addr, u8 data);
static void work_timeOutFunc(struct work_struct *data);



int FL_Enable(void)
{
	char buf[2];
	int reg6;
	int mode_setting=2;
	PK_DBG(" FL_Enable\n");
	if(g_duty>=15)
		mode_setting=3;
	reg6 = 8+mode_setting;
	buf[0]=6;
	buf[1]=reg6;
	//iWriteRegI2C(buf , 2, STROBE_DEVICE_ID);
	s4AS3647_write_byte(buf[0], buf[1]);
    return 0;
}

int FL_Disable(void)
{

	char buf[2];
	int reg6;
	int mode_setting=2;
	PK_DBG(" FL_Disablexx1\n");
	reg6 = mode_setting;
	buf[0]=6;
	buf[1]=reg6;
	//iWriteRegI2C(buf , 2, STROBE_DEVICE_ID);
	s4AS3647_write_byte(buf[0], buf[1]);
    return 0;
}

int FL_dim_duty(kal_uint32 duty)
{
	char buf[2];
	int reg1;
    PK_DBG(" strobe duty : %u\n",duty);
    if(duty<0)
    	duty=0;
    if(duty>=31)
    	duty=31;
    g_duty=duty;
    //[begin]467220 zhfan 2013-6-10 modify the value for each duty
    reg1=(g_duty+1)*5;
    if(reg1>160)
    	reg1=160;
    //[end ]467220 zhfan  2013-6-10
    buf[0]=1;
	buf[1]=reg1;
	//iWriteRegI2C(buf , 2, STROBE_DEVICE_ID);
	s4AS3647_write_byte(buf[0], buf[1]);
    return 0;
}


int FL_Init(void)
{


	//down(&g_strobeSem);
	//up(&g_strobeSem);
	char buf[2];
    //spin_lock(&g_strobeSMPLock);

    PK_DBG(" FL_Init line=%d\n",__LINE__);

    buf[0]=5;
	buf[1]=220;
	//iWriteRegI2C(buf , 2, STROBE_DEVICE_ID); //timeout 1s
	s4AS3647_write_byte(buf[0], buf[1]);

	buf[0]=7;
	buf[1]=0;
	//iWriteRegI2C(buf , 2, STROBE_DEVICE_ID); //strobe_on = 0
	s4AS3647_write_byte(buf[0], buf[1]);

	 INIT_WORK(&workTimeOut, work_timeOutFunc);

    //spin_unlock(&g_strobeSMPLock);

    PK_DBG(" FL_Init line=%d\n",__LINE__);
    return 0;
}

//[Begin]zhfan FL sys pr458716
void TORCH_eng(int val)
{
    s4AS3647_write_byte(0x01, val);
    s4AS3647_write_byte(0x06, 0x0A);
    msleep(10);
}

void FL_eng(int val)
{
    s4AS3647_write_byte(0x01, val);
    s4AS3647_write_byte(0x06, 0x0B);
    msleep(10);
}
//[End]

int FL_Uninit(void)
{
	FL_Disable();
    return 0;
}

ssize_t TORCH_select(void)
{
	//static struct pwm_easy_config pwm_setting;
#if 0
	if(TRUE != hwPowerOn(MT65XX_POWER_LDO_VCAM_IO, VOL_1800,"flashlight"))
		{
        		printk("[CAMERA SENSOR] Fail to enable analog power\n");
      		return -EIO;
  		  }   
#endif
	msleep(1);
	/*
	s4AS3647_write_byte(0x01, 0x30);
	s4AS3647_write_byte(0x03, 0x62);
	s4AS3647_write_byte(0x04, 0x00);
	s4AS3647_write_byte(0x05, 0x23);
	s4AS3647_write_byte(0x06, 0x00);
	s4AS3647_write_byte(0x07, 0x00);
	msleep(10);
	
	mt_set_gpio_mode(GPIO149, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO149, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO149, GPIO_OUT_ONE);
	*/
	switch(strobe_width)
			{
			case 30:
				s4AS3647_write_byte(0x01, 0x10);
				break;
			case 60:
				s4AS3647_write_byte(0x01, 0x20);
				break;
			case 99:
				s4AS3647_write_byte(0x01, 0x2c);
				break;
			default :
					break;
			}
	msleep(10);
	s4AS3647_write_byte(0x06, 0x0A);

	return 0;
}

ssize_t TORCH_Disable(void) {
	/*
	mt_set_gpio_mode(GPIO149, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO149, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO149, GPIO_OUT_ZERO);

	mt_set_gpio_mode(GPIO157, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO157, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO157, GPIO_OUT_ZERO);
	*/
	s4AS3647_write_byte(0x06, 0x02);

	//hwPowerDown(MT65XX_POWER_LDO_VCAM_IO, "flashlight");
	return 0;
}

/*****************************************************************************
User interface
*****************************************************************************/

static void work_timeOutFunc(struct work_struct *data)
{
    FL_Disable();
    PK_DBG("ledTimeOut_callback\n");
    //printk(KERN_ALERT "work handler function./n");
}



enum hrtimer_restart ledTimeOutCallback(struct hrtimer *timer)
{
    schedule_work(&workTimeOut);
    return HRTIMER_NORESTART;
}
static struct hrtimer g_timeOutTimer;
void timerInit(void)
{
	g_timeOutTimeMs=1000; //1s
	hrtimer_init( &g_timeOutTimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL );
	g_timeOutTimer.function=ledTimeOutCallback;

}



static int constant_flashlight_ioctl(MUINT32 cmd, MUINT32 arg)
{
	int i4RetValue = 0;
	int iFlashType = (int)FLASHLIGHT_NONE;
	int ior_shift;
	int iow_shift;
	int iowr_shift;
	ior_shift = cmd - (_IOR(FLASHLIGHT_MAGIC,0, int));
	iow_shift = cmd - (_IOW(FLASHLIGHT_MAGIC,0, int));
	iowr_shift = cmd - (_IOWR(FLASHLIGHT_MAGIC,0, int));
	PK_DBG("constant_flashlight_ioctl() line=%d ior_shift=%d, iow_shift=%d iowr_shift=%d arg=%d\n",__LINE__, ior_shift, iow_shift, iowr_shift, arg);
    switch(cmd)
    {

		case FLASH_IOC_SET_TIME_OUT_TIME_MS:
			PK_DBG("FLASH_IOC_SET_TIME_OUT_TIME_MS: %d\n",arg);
			g_timeOutTimeMs=arg;
		break;


    	case FLASH_IOC_SET_DUTY :
    		PK_DBG("FLASHLIGHT_DUTY: %d\n",arg);
    		FL_dim_duty(arg);
    		break;


    	case FLASH_IOC_SET_STEP:
    		PK_DBG("FLASH_IOC_SET_STEP: %d\n",arg);

    		break;

    	case FLASH_IOC_SET_ONOFF :
    		PK_DBG("FLASHLIGHT_ONOFF: %d\n",arg);
    		if(arg==1)
    		{
				if(g_timeOutTimeMs!=0)
	            {
	            	ktime_t ktime;
					ktime = ktime_set( 0, g_timeOutTimeMs*1000000 );
					hrtimer_start( &g_timeOutTimer, ktime, HRTIMER_MODE_REL );
	            }
    			FL_Enable();
    		}
    		else
    		{
    			FL_Disable();
				hrtimer_cancel( &g_timeOutTimer );
    		}
    		break;
			
        case FLASHLIGHTIOC_G_FLASHTYPE:
            iFlashType = FLASHLIGHT_LED_CONSTANT;
            if(copy_to_user((void __user *) arg , (void*)&iFlashType , _IOC_SIZE(cmd)))
            {
                printk("[strobe_ioctl] ioctl copy to user failed\n");
                return -EFAULT;
            }
            break;
			
        case FLASHLIGHTIOC_X_SET_FLASHLEVEL:
			strobe_width = arg;
            printk("level:%d \n",(int)arg);
	    	break;

		case FLASHLIGHTIOC_ENABLE_STATUS:
			//printk("**********g_strobe_on = %d \n", g_strobe_On);
			copy_to_user((void __user *) arg , (void*)&g_strobe_On , sizeof(int));
			break;
			
	    case FLASHLIGHT_TORCH_SELECT:
			printk("FLASHLIGHT_TORCH_SELECT: %d\n", arg);
			if (arg && strobe_width){
				TORCH_select();
				g_strobe_On = TRUE;
			}
			else {
				if(TORCH_Disable()) {
	                printk("FL_Disable fail!\n");
	                goto strobe_ioctl_error;
	            }
	            g_strobe_On = FALSE;
	        }
			break;
//[Begin]zhfan FL sys pr458716
        case FLASHLIGHT_TORCH_ENG:
            printk("FLASHLIGHT_TORCH_ENG: %d\n", arg);
            TORCH_eng(arg);
        break;

        case FLASHLIGHT_FLASHLIGHT_ENG:
            printk("FLASHLIGHT_FLASHLIGHT_ENG: %d\n", arg);
            FL_eng(arg);
        break;
//[End]
		
		default :
    		PK_DBG(" No such command \n");
    		i4RetValue = -EPERM;
    		break;
    }
    return i4RetValue;

strobe_ioctl_error:
    printk("Error or ON state!\n");
    return -EPERM;
}




static int constant_flashlight_open(void *pArg)
{
    int i4RetValue = 0;
    PK_DBG("constant_flashlight_open line=%d\n", __LINE__);

	if (0 == strobe_Res)
	{
	    FL_Init();
		timerInit();
	}
	PK_DBG("constant_flashlight_open line=%d\n", __LINE__);
	spin_lock_irq(&g_strobeSMPLock);


    if(strobe_Res)
    {
        PK_ERR(" busy!\n");
        i4RetValue = -EBUSY;
    }
    else
    {
        strobe_Res += 1;
    }


    spin_unlock_irq(&g_strobeSMPLock);
    PK_DBG("constant_flashlight_open line=%d\n", __LINE__);

    return i4RetValue;

}


static int constant_flashlight_release(void *pArg)
{
    PK_DBG(" constant_flashlight_release\n");

    if (strobe_Res)
    {
        spin_lock_irq(&g_strobeSMPLock);

        strobe_Res = 0;
        strobe_Timeus = 0;

        /* LED On Status */
        g_strobe_On = FALSE;

        spin_unlock_irq(&g_strobeSMPLock);

    	FL_Uninit();
    }

    PK_DBG(" Done\n");

    return 0;

}


FLASHLIGHT_FUNCTION_STRUCT	constantFlashlightFunc=
{
	constant_flashlight_open,
	constant_flashlight_release,
	constant_flashlight_ioctl
};


MUINT32 constantFlashlightInit(PFLASHLIGHT_FUNCTION_STRUCT *pfFunc)
{
    if (pfFunc != NULL)
    {
        *pfFunc = &constantFlashlightFunc;
    }
    return 0;
}



/* LED flash control for high current capture mode*/
ssize_t strobe_VDIrq(void)
{

    return 0;
}

EXPORT_SYMBOL(strobe_VDIrq);

