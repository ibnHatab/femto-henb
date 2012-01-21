dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.
dnl
AC_REVISION([configure.ac,v 0.9.2.43 2005/07/04 20:07:24 brian Exp])
AC_PREREQ(2.59)
AC_INIT([OpenHeNB],[0.1a],[lib.aca55a@gmail.com])

AC_CONFIG_SRCDIR([src/modules/sched_csap.c])
AC_CONFIG_HEADERS([config.h include/sys/config.h])

AC_CONFIG_AUX_DIR([scripts])
AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_TARGET

dnl AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)

dnl
dnl Checks for programs.
dnl
dnl AM_PROG_AS
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
dnl AM_PROG_CC_C_O
AC_PROG_INSTALL
dnl AM_PROG_INSTALL_STRIP
AC_ARG_VAR([LD], [Linker loader command])
dnl AC_PROG_LD
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_YACC
dnl AC_PROG_LIBTOOL
AC_SUBST([LIBTOOL_DEPS])

dnl
dnl Checks for libraries.
dnl
lfs_save_LIBS="$LIBS"
AC_CHECK_LIB([c], [open])
dnl AC_CHECK_LIB([pthread], [pthread_create])
LIBS="$lfs_save_LIBS"

dnl
dnl Checks for header files.
dnl
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_HEADER_MAJOR
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([arpa/inet.h fcntl.h float.h inttypes.h langinfo.h libintl.h limits.h locale.h malloc.h memory.h netinet/in.h stddef.h stdint.h stdio_ext.h stdlib.h string.h strings.h sys/ioctl.h sys/param.h sys/socket.h sys/time.h syslog.h unistd.h values.h])

dnl
dnl Checks for typedefs, structures, and compiler characteristics.
dnl
AC_HEADER_STAT
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_UID_T
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_CHECK_MEMBERS([struct stat.st_blksize])
AC_STRUCT_ST_BLOCKS
AC_CHECK_MEMBERS([struct stat.st_rdev])
AC_HEADER_TIME
AC_STRUCT_TM
AC_C_VOLATILE
AC_CHECK_TYPES([ptrdiff_t])

dnl Checks for library functions.
dnl
AC_FUNC_ERROR_AT_LINE
if test :${cross_compiling:-no} != :yes ; then :; AC_FUNC_FORK fi
AC_PROG_GCC_TRADITIONAL
AC_FUNC_LSTAT
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
if test :${cross_compiling:-no} != :yes ; then :; AC_FUNC_MALLOC fi
AC_FUNC_MEMCMP
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
if test :${cross_compiling:-no} != :yes ; then :; AC_FUNC_SETPGRP fi
AC_TYPE_SIGNAL
AC_FUNC_STAT
dnl AC_FUNC_STRNLEN
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([alarm bzero gettimeofday inet_ntoa localtime_r mempcpy memset munmap nl_langinfo select setlocale stpcpy strcasecmp strchr strcspn strdup strerror strrchr strstr strtol strtoul])

AC_MODEM

dnl		 doc/Makefile
dnl                 cwmp/Makefile
AC_CONFIG_FILES([Makefile
		 src/modules/Config
		 src/drivers/Config
                 src/libfapi/Makefile
                 ])

AC_OUTPUT
