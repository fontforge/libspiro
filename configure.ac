# -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.

# Copyright (C) 2013,2014,2015 by Joe Da Silva

AC_PREREQ([2.61])
#-------------------------------------------
# PackageTimestamp version
m4_define([spiro_package_stamp], [20150702])
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
m4_define([spiro_major_version], [0])
m4_define([spiro_minor_version], [5])
m4_define([spiro_version],
          [spiro_major_version.spiro_minor_version.spiro_package_stamp])
m4_define([spiro_info],
          [spiro_major_version:spiro_minor_version:0])
m4_define([spiro_package_name], [libspiro])

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
AC_SUBST([LIBTOOL_DEPS])

AC_PROG_CC
AC_PROG_INSTALL
AC_ENABLE_SHARED
AC_PROG_LN_S
AC_PROG_MKDIR_P
AC_PROG_MAKE_SET
AC_PROG_SED
AC_PATH_PROGS([VALGRIND],[valgrind])
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
# Check for math.h include and math library.
# Old distros, use CHECK as a SEARCH backup.
AC_CHECK_HEADER([math.h],
  AC_SEARCH_LIBS([cos],[m],[have_libm=yes],
    AC_CHECK_LIB([m],[cos],[have_libm=yes])))
if test x"${have_libm}" != xyes; then
   AC_MSG_FAILURE([ERROR: Please install Math libraries and math.h include files for libm],[1])
fi

#-------------------------------------------
# Check for C99 hypot. Error if none found.
# Older compilers will have to set/use C99.
AC_CHECK_FUNCS(hypot, ,[AC_MSG_FAILURE([ERROR: hypot() required but not found.],[1])])

#-------------------------------------------
# Check for C99 finite or isfinite. Use one.
math_finite=no
math_isfinite=no
AC_CHECK_FUNCS(finite,
  [AC_SUBST(HAVE_FINITE)
   math_finite=true],
    AC_CHECK_FUNCS(isfinite,
      [AC_SUBST(HAVE_ISFINITE)
       math_isfinite=true]))

if test x"${math_finite}" = xno && test x"${math_isfinite}" = xno; then
  AC_MSG_FAILURE([ERROR: finite() or isfinite() required but not found.],[1])
fi

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
AC_CHECK_HEADER([pthread.h],[havepthreads=true]
  AC_DEFINE(HAVE_PTHREADS,[1],[Have pthreads.h. Do multi-user check in call-test.]),
  [have_pthreads = xfalse])
AM_CONDITIONAL([WANTPTHREADS],[test x$havepthreads = xtrue])

#-------------------------------------------
# call-test uses sys/timeb.h to display test
# time lengths but libspiro doesn't need it.
AC_CHECK_FUNC([gettimeofday],[have_gettimeofday=true],[have_gettimeofday=false])
AC_CHECK_HEADER([sys/time.h],[],[have_gettimeofday=false])
if test x"${have_gettimeofday}" = xtrue; then
  AC_DEFINE_UNQUOTED([DO_TIME_DAY],[1],[Use 'gettimeofday()==true, else use older sys/timeb.h])
else
  AC_CHECK_HEADER([sys/timeb.h],[],
    [AC_MSG_ERROR([ERROR: Please install time.h and sys/timeb.h developer files],[1])])
fi

#-------------------------------------------
# The 'make check' test 'call-testm.c' runs
# many threads at once to test calculations.
# This number is set at a default of 100 to
# meet automated-test 'time-out' deadlines.
# Recommend maintainers/developers run 1000+
testalot=100
AC_ARG_ENABLE([test_a_lot],
  AS_HELP_STRING([--enable-test_a_lot],[Run lots of multithread tests.]),
  [testalot=2000])
AC_DEFINE_UNQUOTED([S_TESTP],[$testalot],
  [Do 'S_TESTS' multi-thread 'call-testm' checks when you run 'make check'.])

#-------------------------------------------
# Platform specific stuff ($host)
AC_CANONICAL_HOST

#-------------------------------------------
# Compiler and Linker flags to override auto
# detection and insertion. Use CFLAGS if you
# want to add more in addition to autodetect
AC_ARG_VAR([LS_CFLAGS],[C compiler flags for libspiro, overriding automatic detection])
AC_ARG_VAR([LS_LIB],[Linker flags for libspiro, overriding automatic detection])

#-------------------------------------------
# Check for and add usable compiler warnings
# Skip if replacing with LS_CFLAGS instead.
WCFLAGS=""
AC_LANG_PUSH([C])
AX_CHECK_COMPILE_FLAG([-Wall],[WCFLAGS="$WCFLAGS -Wall"])
AX_CHECK_COMPILE_FLAG([-Wextra],[WCFLAGS="$WCFLAGS -Wextra"])
AX_CHECK_COMPILE_FLAG([-Wcast-align],[WCFLAGS="$WCFLAGS -Wcast-align"])
AX_CHECK_COMPILE_FLAG([-Wbad-function-cast],[WCFLAGS="$WCFLAGS -Wbad-function-cast"])
AX_CHECK_COMPILE_FLAG([-Wc++-compat],[WCFLAGS="$WCFLAGS -Wc++-compat"])
AX_CHECK_COMPILE_FLAG([-Wmissing-prototypes],[WCFLAGS="$WCFLAGS -Wmissing-prototypes"])
AX_CHECK_COMPILE_FLAG([-Wunused],[WCFLAGS="$WCFLAGS -Wunused"])

dnl  AX_CHECK_COMPILE_FLAG([-Wconversion],[WCFLAGS="$WCFLAGS -Wconversion"])
dnl  AX_CHECK_COMPILE_FLAG([-Wsign-conversion],[WCFLAGS="$WCFLAGS -Wsign-conversion"])
dnl  AX_CHECK_COMPILE_FLAG([-Wformat=2],[WCFLAGS="$WCFLAGS -Wformat=2"])
dnl  AX_CHECK_COMPILE_FLAG([-Wformat-security],[WCFLAGS="$WCFLAGS -Wformat-security"])
dnl  AX_CHECK_COMPILE_FLAG([-fno-common],[WCFLAGS="$WCFLAGS -fno-common"])
dnl  AX_CHECK_COMPILE_FLAG([-Wmissing-declarations],[WCFLAGS="$WCFLAGS -Wmissing-declarations"])
dnl  AX_CHECK_COMPILE_FLAG([-Wstrict-prototypes],[WCFLAGS="$WCFLAGS -Wstrict-prototypes"])
dnl   AX_CHECK_COMPILE_FLAG([-fPIC],[WCFLAGS="$WCFLAGS -fPIC"])
dnl   AX_CHECK_COMPILE_FLAG([-Wstrict-overflow],[WCFLAGS="$WCFLAGS -Wstrict-overflow"])
dnl   AX_CHECK_COMPILE_FLAG([-Wpointer-arith],[WCFLAGS="$WCFLAGS -Wpointer-arith"])
dnl   AX_CHECK_COMPILE_FLAG([-Wcast-qual],[WCFLAGS="$WCFLAGS -Wcast-qual"])
dnl   AX_CHECK_COMPILE_FLAG([-Wcast-align],[WCFLAGS="$WCFLAGS -Wcast-align"])
dnl   AX_CHECK_COMPILE_FLAG([-Wpadded],[WCFLAGS="$WCFLAGS -Wpadded"])
dnl   AX_CHECK_COMPILE_FLAG([-Woverlength-strings],[WCFLAGS="$WCFLAGS -Woverlength-strings"])
AC_LANG_POP
# Skip if replacing with LS_LIB instead.
WLSLIB=""
if test -z $LS_LIB ; then
   WLSLIB="${WCFLAGS}"
fi
if test "${LS_CFLAGS}"x != x; then
   WCFLAGS=""
fi

#-------------------------------------------
# Put ifdef wrapper around spiro-config.h so
# that we don't accidently call it twice.
AH_TOP([#ifndef _SPIRO_CONFIG_H
#define _SPIRO_CONFIG_H 1])
AH_BOTTOM([#endif])

#-------------------------------------------
# Pass variables to MAKEFILE.AM
AC_SUBST([HOST],[host])
AC_SUBST([LIBSPIRO_VERSION],[spiro_info])
AC_SUBST([LS_CFLAGS])
AC_SUBST([LS_LIB])
AC_SUBST([WCFLAGS])
AC_SUBST([WLSLIB])
AM_CONDITIONAL([HAVEVALGRIND],[test "${VALGRIND}"x != x])

#-------------------------------------------
AC_PROG_LIBTOOL

AC_CONFIG_FILES([
Makefile
tests/Makefile
libspiro.pc
])

AC_OUTPUT

AC_MSG_NOTICE([

Configuration:
  Source code location	${srcdir}
  Build code location	${builddir}
  Destination prefix	${prefix}
  Compiler		${CC}
  Config auto WCFLAGS	"${WCFLAGS}"
  Config auto WLSLIB	"${WLSLIB}"
  Config CFLAGS		"${CFLAGS}"
  Config LDFLAGS	"${LDFLAGS}"
  Config LS_CFLAGS	"${LS_CFLAGS}"
  Config LS_LIB		"${LS_LIB}"

])
