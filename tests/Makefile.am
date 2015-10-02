# Common CPPFLAGS
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_builddir)

AM_CFLAGS = -I$(top_srcdir) -I$(top_builddir) $(DEP_CFLAGS) $(BABL_CFLAGS) -lm

DEPS = $(top_builddir)/.libs/libspiro.la

LDADDS = $(top_builddir)/.libs/libspiro.la

# The tests
noinst_PROGRAMS = unit-test  call-test0 call-test1 call-test2 call-test3 \
		  call-test4 call-test5 call-test6 call-test7 call-test8 \
		  call-test9 call-test10 call-test11 call-test12	 \
		  call-test13 call-testm

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

call_test6_SOURCES = call-test6.c
call_test6_LDADD = $(LDADDS)

call_test7_SOURCES = call-test7.c
call_test7_LDADD = $(LDADDS)

call_test8_SOURCES = call-test8.c
call_test8_LDADD = $(LDADDS)

call_test9_SOURCES = call-test9.c
call_test9_LDADD = $(LDADDS)

call_test10_SOURCES = call-test10.c
call_test10_LDADD = $(LDADDS)

call_test11_SOURCES = call-test11.c
call_test11_LDADD = $(LDADDS)

call_test12_SOURCES = call-test12.c
call_test12_LDADD = $(LDADDS)

call_test13_SOURCES = call-test13.c
call_test13_LDADD = $(LDADDS)

call_testm_SOURCES = call-testm.c
call_testm_LDADD = $(LDADDS)
if WANTPTHREADS
call_testm_LDADD += -lpthread
endif

TESTS = $(noinst_PROGRAMS)
