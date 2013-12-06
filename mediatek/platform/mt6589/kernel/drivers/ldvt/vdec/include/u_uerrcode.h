

#ifndef _UERRCODE_H_
#define _UERRCODE_H_






#define UERRCODE(modid, errid)         \
  ((INT32)                             \
    ((INT32)(0x80000000) |             \
     (INT32)((modid & 0x7f) << 24) |   \
     (INT32)(errid & 0xffff))          \
  )

#define UOKCODE(modid, okid)           \
  ((INT32)                             \
    ((INT32)(0x00000000) |             \
     (INT32)((modid & 0x7f) << 24) |   \
     (INT32)(okid & 0xffff))           \
  )

#define EXT_UERR(errcode)              \
  ((INT32)                             \
    (INT32)(errcode & 0xff00ffff) |    \
    (INT32)((__LINE__ & 0xff) << 16)   \
  )

#define EXT_UERR_12(errcode)           \
  ((INT32)                             \
    (INT32)(errcode & 0xff000fff) |    \
    (INT32)((__LINE__ & 0xfff) << 12)  \
  )


typedef INT32 MRESULT; // with MRESULT, we can define a function that reture is MRESULT such as MRESULT SetDVDSector();

// Helper macro
#define SUCCEEDED(Status)   ((MRESULT)(Status) >= 0)
#define FAILED(Status)      ((MRESULT)(Status)<0)
#define CHKMR  if (FAILED(mr)) return mr




#define MODULE_COMMON                           0x00L   /* common module */

// Application Layer, add application module here
#define APP_MODULE_BEGIN                        0x01L   /* common module */


#define APP_MODULE_END                          0x1FL   /* common module */


// Media MiddleWare Layer, add media middleware module here
#define MMWL_MODULE_BEGIN                       0x20L   /* common module */
#define MMWL_MODULE_MPC                         0x21L   //< Multimedia Playback Controller
#define MMWL_MODULE_PB                          0x26L   /* playback engine*/
#define MMWL_MODULE_TCM                         0x28L   /* Thumbnail cache manager */

#define MMWL_MODULE_END                         0x3FL   /* common module */


// System MiddleWare Layer, add system middleware module here
#define SMWL_MODULE_BEGIN                       0x40L   /* common module */


#define SMWL_MODULE_END                         0x5FL   /* common module */


// Driver Layer, add driver module here
#define DRL_MODULE_BEGIN                        0x60L   /* common module */
#define DRL_MODULE_ESM                          0x61L  ///< Elementary stream interface manager
#define DRL_MODULE_FBM                          0x62L  ///< Frame buffer manager
#define DRL_MODULE_PSR                          0x63L  ///< Parser
#define DRL_MODULE_DMX                          0x64L  ///< Transport stream demuxer
#define DRL_MODULE_VDEC                         0x65L  ///< video decoder
#define DRL_MODULE_PBBUF                        0x66L   ///< playback buffer
#define DRL_MODULE_GCPU                         0x67L   ///< GCPU
#define DRL_MODULE_CPSA                         0x68L   ///< CPS Agent
#define DRL_MODULE_SPT                          0x69L   ///< Splitter
#define DRL_MODULE_KM                           0x6AL   ///< KM
#define DRL_MODULE_SYNCCTRL                     0x70L   ///< Sync Control
#define DRL_MODULE_PMX                          0x71L   ///< Plane Mixer
#define DRL_MODULE_AUD                          0x72L   ///< Audio Driver
#define DRL_MODULE_AM                           0x73L   ///< AM
#define DRL_MODULE_HDMI                         0x74L   ///< HDMI
#define DRL_MODULE_AVD                          0x75L   ///< AVD
#define DRL_MODULE_TVE                        0x76L   ///< TVE
#define DRL_MODULE_VENC                        0x77L   ///< VENC
#define DRL_MODULE_END                          0x7FL   /* common module */
#define DRL_MODULE_PVR	                         0x80L ///< PVR

// conflict with E_ABORT in fs.h, remark it

// common definition
#ifndef _WINERROR_

#define S_OK                       ((MRESULT)0x00000000)
#define E_FAIL                     UERRCODE(MODULE_COMMON,1)
#define E_NOTIMPL                  UERRCODE(MODULE_COMMON,2)
#define E_INVALIDARG               UERRCODE(MODULE_COMMON,4)

#endif //_WINERROR_

#endif // _UERRCODE_H_

