# Makefile.am - Top level automakefile for libspiro

SUBDIRS = . tests

test_programs = call-test0 call-test1 call-test2 call-test3 call-test4 \
		call-test5 call-test6 call-test7 call-test8 call-test9 \
		call-test10 call-test11 call-test12 call-test13        \
		call-test14 call-test15 call-test16 call-test17        \
		call-test18 call-test19 call-test20 call-testm

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libspiro.pc

ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

AM_CFLAGS = $(WCFLAGS) $(LS_CFLAGS)
AM_LDFLAGS = $(WLSLIB) $(LS_LIB) -no-undefined --mode=link

LIBTOOL_DEPS = @LIBTOOL_DEPS@

DISTCLEANFILES = libspiro.pc
DISTCHECK_CONFIGURE_FLAGS = --enable-verbose_lib --enable-test_inputs --enable-test-a-lot

lib_LTLIBRARIES = libspiro.la

libspiro_la_LDFLAGS = $(AM_LDFLAGS) -version-info $(LIBSPIRO_VERSION)
libspiro_la_SOURCES = spiro.c bezctx.c spiroentrypoints.c
libspiro_la.$(OBJEXT): bezctx.h bezctx_intf.h spiro.h spiroentrypoints.h

EXTRA_DIST = bezctx.md get-spiro-src.sh README-RaphLevien README.md	    \
	closedspiro.png openspiro.png spiral16.png spiral32.png spiro-a.png \
	java/ShowPlate.java java/Spiro.java java/SpiroBezierContext.java    \
	java/SpiroCanvas.java java/SpiroCP.java java/SpiroGeneralPath.java  \
	java/SpiroPointType.java README-GeorgeWilliams fontforge-debug.txt  \
	fontforge.sh m4/ax_check_compile_flag.m4 path5.png path6.png libspiro.3

include_HEADERS = bezctx.h bezctx_intf.h spiro.h spiroentrypoints.h
man_MANS = libspiro.3

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status libtool

# test: run all tests in cwd and subdirs
test:	$(TEST_PROGS)

.PHONY: test
# run make test as part of make check
check-local: test

check-valgrind:
if HAVEVALGRIND
	$(MAKE);
	$(VALGRIND) --error-exitcode=1 $(builddir)/tests/unit-test
	(for arg in $(test_programs); do \
	$(LIBTOOL) --mode=execute $(VALGRIND) --error-exitcode=1 $(builddir)/tests/$$(arg) ; \
	done)
else
	echo "install Valgrind, then run ./configure again"
endif

