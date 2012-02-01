# =========================================================================
# KINUX_KERNEL
# -------------------------------------------------------------------------
# The linux kernel build directory is the build kernel tree root against which
# kernel modules will be compiled.  In previous 2.4 systems this and the
# kernel source tree are the same.  In 2.6 based systems this build directory
# and the source directory can be different.
# -------------------------------------------------------------------------
AC_DEFUN([_LINUX_KERNEL], [dnl
    AC_CACHE_CHECK([for kernel build directory], [linux_cv_k_build], [dnl
	AC_MSG_RESULT([searching...])
	AC_ARG_WITH([k-build],
	    AS_HELP_STRING([--with-kernel-build=DIR],
		[specify the base kernel build directory in which configured
		kernel source resides.]),
	    [with_k_build="$withval"],
	    [with_k_build=])
	if test :"${with_k_build:-no}" != :no
	then
	    linux_cv_k_build="$with_k_build"
	fi
	if test :"${linux_cv_k_build:-no}" = :no -o ! -d "$linux_cv_k_build"
	then
	    if test :"${linux_cv_k_build:-no}" = :no
	    then
		AC_MSG_ERROR([
***
*** This package does not support headless kernel build.  Install the
*** appropriate built kernel-source or kernel-headers package for the
*** target kernel "${kversion}" and then configure again.
***
*** The following directories do no exist in the build environment:
***	"$with_k_build"
***	"$k_build_search_path"
***
*** ])
	    fi
	fi
	AC_MSG_CHECKING([for kernel build directory]) ])
    kbuilddir="$linux_cv_k_build"
    AC_SUBST([kbuilddir])dnl
])# LINUX_KERNEL
# =========================================================================

