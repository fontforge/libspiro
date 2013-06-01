# Makefile.am - Top level automakefile for libspiro

ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

AM_CFLAGS = $(WCFLAGS)
AM_LDFLAGS = $(WCFLAGS) -no-undefined --mode=link

DISTCLEANFILES = libspiro.pc

lib_LTLIBRARIES = libspiro.la

libspiro_la_LDFLAGS = $(AM_LDFLAGS) -version-info $(FFSPIRO_VERSION)
libspiro_la_SOURCES = spiro.c bezctx.c spiroentrypoints.c
libspiro_la.$(OBJEXT): bezctx.h bezctx_intf.h spiro.h spiroentrypoints.h

EXTRA_DIST = bezctx.md get-spiro-src.sh README-RaphLevien README.md zmisc.h \
	closedspiro.png openspiro.png spiral16.png spiral32.png spiro-a.png \
	java/ShowPlate.java java/Spiro.java java/SpiroBezierContext.java    \
	java/SpiroCanvas.java java/SpiroCP.java java/SpiroGeneralPath.java  \
	java/SpiroPointType.java

include_HEADERS = bezctx.h bezctx_intf.h spiro.h spiroentrypoints.h

