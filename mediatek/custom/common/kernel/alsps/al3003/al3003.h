
#ifndef __AL3003_H__
#define __AL3003_H__

#include <linux/ioctl.h>

#define AL3003_I2C_SLAVE_ADDR 0x38 //0x3A<-->SEL to VDD  0x38<-->SEL to GND

/*REG address*/
#define APS_CONFIGUATION 0x00
#define APS_TIMING_CTL   0x01
#define APS_ALS_CTL      0x02
#define APS_INT_STATUS   0x03
#define APS_PS_CTL       0x04
#define APS_BOTH_DATA    0x05
#define APS_ALS_WINDOW   0x08


/*ALS control*/
#define ALS_LEVEL_64  0xA0
#define ALS_LEVEL_33  0x80
#define ALS_LEVEL_17  0x60
#define ALS_LEVEL_9   0x40
#define ALS_LEVEL_5   0x20
#define ALS_LEVEL_3   0x00


/*PS control*/




#endif

