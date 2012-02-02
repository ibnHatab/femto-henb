# =========================================================================
# _LINUX_KERNEL
# -------------------------------------------------------------------------
# The linux kernel build directory is the build kernel tree root against which
# kernel modules will be compiled.
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_KERNEL], [dnl
    AC_CACHE_CHECK([for kernel build directory], [linux_cv_kernel_build], [dnl
	AC_MSG_RESULT([searching...])
	AC_ARG_WITH([kernel-build],
	    AS_HELP_STRING([--with-kernel-build=DIR],
		[specify the base kernel build directory in which configured
		kernel source resides.]),
	    [with_kernel_build="$withval"],
	    [with_kernel_build=])
	if test :"${with_kernel_build:-no}" != :no
	then
	    linux_cv_kernel_build="$with_kernel_build"
	fi
	if test :"${linux_cv_kernel_build:-no}" = :no -o ! -d "$linux_cv_kernel_build"
	then
	    if test :"${linux_cv_kernel_build:-no}" = :no
	    then
		AC_MSG_ERROR([
***
*** This package does not support headless kernel build.  Install the
*** appropriate built kernel-source and kernel-headers package for the
*** target kernel and then configure again.
***
*** The following directories do no exist in the build environment:
***	"$with_kernel_build"
***
*** ])
	    fi
	fi
	AC_MSG_CHECKING([for kernel build directory]) ])
    kbuilddir="$linux_cv_kernel_build"
    AC_SUBST([kbuilddir])dnl
])# LINUX_KERNEL
# =========================================================================

# =========================================================================
# _LINUX_EMBUNIT
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_EMBUNIT], [dnl
    AC_CACHE_CHECK([for embunit build directory], [linux_cv_embunit_build], [dnl
	AC_MSG_RESULT([searching...])
	AC_ARG_WITH([embunit-build],
	    AS_HELP_STRING([--with-embunit-build=DIR],
		[specify the embunit build directory in which embunit source resides.]),
	    [with_embunit_build="$withval"],
	    [with_embunit_build=])
	if test :"${with_embunit_build:-no}" != :no
	then
	    linux_cv_embunit_build="$with_embunit_build"
	fi
	if test :"${linux_cv_embunit_build:-no}" = :no -o ! -d "$linux_cv_embunit_build"
	then
	    if test :"${linux_cv_embunit_build:-no}" = :no
	    then
		AC_MSG_ERROR([
***
*** This package does not support embunit less build.  Install the
*** appropriate built embunit-source.
*** The following directories do no exist in the build environment:
***	"$with_embunit_build"
***
*** ])
	    fi
	fi
	AC_MSG_CHECKING([for embunit build directory]) ])
    embunit_dir="$linux_cv_embunit_build"
    AC_SUBST([embunit_dir])
    AC_SUBST([CONFIG_ENABLE_TESTING])dnl
])# LINUX_EMBUNIT
# =========================================================================
