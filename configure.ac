#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([jack_capture], [0.9.61], [http://github.com/kmatheussen/jack_capture/issues])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([./])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_CPP

# Checks for libraries.
# FIXME: Replace `main' with a function in `-la':
AC_CHECK_LIB([a], [main])
AC_CHECK_LIB([jack], [jack_client_open])
AC_CHECK_LIB([m], [sin])
AC_CHECK_LIB([pthread], [pthread_create])
AC_CHECK_LIB([sndfile], [sf_open])
AC_CHECK_LIB([mp3lame], [lame_init])

AC_SEARCH_LIBS(jack_port_get_latency_range, jack,
               AC_DEFINE(NEW_JACK_LATENCY_API=1))

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdint.h stdlib.h string.h sys/time.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT64_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([clock_gettime dup2 memset pow rint strcasecmp strdup strerror strtol sysinfo])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

