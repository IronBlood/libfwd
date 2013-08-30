#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([libfwd], [0.1.0], [aplshenyang@gmail.com])
AC_CONFIG_SRCDIR([src/cgi_fcgx_wrapper.c])
AM_INIT_AUTOMAKE
AM_DISABLE_SHARED

# Checks for programs.
AC_PROG_CC
AM_PROG_LIBTOOL
AM_PROG_CC_C_O

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h sys/file.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_CHECK_FUNCS([ftruncate memset munmap strcasecmp strchr strdup strncasecmp strstr])

AC_OUTPUT(Makefile src/Makefile)

