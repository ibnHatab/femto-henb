
target_alias = arm-linux
target_cpu = arm
prefix = /usr/local

KERNEL_ROOT=/udir/vlad/repos/net/buildroot-2011.11/output/build/linux-3.1
TARGETDIR=$(prefix)/firmware

EMBUNIT_DIR := /udir/vlad/repos/net/embunit
EMBUNIT_SRC := embunit/embUnit

ifneq ($(KERNELRELEASE),)

ifdef V
    KBUILD_VERBOSE = $(V)
endif


EXTRA_CFLAGS += -I$(src) -I$(EMBUNIT_DIR)
EXTRA_CFLAGS += -DNO_STDIO_PRINTF

obj-m += test.o

#test runner
test-y := test_runner.o

#counter module
test-y += counter.o
test-y += counterTest.o

#person module
test-y += person.o
test-y += personTest.o

#embunit
test-y += $(EMBUNIT_SRC)/AssertImpl.o
test-y += $(EMBUNIT_SRC)/RepeatedTest.o
test-y += $(EMBUNIT_SRC)/stdImpl.o
test-y += $(EMBUNIT_SRC)/TestCaller.o
test-y += $(EMBUNIT_SRC)/TestCase.o
test-y += $(EMBUNIT_SRC)/TestResult.o
test-y += $(EMBUNIT_SRC)/TestRunner.o
test-y += $(EMBUNIT_SRC)/TestSuite.o

else

all: testing

testing:
	echo "Making $@ in `pwd`"
	@ln -sf $(EMBUNIT_DIR)
	ARCH=$(target_cpu) CROSS_COMPILE="$(target_alias)-" \
	$(MAKE) V=1 -C $(KERNEL_ROOT) M=`pwd` modules

clean:
	echo "Making $@ in `pwd`"
	ARCH=$(target_cpu) CROSS_COMPILE="$(target_alias)-" \
	$(MAKE) V=1 -C $(KERNEL_ROOT) M=`pwd` clean

install:
	echo "Making $@ in `pwd`"
	ARCH=$(target_cpu) CROSS_COMPILE="$(target_alias)-" \
	$(MAKE) V=99 -C $(KERNEL_ROOT) M=`pwd` INSTALL_MOD_PATH=$(TARGETDIR) modules_install 

embunit:
	echo "Making $@ in `pwd`"
	@ln -sf $(EMBUNIT_DIR)
	$(MAKE) -C $(EMBUNIT_DIR)/tools


################################################################################
# Run QEMU testing using buildroot
# FIXME: add --with-buildroot=DIR
################################################################################

export REPOS="/udir/vlad/repos/net/"

qemu-stop:
	killall qemu-system-arm

qemu-debug:
	screen -t qemu-debug -L -d -m './qemu.run -s -S'

# screen -t qemu -L -d -m ./qemu.run
qemu-start:
	screen -X -p qemu stuff "root"
	sleep 1
	screen -X -p qemu stuff "vision"

MTEST=test.ko
qemu-test:
	screen -X -p qemu stuff "mount -o remount,rw /"
	scp ${MTEST} root@192.168.0.2:/root
	screen -X -p qemu stuff "rmmod ${MTEST}"
	screen -X -p qemu stuff "insmod /root/${MTEST}"


endif # ! KERNELRELEASE
