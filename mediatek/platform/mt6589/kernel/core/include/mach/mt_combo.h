
#ifndef __ARCH_ARM_MACH_MT6573_INCLUDE_MACHINE_MT_COMBO_H
#define __ARCH_ARM_MACH_MT6573_INCLUDE_MACHINE_MT_COMBO_H


#include <linux/types.h>


typedef enum {
    COMBO_AUDIO_STATE_0 = 0, /* 0000: BT_PCM_OFF & FM analog (line in/out) */
    COMBO_AUDIO_STATE_1 = 1, /* 0001: BT_PCM_ON & FM analog (in/out) */
    COMBO_AUDIO_STATE_2 = 2, /* 0010: BT_PCM_OFF & FM digital (I2S) */
    COMBO_AUDIO_STATE_3 = 3, /* 0011: BT_PCM_ON & FM digital (I2S) (invalid in 73evb & 1.2 phone configuration) */
    COMBO_AUDIO_STATE_MAX = 4,
} COMBO_AUDIO_STATE;

typedef enum {
    COMBO_FUNC_TYPE_BT = 0,
    COMBO_FUNC_TYPE_FM = 1,
    COMBO_FUNC_TYPE_GPS = 2,
    COMBO_FUNC_TYPE_WIFI = 3,
    COMBO_FUNC_TYPE_WMT = 4,
    COMBO_FUNC_TYPE_STP = 5,
    COMBO_FUNC_TYPE_NUM = 6
} COMBO_FUNC_TYPE;

typedef enum {
    COMBO_IF_UART = 0,
    COMBO_IF_MSDC = 1,
    COMBO_IF_MAX,
} COMBO_IF;




extern int mt_combo_audio_ctrl_ex(COMBO_AUDIO_STATE state, u32 clt_ctrl);
static inline int mt_combo_audio_ctrl(COMBO_AUDIO_STATE state) {
    return mt_combo_audio_ctrl_ex(state, 1);
}
extern int mt_combo_plt_enter_deep_idle(COMBO_IF src);
extern int mt_combo_plt_exit_deep_idle(COMBO_IF src);

extern void mtk_wcn_cmb_stub_func_ctrl (unsigned int type, unsigned int on);
extern int board_sdio_ctrl (unsigned int sdio_port_num, unsigned int on);


#endif

