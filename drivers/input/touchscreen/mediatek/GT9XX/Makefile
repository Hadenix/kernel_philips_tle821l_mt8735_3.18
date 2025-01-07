ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/GT9XX/
#ifneq (,$(findstring config,$(CONFIG_GT910_CONFIG)))
#ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/GT9XX/include/$(CONFIG_GT910_CONFIG)/
#else
#ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/GT9XX/include/config_default/
ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/GT9XX/include/config_wxga/
#endif
ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(MTK_PLATFORM)/include/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(MTK_PLATFORM)/include/mach/

obj-y	+=  gt9xx_driver.o
obj-y	+=  goodix_tool.o
obj-y	+=  gt9xx_update.o

