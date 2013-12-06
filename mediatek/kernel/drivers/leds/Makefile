ifeq ($(PROJECT),jrdsz89_we_jb2)
EXTRA_CFLAGS += -DRAV4_LED=1
endif
obj-$(CONFIG_MTK_LEDS) := leds.o
ifeq ($(PROJECT),diabloalpha)
obj-y += sn3199.o
endif
ifeq ($(PROJECT),scribepro)
obj-y += sn3731.o
endif
