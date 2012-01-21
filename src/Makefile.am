
SUBDIRS = libfapi

##
## Drivers
##
libLte_drivers_a_SOURCES	=

if CONFIG_MODEM_LTE_SCHED_MODULE
libLte_drivers_a_SOURCES	+= drivers/lte_sched.c drivers/lte_sched.h
endif

libLte_drivers_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
libLte_drivers_a_CPPFLAGS	= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) -DKBUILD_BASENAME=`echo $@ | sed -e 's|lib.*_a-|${kpre}|;s|\.o||;s|-|_|g'` $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_EXPSYMTAB)
libLte_drivers_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libLte_drivers_a_OBJECTS): $(libLte_drivers_a_DEPENDENCIES)

##
## Modules
##
libLte_modules_a_SOURCES	=

if CONFIG_MODEM_LTE_SCHED_MODULE
libLte_modules_a_SOURCES	+= modules/lte_sched.c modules/lte_sched.h
endif


libLte_modules_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
libLte_modules_a_CPPFLAGS	= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) -DKBUILD_BASENAME=`echo $@ | sed -e 's|lib.*_a-|${kpre}|;s|\.o||;s|-|_|g'` $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_EXPSYMTAB)
libLte_modules_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libLte_modules_a_OBJECTS): $(libLte_modules_a_DEPENDENCIES)

##
## Rules for building kernel objects
##
KERNEL_OBJECTS			= $(libLte_drivers_a_OBJECTS) $(libLte_modules_a_OBJECTS)
KERNEL_MODULES			= $(libLte_drivers_a_OBJECTS) $(libLte_modules_a_OBJECTS)
KERNEL_PRELOADS			=
KERNEL_MODCONF			= $(STRCONF_CONFMOD)
KERNEL_SUBDIR			= $(PACKAGE_LCNAME)
KMODCONF_EXT			= $(PACKAGE_LCNAME)
KERNEL_MODMAPS			=

#FIXME:  include ${top_srcdir}/am/kernel.am