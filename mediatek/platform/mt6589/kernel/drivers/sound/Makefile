obj-$(CONFIG_MTK_SOUND) += AudDrv_Kernel.o AudDrv_Clk.o AudDrv_Afe.o AudDrv_Ana.o

ifeq ($(JRD_IPHONE_HDST_COMPAT), yes)
    ccflags-y += -DJRD_IPHONE_HDST_COMPATIBILITY
endif
