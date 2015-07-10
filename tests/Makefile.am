# Common CPPFLAGS
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_builddir)

AM_CFLAGS = -I$(top_srcdir) -I$(top_builddir) $(DEP_CFLAGS) $(BABL_CFLAGS) -lm

DEPS = $(top_builddir)/.libs/libspiro.la

LDADDS = $(top_builddir)/.libs/libspiro.la

# The tests
noinst_PROGRAMS = unit-test call-test0 call-test1 call-test2 call-test3 \
		  call-test4 call-test5 call-testm

unit_test_SOURCES = unit-test.c

call_test0_SOURCES = call-test0.c
call_test0_LDADD = $(LDADDS)

call_test1_SOURCES = call-test1.c
call_test1_LDADD = $(LDADDS)

call_test2_SOURCES = call-test2.c
call_test2_LDADD = $(LDADDS)

call_test3_SOURCES = call-test3.c
call_test3_LDADD = $(LDADDS)

call_test4_SOURCES = call-test4.c
call_test4_LDADD = $(LDADDS)

call_test5_SOURCES = call-test5.c
call_test5_LDADD = $(LDADDS)

call_testm_SOURCES = call-testm.c
call_testm_LDADD = $(LDADDS)
if WANTPTHREADS
call_testm_LDADD += -lpthread
endif

TESTS = $(noinst_PROGRAMS)
