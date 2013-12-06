

#include "camera_custom_types.h"
#include "tsf_tuning_custom.h"
#include "TSF_data.h"
#include "TSF_para.h"

MVOID *
getTSFTrainingData(void)
{
    return tsf_data;
}

MVOID *
getTSFTuningData(void)
{

    return TSF_para;
}

MBOOL
isEnableTSF(void)
{
    return MFALSE;
}

MUINT32
getTSFD65Idx(void)
{
#define D65_IDX     (2)
    return D65_IDX;
}

