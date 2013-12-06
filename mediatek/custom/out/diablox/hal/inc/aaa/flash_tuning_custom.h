

#ifndef __FLASH_TUNING_CUSTOM_H__
#define __FLASH_TUNING_CUSTOM_H__



int getDefaultStrobeNVRam(int sensorType, void* data, int* ret_size);

FLASH_PROJECT_PARA& cust_getFlashProjectPara(int AEMode, NVRAM_CAMERA_STROBE_STRUCT* nvrame);
int cust_isNeedAFLamp(int flashMode, int afLampMode, int isBvTriger);
int cust_isNeedDoPrecapAF(int isFocused, int flashMode, int afLampMode, int isBvLowerTriger);


#endif //#ifndef __FLASH_TUNING_CUSTOM_H__

