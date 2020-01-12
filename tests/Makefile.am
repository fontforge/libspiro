# Common CPPFLAGS
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_builddir)

AM_CFLAGS = -I$(top_srcdir) -I$(top_builddir) $(DEP_CFLAGS) $(BABL_CFLAGS) -lm

DEPS = $(top_builddir)/.libs/libspiro.la

LDADDS = $(top_builddir)/.libs/libspiro.la

EXTRA_DIST = call-test.c

# The tests
noinst_PROGRAMS = unit-test  call-test0 call-test1 call-test2 call-test3 \
		  call-test4 call-test5 call-test6 call-test7 call-test8 \
		  call-test9 call-test10 call-test11 call-test12	 \
		  call-test13 call-test14 call-test15 call-test16	 \
		  call-test17 call-test18 call-test19 call-test20	 \
		  call-testm

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

call_test14_SOURCES = call-test14.c
call_test14_LDADD = $(LDADDS)

call_test15_SOURCES = call-test15.c
call_test15_LDADD = $(LDADDS)

call_test16_SOURCES = call-test16.c
call_test16_LDADD = $(LDADDS)

call_test17_SOURCES = call-test17.c
call_test17_LDADD = $(LDADDS)

call_test18_SOURCES = call-test18.c
call_test18_LDADD = $(LDADDS)

call_test19_SOURCES = call-test19.c
call_test19_LDADD = $(LDADDS)

call_test20_SOURCES = call-test20.c
call_test20_LDADD = $(LDADDS)

call_testm_SOURCES = call-testm.c
call_testm_LDADD = $(LDADDS)
if WANTPTHREADS
call_testm_LDADD += -lpthread
endif

TESTS = $(noinst_PROGRAMS)
