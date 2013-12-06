
#ifndef __CUSTOM_EMI__
#define __CUSTOM_EMI__

struct EMI_SETTINGS{
  unsigned short  NAND_ID;
  unsigned int  EMI_CONI_VAL;
  unsigned int  EMI_DRVA_VAL; 
  unsigned int  EMI_DRVB_VAL;      
  unsigned int  EMI_CONJ_VAL;      
  unsigned int  EMI_CONK_VAL;      
  unsigned int  EMI_CONL_VAL;      
  unsigned int  EMI_IOCL_VAL;      
  unsigned int  EMI_GENA_VAL;      
  unsigned int  EMI_GEND_VAL;      
  unsigned int  EMI_DRCT_VAL;      
  unsigned int  EMI_PPCT_VAL;      
  unsigned int  EMI_SLCT_VAL;      
  unsigned int  EMI_ABCT_VAL;      
  unsigned int  EMI_DUTB_VAL;
};


typedef struct 
{
    unsigned long EMI_CONN_regval;
        
    unsigned long EMI_DQSA_regval;
    unsigned long EMI_DQSB_regval;
    unsigned long EMI_DQSC_regval;
    unsigned long EMI_DQSD_regval;
    unsigned long EMI_DQSE_regval;
    
    unsigned long EMI_IDLA_regval;
    unsigned long EMI_IDLB_regval;
    unsigned long EMI_IDLC_regval;
    unsigned long EMI_IDLD_regval;
    unsigned long EMI_IDLE_regval;
    unsigned long EMI_IDLF_regval;
    unsigned long EMI_IDLG_regval;
    unsigned long EMI_IDLH_regval;
    unsigned long EMI_IDLI_regval;
    
    unsigned long EMI_CALA_regval;
    unsigned long EMI_CALB_regval;
    unsigned long EMI_CALE_regval;
    unsigned long EMI_CALF_regval;
    unsigned long EMI_CALI_regval;
    unsigned long EMI_CALJ_regval;
    unsigned long EMI_CALP_regval;
    
} EMI_DATA_TRAIN_REG_t;

#endif /* __CUSTOM_EMI__ */

