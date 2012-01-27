SUBDIRS = 

##
## Drivers
##
libLte_drivers_a_SOURCES	=

if CONFIG_MODEM_LTE_SCHED_MODULE
libLte_drivers_a_SOURCES	+= lte_sched.c lte_sched.h
endif

libLte_drivers_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG) $(AUTOCONFIG_H)
libLte_drivers_a_CPPFLAGS	= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) -DKBUILD_BASENAME=`echo $@ | sed -e 's|lib.*_a-|${kpre}|;s|\.o||;s|-|_|g'` $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_EXPSYMTAB)
libLte_drivers_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libLte_drivers_a_OBJECTS): $(libLte_drivers_a_DEPENDENCIES)

KERNEL_OBJECTS			+= $(libLte_drivers_a_OBJECTS)
KERNEL_MODULES			+= $(libLte_drivers_a_OBJECTS)
