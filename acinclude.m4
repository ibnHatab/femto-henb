# =============================================================================

m4_include([m4/kernel.m4])
m4_include([m4/devfs.m4])


# =============================================================================
# AC_MODEM
# -----------------------------------------------------------------------------
AC_DEFUN([AC_MODEM], [dnl
    USER_CPPFLAGS="$CPPFLAGS"
    USER_CFLAGS="$CFLAGS"
    USER_LDFLAGS="$LDFLAGS"
    _MODEM_SETUP
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros $(top_builddir)/config.h'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I$(top_builddir)/include -I$(top_srcdir)/include'
    AC_SUBST([USER_CPPFLAGS])dnl
    AC_SUBST([USER_CFLAGS])dnl
    AC_SUBST([USER_LDFLAGS])dnl
    AC_SUBST([PKG_INCLUDES])dnl
    CPPFLAGS=
    CFLAGS=
])# AC_MODEM

# =============================================================================
# _MODEM_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_MODEM_SETUP], [dnl
    _LINUX_KERNEL
    _LINUX_DEVFS
    _MODEM_SETUP_MODULE
    _MODEM_SETUP_MODULES
    _MODEM_SETUP_DRIVERS
])

# =============================================================================
# _MODEM_SETUP_MODULE
# -----------------------------------------------------------------------------
AC_DEFUN([_MODEM_SETUP_MODULE], [dnl
    if test :"${linux_cv_k_linkage:-loadable}" = :loadable ; then
	AC_DEFINE_UNQUOTED([CONFIG_MODEM_MODULE], [1], [When defined, MODEM
	    is being compiled as a loadable kernel module.])
    else
	AC_DEFINE_UNQUOTED([CONFIG_MODEM], [1], [When defined, MODEM is
	    being compiled as a kernel linkable object.])
    fi
    AM_CONDITIONAL([CONFIG_MODEM_MODULE],	[test :${linux_cv_k_linkage:-loadable} = :loadable])
    AM_CONDITIONAL([CONFIG_MODEM],		[test :${linux_cv_k_linkage:-loadable} = :linkable])
])# _MODEM_SETUP_MODULE

# =============================================================================

# =============================================================================
# _MODEM_SETUP_MODULES
# -----------------------------------------------------------------------------
AC_DEFUN([_MODEM_SETUP_MODULES], [dnl
    AC_ARG_ENABLE([module-lte_sched],
	AS_HELP_STRING([--enable-module-lte_sched],
	    [enable module lte_sched for linkage with MODEM.
	    @<:@default=module@:>@]),
	    [enable_module_lte_sched="$enableval"],
	    [if test :${lfs_big_compile:-yes} = :yes ; then enable_module_lte_sched='yes' ; else enable_module_lte_sched='module' ; fi])
    AM_CONDITIONAL([CONFIG_MODEM_LTE_SCHED],		[test :${lfs_module_lte_sched:-module}	= :yes])
# -----------------------------------------------------------------------------

    AC_CACHE_CHECK([for MODEM module lte_sched], [lfs_module_lte_sched], [dnl
	lfs_module_lte_sched="${enable_module_lte_sched:-module}"
	if test :$lfs_module_lte_sched = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    lfs_module_lte_sched='yes'
	fi])
# -----------------------------------------------------------------------------
    case ${lfs_module_lte_sched:-module} in
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
    AM_CONDITIONAL([CONFIG_MODEM_LTE_SCHED],		[test :${lfs_module_lte_sched:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_MODEM_LTE_SCHED_MODULE],		[test :${lfs_module_lte_sched:-module}	= :module])
])# _MODEM_SETUP_MODULES

# =============================================================================
# _MODEM_SETUP_DRIVERS
# -----------------------------------------------------------------------------
AC_DEFUN([_MODEM_SETUP_DRIVERS], [dnl
    AC_ARG_ENABLE([driver-sched_csap],
	AS_HELP_STRING([--enable-driver-sched_csap],
	    [enable sched_csap driver for linkage with MODEM.
	    @<:@default=module@:>@]),
	    [enable_driver_sched_csap="$enableval"],
	    [if test :${lfs_big_compile:-yes} = :yes ; then enable_driver_sched_csap='yes' ; else enable_driver_sched_csap='module' ; fi])
# -----------------------------------------------------------------------------
    AC_CACHE_CHECK([for MODEM driver sched_csap], [lfs_driver_sched_csap], [dnl
	lfs_driver_sched_csap="${enable_driver_sched_csap:-module}"
	if test :$lfs_driver_sched_csap = :module -a :${linux_cv_k_linkage:-loadable} = :linkable ; then
	    lfs_driver_sched_csap='yes'
	fi])

# -----------------------------------------------------------------------------
    case ${lfs_driver_sched_csap:-module} in
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

    AM_CONDITIONAL([CONFIG_MODEM_SCHED_CSAP],		[test :${lfs_driver_sched_csap:-module}	= :yes])
    AM_CONDITIONAL([CONFIG_MODEM_SCHED_CSAP_MODULE],	[test :${lfs_driver_sched_csap:-module}	= :module])
])# _MODEM_SETUP_DRIVERS
# =============================================================================
