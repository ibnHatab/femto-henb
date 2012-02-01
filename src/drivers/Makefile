

ifneq ($(KERNELRELEASE),)

EXTRA_CFLAGS += -I$(src)
EXTRA_CFLAGS += 

obj-m += sched_csap.o

#sched_csap-y := sched_csap.o

else

all: default

default:
	echo "Making $@ in `pwd`"
	ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) \
	$(MAKE) V=99 -C $(KERNEL_ROOT) M=`pwd` modules

clean:
	echo "Making $@ in `pwd`"
	ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) \
	$(MAKE) V=99 -C $(KERNEL_ROOT) M=`pwd` clean

endif