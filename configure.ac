#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#
AC_PREREQ([2.67])
AC_INIT(OpenHeNB, 0.1a, lib.aca55a@gmail.com)

AC_CONFIG_SRCDIR([include/sys/config.h.in])
AC_CONFIG_HEADERS([config.h include/sys/config.h])

AC_CONFIG_AUX_DIR([scripts])
AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)

LT_INIT

# Checks for programs.
# AM_PROG_AS
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_ARG_VAR([LD], [Linker loader command])
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_YACC
AC_SUBST([LIBTOOL_DEPS])

# Checks for libraries.
lfs_save_LIBS="$LIBS"
AC_CHECK_LIB([c], [open])
# AC_CHECK_LIB([pthread], [pthread_create])
LIBS="$lfs_save_LIBS"

# Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_HEADER_MAJOR
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([arpa/inet.h fcntl.h float.h inttypes.h langinfo.h libintl.h limits.h locale.h malloc.h memory.h netinet/in.h stddef.h stdint.h stdio_ext.h stdlib.h string.h strings.h sys/ioctl.h sys/param.h sys/socket.h sys/time.h syslog.h unistd.h values.h types.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STAT
AC_HEADER_STDBOOL
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
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

# Checks for library functions.
#
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
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([alarm bzero gettimeofday inet_ntoa localtime_r mempcpy memset munmap nl_langinfo select setlocale stpcpy strcasecmp strchr strcspn strdup strerror strrchr strstr strtol strtoul])

AC_MSG_NOTICE([Checking for MODEM and its requirements.])
AC_MODEM

#		 doc/Makefile
#                 cwmp/Makefile
AC_CONFIG_FILES([Makefile
		 src/modules/Config
		 src/drivers/Config
                 src/libfapi/Makefile
                 ])

AC_OUTPUT
