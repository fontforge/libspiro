# -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.

# Copyright (C) 2013, by Joe Da Silva

AC_PREREQ([2.61])
#-------------------------------------------
# PackageTimestamp version
m4_define([spiro_package_stamp], "20130930")
#-------------------------------------------
# Making point releases:
#   spiro_major_version += 0;
#   spiro_minor_version += 1; (patches or added function(s))
#
# If any new functions have been added:
#   spiro_major_version += 0;
#   spiro_minor_version += 1; (added function(s))
#
# If backwards compatibility has been broken:
#   spiro_major_version += 1;
#   spiro_minor_version = 0;
#
m4_define([spiro_major_version], '0')
m4_define([spiro_minor_version], '2')
m4_define([spiro_version],
          [spiro_major_version.spiro_minor_version.spiro_package_stamp])
m4_define([spiro_info],
          [spiro_major_version:spiro_minor_version:0])
m4_define([spiro_package_name], ["libspiro"])

#-------------------------------------------
AC_INIT([spiro],[spiro_version],[fontforge-devel@lists.sourceforge.net],
	[spiro_package_name],[https://github.com/fontforge/libspiro])
AC_CONFIG_SRCDIR([spiro.c])
AM_INIT_AUTOMAKE([foreign -Wall])

#-------------------------------------------
AC_CONFIG_MACRO_DIR([m4])

#-------------------------------------------
# automake 1.12 seems to require AM_PROG_AR,
# but automake 1.11 doesn't recognize it.
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

LT_INIT
AC_SUBST(LIBTOOL_DEPS)

AC_PROG_CC
AC_PROG_INSTALL
AC_DISABLE_STATIC
AC_ENABLE_SHARED
AC_PROG_LN_S
AC_PROG_MKDIR_P
AC_PROG_MAKE_SET
AC_PROG_SED
AC_PATH_PROGS([WGET],[wget],[wget])
AC_CONFIG_HEADERS([spiro-config.h])

#-------------------------------------------
# Indicate this is a release build and that
# dependancies for changes between Makefile.am
# and Makefile.in should not be checked. This
# makes compiling faster. If you are working
# on the library, run:
# ./configure --enable-maintainer-mode
# to enable the dependancies
dnl AM_MAINTAINER_MODE([enable])

#-------------------------------------------
# Enable silent build rules by default, this
# requires atleast Automake-1.11. Disable by
# either passing --disable-silent-rules to
# configure or passing V=1 to make
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])],[AC_SUBST([AM_DEFAULT_VERBOSITY],[1])])

#-------------------------------------------
# Check for libraries
AC_SEARCH_LIBS([cos],[m])

#-------------------------------------------
# Check for finite or isfinite. Use one.
AC_CHECK_FUNCS(finite, ,[
  AC_MSG_CHECKING(for finite in <math.h>)
  AC_LINK_IFELSE(
    [AC_LANG_PROGRAM([[#include <math.h>]],[[double f = 0.0; finite(f)]])],
    [AC_DEFINE([HAVE_FINITE],[1],[Define to 1 if you have the finite function.])
     AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no)
     AC_MSG_CHECKING(for isfinite in <math.h>)
     AC_LINK_IFELSE(
       [AC_LANG_PROGRAM([[#include <math.h>]],[[float f = 0.0; isfinite(f)]])],
       [AC_DEFINE([HAVE_ISFINITE],[1],[Define to 1 if you have the isfinite function.])
	AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no)])])])
AC_SUBST(HAVE_FINITE)
AC_SUBST(HAVE_ISFINITE)
AH_BOTTOM([/* Define IS_FINITE(x) to finite(x) or isfinite(x) */
#ifdef HAVE_FINITE
#define IS_FINITE(x) finite(x)
#else
#define IS_FINITE(x) isfinite(x)
#endif])

#-------------------------------------------
# Enable VERBOSE flag in library
AC_ARG_ENABLE([verbose_lib],AS_HELP_STRING([--enable-verbose_lib],[Verbose library output (for debugging).]),
  [case "${enableval}" in
    yes) verbose_lib=true ;;
    no)  verbose_lib=false ;;
    *) AC_MSG_ERROR([bad value ${enableval} for --enable-verbose_lib]) ;;
   esac],[verbose_lib=false])
if test x"${verbose_lib}" = xtrue; then
   AC_DEFINE(VERBOSE,[1],[Verbose library printf() output enabled for debugging.])
fi

#-------------------------------------------
# Enable input CHECK_INPUT_FINITENESS flag
AC_ARG_ENABLE([test_inputs],AS_HELP_STRING([--enable-test_inputs],[Test input values for finiteness.]),
  [case "${enableval}" in
    yes) test_inputs=true ;;
    no)  test_inputs=false ;;
    *) AC_MSG_ERROR([bad value ${enableval} for --enable-test_inputs]) ;;
   esac],[test_inputs=false])
if test x"${test_inputs}" = xtrue; then
   AC_DEFINE(CHECK_INPUT_FINITENESS,[1],[Check if input values are realistic and not infinite in value.])
fi

#-------------------------------------------
# call-test uses pthread for testing (if you
# have it), but libspiro doesn't require it.
AC_CHECK_HEADERS(pthread.h,[havepthreads=true]
  AC_DEFINE(HAVE_PTHREADS,[1],[Have pthreads.h. Do multi-user check in call-test.]),
  [have_pthreads = xfalse])
AM_CONDITIONAL([WANTPTHREADS],[test x$havepthreads = xtrue])

#-------------------------------------------
# Platform specific stuff ($host)
AC_CANONICAL_HOST

#-------------------------------------------
# Pass variables to MAKEFILE.AM
AC_SUBST(HOST,$host)
AC_SUBST(LIBSPIRO_VERSION,$spiro_info)

#-------------------------------------------
# Check for and add usable compiler warnings
AC_LANG_PUSH([C])
AC_SUBST([WCFLAGS],[])
AX_CHECK_COMPILE_FLAG([-Wall],[WCFLAGS+=" -Wall"])
AX_CHECK_COMPILE_FLAG([-Wextra],[WCFLAGS+=" -Wextra"])
AX_CHECK_COMPILE_FLAG([-Wcast-align],[WCFLAGS+=" -Wcast-align"])
AX_CHECK_COMPILE_FLAG([-Wbad-function-cast],[WCFLAGS+=" -Wbad-function-cast"])
AX_CHECK_COMPILE_FLAG([-Wc++-compat],[WCFLAGS+=" -Wc++-compat"])
AX_CHECK_COMPILE_FLAG([-Wmissing-prototypes],[WCFLAGS+=" -Wmissing-prototypes"])
AX_CHECK_COMPILE_FLAG([-Wunused],[WCFLAGS+=" -Wunused"])
AC_LANG_POP

#-------------------------------------------
# Put ifdef wrapper around spiro-config.h so
# that we don't accidently call it twice.
AH_TOP([#ifndef _SPIRO_CONFIG_H])
AH_TOP([#define _SPIRO_CONFIG_H 1])
AH_BOTTOM([#endif])

#-------------------------------------------
AC_PROG_LIBTOOL

AC_CONFIG_FILES([
Makefile
tests/Makefile
libspiro.pc
])

AC_OUTPUT
