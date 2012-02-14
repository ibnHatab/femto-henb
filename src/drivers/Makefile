
target_alias = arm-linux
target_cpu = arm
prefix = /usr/local

KERNEL_ROOT=/udir/vlad/repos/net/buildroot-2011.11/output/build/linux-3.1
TARGETDIR=$(prefix)/firmware

ifneq ($(KERNELRELEASE),)
ifdef V
    KBUILD_VERBOSE = $(V)
endif

EXTRA_CFLAGS += -I$(src)
EXTRA_CFLAGS += 

 obj-m += sched_csap.o

#sched_csap-y := sched_csap.o

else

all: drivers

drivers:
	echo "Making $@ in `pwd`"
	ARCH=$(target_cpu) CROSS_COMPILE="$(target_alias)-" \
	$(MAKE) V=1 -C $(KERNEL_ROOT) M=`pwd` modules

clean:
	echo "Making $@ in `pwd`"
	ARCH=$(target_cpu) CROSS_COMPILE="$(target_alias)-" \
	$(MAKE) V=0 -C $(KERNEL_ROOT) M=`pwd` clean

install:
	echo "Making $@ in `pwd`"
	ARCH=$(target_cpu) CROSS_COMPILE="$(target_alias)-" \
	$(MAKE) V=0 -C $(KERNEL_ROOT) M=`pwd` INSTALL_MOD_PATH=$(TARGETDIR) modules_install 


################################################################################
# Run QEMU testing throughout screen 
################################################################################

MTEST=sched_csap.ko
qemu-test: ${MTEST}
	screen -X -p qemu stuff "mount -o remount,rw /"
	scp ${MTEST} root@192.168.0.2:/root
	screen -X -p qemu stuff "rmmod ${MTEST}"
	screen -X -p qemu stuff "insmod /root/${MTEST}"

endif
