prefix = /usr/local
exec_prefix = ${prefix}

srcdir = .
top_srcdir = .
top_builddir = .

bindir = ${exec_prefix}/bin
libdir = ${exec_prefix}/lib
includedir = ${prefix}/include

FFSPIRO_VERSION=0
FFSPIRO_REVISION=1
FFSPIRO_AGE=0

LIBTOOL = $(SHELL) $(top_builddir)/libtool
INSTALL = /usr/bin/install -c
INSTALL_DATA = ${INSTALL} -m 644
CC = gcc

_CFLAGS = -I$(srcdir)/../inc -I$(srcdir) -I. -I$(srcdir)/../fontforge \
    -Wmissing-prototypes -Wunused -Wimplicit -Wreturn-type -Wparentheses -Wformat -Wchar-subscripts $(X_CFLAGS) \
    -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_DLFCN_H=1  '-DSHAREDIR="$(sharedir)"' -DLIBDIR='"$(libdir)"' \
     '-DPREFIX="/usr/local"'
CFLAGS = -g -O2 $(_CFLAGS)

O=lo
LIB_FF_SPIRO_OBJECTS = spiro.$O bezctx.$O spiroentrypoints.$O

LIBS = -lm

all: libspiro.la


clean:
	-rm -rf .libs *.la *.lo

distclean: clean
	-rm -rf Makefile
	-rm -rf bezctx.c bezctx.h bezctx_intf.h spiro.c spiro.h zmisc.h README-RaphLevian gpl.txt

get-spiro-src:
	bash < get-spiro-src.sh

%.lo: %.c
	$(LIBTOOL) --mode=compile $(CC) $(CFLAGS) -c $<

libspiro.la: $(LIB_FF_SPIRO_OBJECTS)
	$(LIBTOOL) --mode=link $(CC) $(CFLAGS) -no-undefined   -o libspiro.la $(LIB_FF_SPIRO_OBJECTS) $(LIBS) \
	     \
	    -rpath $(libdir) -version-info $(FFSPIRO_VERSION):$(FFSPIRO_REVISION):$(FFSPIRO_AGE)

install: all
	mkdir -p $(DESTDIR)$(libdir)
	$(LIBTOOL) --mode=install $(INSTALL) -c libspiro.la $(DESTDIR)$(libdir)/libspiro.la
	$(LIBTOOL) --mode=finish $(DESTDIR)$(libdir)
	$(INSTALL_DATA) bezctx.h $(includedir)
	$(INSTALL_DATA) bezctx_intf.h $(includedir)
	$(INSTALL_DATA) spiro.h $(includedir)
	$(INSTALL_DATA) spiroentrypoints.h $(includedir)
