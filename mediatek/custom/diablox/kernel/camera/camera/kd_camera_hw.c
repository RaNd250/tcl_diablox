//[begin]456653 zhfan 2013-6-10 for imx135 and imx132 power sequence
#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/atomic.h>
#include <linux/xlog.h>
#include <linux/kernel.h>//for printk


#include "kd_camera_hw.h"

#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_camera_feature.h"

#define PFX "[kd_camera_hw]"
#define PK_DBG_NONE(fmt, arg...)    do {} while (0)
#define PK_DBG_FUNC(fmt, arg...)    xlog_printk(ANDROID_LOG_INFO, PFX , fmt, ##arg)

#define DEBUG_CAMERA_HW_K
#ifdef DEBUG_CAMERA_HW_K
#define PK_DBG PK_DBG_FUNC
#define PK_ERR(fmt, arg...)         xlog_printk(ANDROID_LOG_ERR, PFX , fmt, ##arg)
#define PK_XLOG_INFO(fmt, args...) \
                do {    \
                   xlog_printk(ANDROID_LOG_INFO, PFX , fmt, ##arg); \
                } while(0)
#else
#define PK_DBG(a,...)
#define PK_ERR(a,...)
#define PK_XLOG_INFO(fmt, args...)
#endif

#define MAIN_RST         GPIO_CAMERA_CMRST_PIN
#define MAIN_PDN         GPIO_CAMERA_CMPDN_PIN
#define MAIN_AF_EN_PIN   GPIO133
#define SUB_RST          GPIO_CAMERA_CMRST1_PIN
#define SUB_PDN          GPIO_CAMERA_CMPDN1_PIN
#define MODE_GPIO        GPIO_MODE_00
#define MODE_CMMCLK      GPIO_MODE_01
#define IMX135_DVDD_PIN  GPIO209
#define MCLK1_PIN        GPIO210


extern void ISP_MCLK1_EN(BOOL En);
static bool JRDPowerSwitchDVDDVOL1050(char *currSensorName, bool on); //for imx135 1.05V DVDD special requirement
static void setCameraPin(u16 pinNum, u8 mode, u8 dir, u8 level);
static void initCameraPin(char *currSensorName);


int kdCISModulePowerOn(CAMERA_DUAL_CAMERA_SENSOR_ENUM SensorIdx, char *currSensorName, BOOL On, char* mode_name)
{
    //power ON
    if (On) 
    {
#if 0  //TODO: depends on HW layout. Should be notified by SA.

        PK_DBG("Set CAMERA_POWER_PULL_PIN for power \n");
        if (mt_set_gpio_pull_enable(GPIO_CAMERA_LDO_EN_PIN, GPIO_PULL_DISABLE)) {PK_DBG("[[CAMERA SENSOR] Set CAMERA_POWER_PULL_PIN DISABLE ! \n"); }
        if(mt_set_gpio_mode(GPIO_CAMERA_LDO_EN_PIN, GPIO_CAMERA_LDO_EN_PIN_M_GPIO)){PK_DBG("[[CAMERA SENSOR] set CAMERA_POWER_PULL_PIN mode failed!! \n");}
        if(mt_set_gpio_dir(GPIO_CAMERA_LDO_EN_PIN,GPIO_DIR_OUT)){PK_DBG("[[CAMERA SENSOR] set CAMERA_POWER_PULL_PIN dir failed!! \n");}
        if(mt_set_gpio_out(GPIO_CAMERA_LDO_EN_PIN,GPIO_OUT_ONE)){PK_DBG("[[CAMERA SENSOR] set CAMERA_POWER_PULL_PIN failed!! \n");}
#endif

        PK_DBG("kdCISModulePowerOn -on:currSensorName=%s\n",currSensorName);
        
        initCameraPin(currSensorName);
        
        if ((DUAL_CAMERA_MAIN_SENSOR == SensorIdx) && currSensorName && (0 == strcmp(SENSOR_DRVNAME_IMX135_MIPI_RAW,currSensorName)))
        {
            PK_DBG("kdCISModulePowerOn get in---  SENSOR_DRVNAME_IMX135_MIPI_RAW \n");
            PK_DBG("[ON_general 2.8V]sensorIdx:%d \n",SensorIdx);

            setCameraPin(MCLK1_PIN, MODE_GPIO, GPIO_DIR_OUT, GPIO_OUT_ZERO);

            ISP_MCLK1_EN(0);

            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A, VOL_2800,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                goto _kdCISModulePowerOn_exit_;
            }

            if(TRUE != JRDPowerSwitchDVDDVOL1050(currSensorName, On))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable DOVDD power\n");
                goto _kdCISModulePowerOn_exit_;
            }

            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D2, VOL_1800,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                goto _kdCISModulePowerOn_exit_;
            }

            mdelay(10);
            
            setCameraPin(MAIN_RST, MODE_GPIO, GPIO_DIR_OUT, GPIO_OUT_ONE);
            setCameraPin(MAIN_PDN, MODE_GPIO, GPIO_DIR_OUT, GPIO_OUT_ONE);

            setCameraPin(MCLK1_PIN, MODE_CMMCLK, GPIO_DIR_OUT, GPIO_OUT_ZERO);

            ISP_MCLK1_EN(1);

            //modified by zhfan, pr450349, porting diablox main camera imx135 VCM driver dw9714a 
            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A2, VOL_2800,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                goto _kdCISModulePowerOn_exit_;
            }

            mdelay(13);
            
            setCameraPin(MAIN_AF_EN_PIN, MODE_GPIO, GPIO_DIR_OUT, GPIO_OUT_ONE); 
        }
        else if ((DUAL_CAMERA_SUB_SENSOR == SensorIdx) && currSensorName && (0 == strcmp(SENSOR_DRVNAME_IMX132_MIPI_RAW,currSensorName)))
        {
            PK_DBG("kdCISModulePowerOn get in---  SENSOR_DRVNAME_IMX132_MIPI_RAW \n");
            PK_DBG("[ON_general 2.8V]sensorIdx:%d \n",SensorIdx);

            setCameraPin(MCLK1_PIN, MODE_GPIO, GPIO_DIR_OUT, GPIO_OUT_ZERO);

            ISP_MCLK1_EN(0);

            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D2, VOL_1800,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                goto _kdCISModulePowerOn_exit_;
            }

            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A, VOL_2800,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                goto _kdCISModulePowerOn_exit_;
            }

            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D_SUB, VOL_1200,mode_name))
            {
                 PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                 goto _kdCISModulePowerOn_exit_;
            }

            mdelay(10);
            
            setCameraPin(SUB_RST, MODE_GPIO, GPIO_DIR_OUT, GPIO_OUT_ONE);
            setCameraPin(SUB_PDN, MODE_GPIO, GPIO_DIR_OUT, GPIO_OUT_ONE);

            setCameraPin(MCLK1_PIN, MODE_CMMCLK, GPIO_DIR_OUT, GPIO_OUT_ZERO);

            ISP_MCLK1_EN(1);

            mdelay(2);
        }
        else
        {
            PK_DBG("kdCISModulePowerOn get in---  other \n");
            
            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D2, VOL_1800,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                goto _kdCISModulePowerOn_exit_;
            }
        }
    }
    else 
    {//power OFF

#if 0 //TODO: depends on HW layout. Should be notified by SA.
        PK_DBG("Set GPIO 94 for power OFF\n");
        if (mt_set_gpio_pull_enable(GPIO_CAMERA_LDO_EN_PIN, GPIO_PULL_DISABLE)) {PK_DBG("[CAMERA SENSOR] Set GPIO94 PULL DISABLE ! \n"); }
        if(mt_set_gpio_mode(GPIO_CAMERA_LDO_EN_PIN, GPIO_CAMERA_LDO_EN_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
        if(mt_set_gpio_dir(GPIO_CAMERA_LDO_EN_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
        if(mt_set_gpio_out(GPIO_CAMERA_LDO_EN_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
#endif
        PK_DBG("kdCISModulePowerOn -off:currSensorName=%s\n",currSensorName);
        
        initCameraPin(currSensorName);

        if ((DUAL_CAMERA_MAIN_SENSOR == SensorIdx) && currSensorName && (0 == strcmp(SENSOR_DRVNAME_IMX135_MIPI_RAW,currSensorName)))
        {
            PK_DBG("kdCISModulePower--off get in---SENSOR_DRVNAME_IMX135_MIPI_RAW \n");

            mdelay(2);

            setCameraPin(MCLK1_PIN, MODE_GPIO, GPIO_DIR_OUT, GPIO_OUT_ZERO);

            ISP_MCLK1_EN(0);

            mdelay(2);

            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A2,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to OFF analog power\n");
                //return -EIO;
                goto _kdCISModulePowerOn_exit_;
            }
                        
            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D2, mode_name)) {
                PK_DBG("[CAMERA SENSOR] Fail to OFF digital power\n");
                //return -EIO;
                goto _kdCISModulePowerOn_exit_;
            }

            if(TRUE != JRDPowerSwitchDVDDVOL1050(currSensorName, On))
            {
                PK_DBG("[CAMERA SENSOR] Fail to OFF DOVDD power\n");
                goto _kdCISModulePowerOn_exit_;
            }
            
            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to OFF analog power\n");
                //return -EIO;
                goto _kdCISModulePowerOn_exit_;
            }
        }
        else if((DUAL_CAMERA_SUB_SENSOR == SensorIdx) && currSensorName && (0 == strcmp(SENSOR_DRVNAME_IMX132_MIPI_RAW,currSensorName)))
        {
            PK_DBG("kdCISModulePower--off get in---SENSOR_DRVNAME_IMX132_MIPI_RAW \n");

            mdelay(2);

            setCameraPin(MCLK1_PIN, MODE_GPIO, GPIO_DIR_OUT, GPIO_OUT_ZERO);

            ISP_MCLK1_EN(0);

            mdelay(2);

            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D_SUB, mode_name)) {
                PK_DBG("[CAMERA SENSOR] Fail to OFF digital power\n");
                //return -EIO;
                goto _kdCISModulePowerOn_exit_;
            }

            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A,mode_name)) {
                PK_DBG("[CAMERA SENSOR] Fail to OFF analog power\n");
                //return -EIO;
                goto _kdCISModulePowerOn_exit_;
            }

            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D2,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to OFF analog power\n");
                //return -EIO;
                goto _kdCISModulePowerOn_exit_;
            }
        }
        else
        {
            PK_DBG("kdCISModulePower--off get in---other \n");
            
            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D2,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to OFF analog power\n");
                //return -EIO;
                goto _kdCISModulePowerOn_exit_;
            }
        }
    }
    return 0;

_kdCISModulePowerOn_exit_:
    return -EIO;
}

static bool JRDPowerSwitchDVDDVOL1050(char *currSensorName, bool on)
{
    bool ret = FALSE;
    if(currSensorName && (0 == strcmp(SENSOR_DRVNAME_IMX135_MIPI_RAW,currSensorName)))
    {
        if(mt_set_gpio_mode(IMX135_DVDD_PIN, MODE_GPIO))
        {
            PK_DBG("[IMX135_DVDD_PIN] set gpio mode failed!! \n");
        }
        
        if(on)
        {
            if(mt_set_gpio_out(IMX135_DVDD_PIN, GPIO_OUT_ONE))
            {
                PK_DBG("[IMX135_DVDD_PIN] set gpio dir failed!! \n");
            }
        }
        else
        {
            if(mt_set_gpio_out(IMX135_DVDD_PIN, GPIO_OUT_ZERO))
            {
                PK_DBG("[IMX135_DVDD_PIN] set gpio dir failed!! \n");
            }
        }

        if(mt_set_gpio_dir(IMX135_DVDD_PIN, GPIO_DIR_OUT))
        {
            PK_DBG("[IMX135_DVDD_PIN] set gpio failed!! \n");
        }
        else
        {
            ret = TRUE;
        }
    }
    else
    {
        PK_DBG("[JRDPowerSwitchDVDDVOL1050] It's for imx135 only, not used by other sensor!! \n");
    }
    return ret;
}

static void setCameraPin(u16 pinNum, u8 mode, u8 dir, u8 level)
{
    if(mt_set_gpio_mode(pinNum, mode)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
    if(mt_set_gpio_out(pinNum, level)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
    if(mt_set_gpio_dir(pinNum, dir)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
}

static void initCameraPin(char *currSensorName)
{
    if(currSensorName && (0 == strcmp(SENSOR_DRVNAME_IMX135_MIPI_RAW,currSensorName)))
    {
        setCameraPin(MAIN_RST, MODE_GPIO, GPIO_DIR_OUT, GPIO_OUT_ZERO);
        setCameraPin(MAIN_PDN, MODE_GPIO, GPIO_DIR_OUT, GPIO_OUT_ZERO);
        mdelay(1);
        setCameraPin(MAIN_AF_EN_PIN, MODE_GPIO, GPIO_DIR_OUT, GPIO_OUT_ZERO);
    }
    else if(currSensorName && (0 == strcmp(SENSOR_DRVNAME_IMX132_MIPI_RAW, currSensorName)))
    {
        setCameraPin(SUB_RST, MODE_GPIO, GPIO_DIR_OUT, GPIO_OUT_ZERO);
        setCameraPin(SUB_PDN, MODE_GPIO, GPIO_DIR_OUT, GPIO_OUT_ZERO);
    }
    else
    {
        PK_DBG("[CAMERA SENSOR]Do not support this sensor!!\n");
    }
}

EXPORT_SYMBOL(kdCISModulePowerOn);


//!--
//
//[end]456653 zhfan 2013-6-10