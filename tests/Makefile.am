# Common CPPFLAGS
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_builddir)

AM_CFLAGS = -I$(top_srcdir) -I$(top_builddir) $(DEP_CFLAGS) $(BABL_CFLAGS) -lm

# The tests
noinst_PROGRAMS = unit-test

unit_test_SOURCES = unit-test.c

TESTS = $(noinst_PROGRAMS)
