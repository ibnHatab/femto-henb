# =============================================================================
m4_include([m4/kernel.m4])

# =============================================================================
# AC_MODEM
# -----------------------------------------------------------------------------
AC_DEFUN([AC_MODEM], [dnl
    KERNEL_CPPFLAGS="$CPPFLAGS"
    KERNEL_CFLAGS="$CFLAGS"
    KERNEL_LDFLAGS="$LDFLAGS"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros $(top_builddir)/config.h'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I$(top_builddir)/include -I$(top_srcdir)/include/sys'
    AC_SUBST([PKG_INCLUDES])dnl
    _MODEM_SETUP
    AC_SUBST([KERNEL_CPPFLAGS])dnl
    AC_SUBST([KERNEL_CFLAGS])dnl
    AC_SUBST([KERNEL_LDFLAGS])dnl
    CPPFLAGS=
    CFLAGS=
])# AC_MODEM

# =============================================================================
# _MODEM_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_MODEM_SETUP], [dnl
    _LINUX_KERNEL
    _LINUX_EMBUNIT
    _MODEM_SETUP_MODULES
    _MODEM_SETUP_DRIVERS
])
# =============================================================================

# =============================================================================
# _MODEM_SETUP_MODULES
# -----------------------------------------------------------------------------
AC_DEFUN([_MODEM_SETUP_MODULES], [dnl
    AC_ARG_ENABLE([module-lte-sched],
	AS_HELP_STRING([--enable-module-lte-sched],
	    [enable module lte_sched for linkage with MODEM.
	    @<:@default=module@:>@]),
	    [enable_module_lte_sched="$enableval"],
	    [if test :${modem_big_compile:-yes} = :yes ; then enable_module_lte_sched='yes' ; else enable_module_lte_sched='module' ; fi])
    AM_CONDITIONAL([CONFIG_MODEM_LTE_SCHED],		[test :${modem_cv_module_lte_sched:-module}	= :yes])
# -----------------------------------------------------------------------------

    AC_CACHE_CHECK([for MODEM module lte_sched], [modem_cv_module_lte_sched], [dnl
	modem_cv_module_lte_sched="${enable_module_lte_sched:-module}"
	if test :$modem_cv_module_lte_sched = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    modem_cv_module_lte_sched='yes'
	fi])
# -----------------------------------------------------------------------------
    case ${modem_cv_module_lte_sched:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_MODEM_LTE_SCHED], [1], [When defined,] AC_PACKAGE_TITLE [
	    will include the lte_sched module for linkage with MODEM.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the lte_sched module for linkage with MODEM.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_MODEM_LTE_SCHED_MODULE], [1], [When defined,]
	    AC_PACKAGE_TITLE [will include the lte_sched module as a standalone loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the lte_sched module as a standalone loadable kernel
	    module.])
	    ;;
    esac
# -----------------------------------------------------------------------------
    AM_CONDITIONAL([CONFIG_MODEM_LTE_SCHED],		[test :${modem_cv_module_lte_sched:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_MODEM_LTE_SCHED_MODULE],	[test :${modem_cv_module_lte_sched:-module}	= :module])
])# _MODEM_SETUP_MODULES

# =============================================================================
# _MODEM_SETUP_DRIVERS
# -----------------------------------------------------------------------------
AC_DEFUN([_MODEM_SETUP_DRIVERS], [dnl
    AC_ARG_ENABLE([driver-sched-csap],
	AS_HELP_STRING([--enable-driver-sched-csap],
	    [enable sched_csap driver for linkage with MODEM.
	    @<:@default=module@:>@]),
	    [enable_driver_sched_csap="$enableval"],
	    [if test :${modem_big_compile:-yes} = :yes ; then enable_driver_sched_csap='yes' ; else enable_driver_sched_csap='module' ; fi])
# -----------------------------------------------------------------------------
    AC_CACHE_CHECK([for MODEM driver sched_csap], [modem_cv_driver_sched_csap], [dnl
	modem_cv_driver_sched_csap="${enable_driver_sched_csap:-module}"
	if test :$modem_cv_driver_sched_csap = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    modem_cv_driver_sched_csap='yes'
	fi])

# -----------------------------------------------------------------------------
    case ${modem_cv_driver_sched_csap:-module} in
	(yes)
	    AC_DEFINE_UNQUOTED([CONFIG_MODEM_SCHED_CSAP], [1], [When defined,] AC_PACKAGE_TITLE [
	    will include the sched_csap driver for linkage with MODEM.  When undefined,]
	    AC_PACKAGE_TITLE [will not include the sched_csap driver for linkage with MODEM.])
	    ;;
	(module)
	    AC_DEFINE_UNQUOTED([CONFIG_MODEM_SCHED_CSAP_MODULE], [1], [When defined,]
	    AC_PACKAGE_TITLE [will include the sched_csap driver as a standalone loadable kernel module.  When
	    undefined,] AC_PACKAGE_TITLE [will not include the sched_csap driver as a standalone loadable kernel
	    module.])
	    ;;
    esac
# -----------------------------------------------------------------------------

    AM_CONDITIONAL([CONFIG_MODEM_SCHED_CSAP],		[test :${modem_cv_driver_sched_csap:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_MODEM_SCHED_CSAP_MODULE],	[test :${modem_cv_driver_sched_csap:-module}	= :module])
])# _MODEM_SETUP_DRIVERS
# =============================================================================
