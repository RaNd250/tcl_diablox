
#ifndef __SENSOR_H
#define __SENSOR_H

#include "image_sensor.h"//get IMAGE_SENSOR_DRVNAME
#define IMAGE_SENSOR_DRVNAME SENSOR_DRVNAME_OV7675_YUV

    //------------------------Engineer mode---------------------------------
#define FACTORY_START_ADDR 	0
#define ENGINEER_START_ADDR	10

    typedef enum group_enum {
       PRE_GAIN=0,
	   CMMCLK_CURRENT,
	   FRAME_RATE_LIMITATION,
	   REGISTER_EDITOR,
	   GROUP_TOTAL_NUMS
    } FACTORY_REGISTER_INDEX;

    typedef enum register_index {
        SENSOR_BASEGAIN=FACTORY_START_ADDR,
	      PRE_GAIN_R_INDEX,
	      PRE_GAIN_Gr_INDEX,
	      PRE_GAIN_Gb_INDEX,
	      PRE_GAIN_B_INDEX,
	      FACTORY_END_ADDR
    } CCT_REGISTER_INDEX;
    
 typedef enum engineer_index
{   
	CMMCLK_CURRENT_INDEX=ENGINEER_START_ADDR,
	ENGINEER_END
} FACTORY_ENGINEER_INDEX; 

    //------------------------Engineer mode---------------------------------
    typedef struct {
        SENSOR_REG_STRUCT Reg[ENGINEER_END];
        SENSOR_REG_STRUCT CCT[FACTORY_END_ADDR];
    } SENSOR_DATA_STRUCT,*PSENSOR_DATA_STRUCT;




	
 #define CAM_PREVIEW_30FPS
 #define SYSTEM_CLK                           (52*1000*1000)
		/* PIXEL CLOCK USED BY BANDING FILTER CACULATION*/
#if defined(CAM_PREVIEW_15FPS)
  #define PIXEL_CLK							    (SYSTEM_CLK/8)		// 52/8 MHz
#elif defined(CAM_PREVIEW_22FPS)
   #define PIXEL_CLK							    (SYSTEM_CLK/6)		// 52/6 MHz
#elif defined(CAM_PREVIEW_30FPS)
   #define PIXEL_CLK 						      	(SYSTEM_CLK/4)		// 52/4 MHz
#endif

   #define OV7675_VIDEO_NORMALMODE_FRAME_RATE							30		// Limitation for MPEG4 Encode Only
   #define OV7675_VIDEO_NIGHTMODE_FRAME_RATE							15
	/* SENSOR PIXEL/LINE NUMBERS IN ONE PERIOD */
	#define VGA_PERIOD_PIXEL_NUMS					784
	#define VGA_PERIOD_LINE_NUMS					510

	/* SENSOR EXPOSURE LINE LIMITATION */
	#define VGA_EXPOSURE_LIMITATION					510

	/* SENSOR GLOBAL GAIN AT NIGHT MODE */
	#define OV7675_SENSOR_NIGHT_MODE_GAIN					0x08	// Please refer to OV7670 Implementation Guide
	
	/* SENSOR VGA SIZE */
	#define IMAGE_SENSOR_VGA_WIDTH					(640-8)
	#define IMAGE_SENSOR_VGA_HEIGHT					(480-6)


    #define IMAGE_SENSOR_FULL_WIDTH          (640-8) 
    #define IMAGE_SENSOR_FULL_HEIGHT         (480-6) 

    #define IMAGE_SENSOR_PV_WIDTH   IMAGE_SENSOR_VGA_WIDTH
    #define IMAGE_SENSOR_PV_HEIGHT  IMAGE_SENSOR_VGA_HEIGHT
    
	/* SETUP TIME NEED TO BE INSERTED */
	#define IMAGE_SENSOR_VGA_INSERTED_PIXELS		128
	#define IMAGE_SENSOR_VGA_INSERTED_LINES		    17
	
	#define OV7675_WRITE_ID								0x42
	#define OV7675_READ_ID								0x43
	
//  #define OV7675_SENSOR_ID    (0x7673)  

  typedef struct
  {
	  kal_uint32 (*Open)(struct i2c_client *i2c_clit);
	  kal_uint32 (*Close)(void);
  
	  kal_uint32 (*GetResolution)(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *pSensorResolution);
	  kal_uint32 (*GetInfo)(MSDK_SCENARIO_ID_ENUM ScenarioId,MSDK_SENSOR_INFO_STRUCT *pSensorInfo,MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData);
  
	  kal_uint32 (*Control)(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow,MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData);	  
	  kal_uint32 (* FeatureControl)(MSDK_SENSOR_FEATURE_ENUM ScenarioId, kal_uint8 *para, kal_uint32 *len);
  
  }image_sensor_func_struct;
  
  

  void image_sensor_func_config(void);








struct OV7675_Sensor_Struct
{
	struct i2c_client *i2c_clit;
	MSDK_SENSOR_CONFIG_STRUCT cfg_data;
	SENSOR_DATA_STRUCT eng; /* engineer mode */
	MSDK_SENSOR_ENG_INFO_STRUCT eng_info;

	
	kal_bool sensor_night_mode;
	kal_bool MPEG4_encode_mode;

	kal_uint16 dummy_pixels;
	kal_uint16 dummy_lines;
	kal_uint16 extra_exposure_lines;
	kal_uint16 exposure_lines;

	kal_bool MODE_CAPTURE;
	kal_uint16 iBackupExtraExp;


	
	kal_uint32 fPV_PCLK; //26000000;
	kal_uint16 iPV_Pixels_Per_Line;

	kal_bool  bNight_mode; // to distinguish night mode or auto mode, default: auto mode setting
	kal_bool  bBanding_value; // to distinguish between 50HZ and 60HZ.
	kal_uint8 u8Wb_value;
	kal_uint8 u8Effect_value;
	kal_uint8 u8Ev_value;
};




#endif /* __SENSOR_H */ 
