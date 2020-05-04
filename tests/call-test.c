/* Test libspiro normal library calls
Copyright (C) 2013... Joe Da Silva

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 3
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA.
*/

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "spiro-config.h"	/* for ./configure test settings like VERBOSE */
#ifdef DO_TIME_DAY
#include <sys/time.h>		/* for gettimeofday */
#else
#include <sys/timeb.h>		/* for old get_time */
#endif

#include "spiroentrypoints.h"	/* call spiro through here */
#include "bezctx.h"		/* bezctx structure */

#ifdef DO_CALL_TESTM
#ifdef HAVE_PTHREADS
#include <pthread.h>		/* multi-thread check. Not part of libspiro */
#endif
#endif

static double get_time (void) {
#ifdef DO_TIME_DAY
    struct timeval tv;

    gettimeofday(&tv, NULL);

    return tv.tv_sec + 1e-6 * tv.tv_usec;
#else
    struct timeb tb;

    ftime(&tb);

    return tb.time + 1e-3 * tb.millitm;
#endif
}

#ifndef DO_CALL_TESTM
/* Expected run_spiro() test data results for path0/1/2/4/5/6[]. */
typedef struct {
    double b, ch, th;
} rs_check_vals;

rs_check_vals verify_rs0[] = {		/* iteration5 */
    {0.000000, 65.741920, 2.027641},	/* v,334,117 */
    {0.000000, 99.020200, -2.791096},	/* v,305,176 */
    {-0.851164, 60.415230, 2.640925},	/* c,212,142 */
    {-1.847893, 92.633687, 0.793032},	/* c,159,171 */
    {-0.120277, 157.267288, 0.672755},	/* c,224,237 */
    {1.730337, 196.084166, 2.403091},	/* c,347,335 */
    {1.042797, 126.826653, -2.837297},	/* c,202,467 */
    {0.000000, 69.354164, -1.074902},	/* v, 81,429 */
    {0.000000, 93.407708, 0.372554},	/* v,114,368 */
    {-0.787061, 81.939002, -0.414507},	/* c,201,402 */
    {-1.916483, 84.172442, -2.330990},	/* c,276,369 */
    {-0.158341, 159.806133, -2.489331},	/* c,218,308 */
    {1.237282, 105.304321, -1.252049},	/* c, 91,211 */
    {0.982576, 108.931171, -0.269473},	/* c,124,111 */
    {0.591223, 110.679718, 0.321751}	/* c,229, 82 */
};					/* z */

rs_check_vals verify_rs1[] = {		/* iteration6 */
    {0.000000, 697.685459, -0.287750},	/* {, 80,738 */
    {-1.608688, 243.813453, -1.896438},	/* o,749,540 */
    {-1.770739, 173.404152, 2.616009},	/* o,671,309 */
    {0.937994, 157.178243, -2.729182},	/* o,521,396 */
    {1.881365, 136.029409, -0.847817}	/* o,377,333 */
};					/* },467,231 */

rs_check_vals verify_rs2[] = {		/* iteration26 */
    {0.000000, 108.115679, -0.046263},	/* {,233,143 */
    {-0.926114, 79.881162, -0.972377},	/* o,341,138 */
    {1.844174, 90.138782, 0.871797}	/* o,386, 72 */
};					/* },444,141 */

rs_check_vals verify_rs4[] = {		/* iteration2 */
    {-1.570796, 141.421356, 0.785398},	/* o,-100,  0 */
    {-1.570796, 141.421356, -0.785398},	/* o,  0, 100 */
    {-1.570796, 141.421356, -2.356194}	/* o,100,   0 */
};					/* o,  0,-100 */

rs_check_vals verify_rs5[] = {		/* iteration4	verify_rs6 */
    {0.000000, 141.421356, 0.785398},	/* {,  0,   0	{,  0,   0 */
    {0.000000, 141.421356, 0.785398},	/* c,100, 100	c,100, 100 */
    {-0.785398, 100.000000, 0.000000},	/* [,200, 200	a,200, 200 */
    {-0.463648, 111.803399, -0.463648},	/* ],300, 200	h,300, 200 */
    {-2.214297, 111.803399, -2.677945},	/* c,400, 150	c,300, 150 */
    {-0.463648, 100.000000, 3.141593},	/* [,300, 100	a,200, 100 */
    {0.785398, 70.710678, -2.356194},	/* ],200, 100	h,100, 100 */
    {0.000000, 70.710678, -2.356194},	/* c,150,  50	c,150,  50 */
    {0.000000, 141.421356, -2.356194},	/* [,100,   0	a,100,   0 */
    {0.321751, 111.803399, -2.034444},	/* ],  0,-100	h,  0,-100 */
    {-0.076772, 58.309519, -2.111216}	/* c,-50,-200	c, 50,-100 */
};					/* },-80,-250	}, 20,-150 */

rs_check_vals verify_rs7[] = {		/* iteration4 */
    {-0.950547, 50.990195, 0.197396},	/* o,  0, 200 */
    {-1.534449, 215.870331, -1.337053},	/* o, 50, 210 */
    {-0.233743, 50.000000, -1.570796},	/* a,100,   0 */
    {-0.484478, 214.709106, -2.055274},	/* h,100, -50 */
    {-0.986650, 50.249378, -3.041924},	/* a,  0,-190 */
    {-1.228091, 210.237960, 2.013171},	/* h,-50,-195 */
    {-0.342706, 50.249378, 1.670465},	/* a,-90,   0 */
    {-0.522523, 219.317122, 1.147942}	/* h,-95,  50 */
};					/* z, path12  */

rs_check_vals verify_rs8[] = {		/* iteration4 */
    {-0.950547, 50.990195, 0.197396},	/* o,  0, 200 */
    {-1.534449, 215.870331, -1.337053},	/* o, 50, 210 */
    {-0.233743, 50.000000, -1.570796},	/* a,100,   0 */
    {-0.484478, 214.709106, -2.055274},	/* h,100, -50 */
    {-0.986650, 50.249378, -3.041924},	/* a,  0,-190 */
    {-1.228091, 210.237960, 2.013171},	/* h,-50,-195 */
    {-0.342706, 50.249378, 1.670465}	/* a,-90,   0 */
};					/* h,-95,  50 */

rs_check_vals verify_rs9[] = {		/* iteration9 */
    {-1.570796, 141.421356, 0.785398},	/* o,-100,  0 */
    {-1.570796, 141.421356, -0.785398},	/* o,  0, 100 */
    {-1.570796, 141.421356, -2.356194},	/* o,100,   0 */
    {-1.570796, 141.421356, 2.356194}	/* o,  0,-100 */
};

/* test path10[] path11[] using '[]' instead of 'ah'. */
#define TEST_10_11 1
rs_check_vals verify_rs10[] = {		/* iteration4 */
#ifndef TEST_10_11
    {-0.233743, 50.000000, -1.570796},	/* a,100,   0 */
    {-0.484478, 214.709106, -2.055274},	/* h,100, -50 */
    {-0.986650, 50.249378, -3.041924},	/* a,  0,-190 */
    {-1.228091, 210.237960, 2.013171},	/* h,-50,-195 */
    {-0.342706, 50.249378, 1.670465},	/* a,-90,   0 */
    {-0.522523, 219.317122, 1.147942},	/* h,-95,  50 */
    {-0.950547, 50.990195, 0.197396},	/* o,  0, 200 */
    {-1.534449, 215.870331, -1.337053}	/* o, 50, 210 */
#else
    {-0.256151, 50.000000, -1.570796},	/* [,100,   0 */
    {-0.484478, 214.709106, -2.055274},	/* ],100, -50 */
    {-0.986650, 50.249378, -3.041924},	/* a,  0,-240 */
    {-1.228091, 210.237960, 2.013171},	/* h,-50,-245 */
    {-0.342706, 50.249378, 1.670465},	/* [,-90, -50 */
    {-0.522523, 219.317, 1.14794},	/* ],-95,   0 */
    {-0.950547, 50.990195, 0.197396},	/* o, -5, 200 */
    {-1.512041, 217.082933, -1.314645}	/* o, 45, 210 */
#endif
};

rs_check_vals verify_rs11[] = {		/* iteration4 */
#ifndef TEST_10_11
    {-0.233743, 50.000000, -1.570796},	/* a,100,   0 */
    {-0.484478, 214.709106, -2.055274},	/* h,100, -50 */
    {-0.986650, 50.249378, -3.041924},	/* a,  0,-190 */
    {-1.228091, 210.237960, 2.013171},	/* h,-50,-195 */
    {-0.342706, 50.249378, 1.670465},	/* a,-90,   0 */
    {-0.522523, 219.317122, 1.147942},	/* h,-95,  50 */
    {-0.950547, 50.990195, 0.197396},	/* o,  0, 200 */
    {-1.534449, 215.870331, -1.337053}	/* o, 50, 210 */
#else
    {-0.256151, 50.000000, -1.570796},	/* [,100,   0 */
    {-0.484478, 214.709106, -2.055274},	/* ],100, -50 */
    {-0.986650, 50.249378, -3.041924},	/* a,  0,-240 */
    {-1.228091, 210.237960, 2.013171},	/* h,-50,-245 */
    {-0.342706, 50.249378, 1.670465},	/* [,-90, -50 */
    {-0.522523, 219.317122, 1.147942},	/* ],-95,   0 */
    {-0.950547, 50.990195, 0.197396},	/* o, -5, 200 */
    {-1.512041, 217.0829933, -1.314645}	/* o, 45, 210 */
#endif
};					/* a,100,   0 */

rs_check_vals verify_rs13[] = {		/* iteration4 */
    { 0.000000, 50.000000, -1.570796},	/* {,100,   0 */
    {-0.484478, 214.709106, -2.055274},	/* h,100, -50 */
    {-0.986650, 50.249378, -3.041924},	/* a,  0,-190 */
    {-1.228091, 210.237960, 2.013171},	/* h,-50,-195 */
    {-0.342706, 50.249378, 1.670465},	/* a,-90,   0 */
    {-0.522523, 219.317122, 1.147942},	/* h,-95,  50 */
    {-0.950547, 50.990195, 0.197396},	/* o,  0, 200 */
    {-1.534449, 215.870331, -1.337053}	/* }, 50, 210 */
};					/* a,100,   0 */

rs_check_vals verify_rs14[] = {			/* iteration9	     */
    {-1.570796, 141.421356e10, 0.785398},	/* o,-100e10,	   0 */
    {-1.570796, 141.421356e10, -0.785398},	/* o,	   0, 100e10 */
    {-1.570796, 141.421356e10, -2.356194},	/* o, 100e10,	   0 */
    {-1.570796, 141.421356e10, 2.356194}	/* o,	   0,-100e10 */
};

rs_check_vals verify_rs15[] = {			/* iteration9	       */
    {-1.570796, 141.421356e-42, 0.785398},	/* o,-100e-42,	     0 */
    {-1.570796, 141.421356e-42, -0.785398},	/* o,	    0, 100e-42 */
    {-1.570796, 141.421356e-42, -2.356194},	/* o, 100e-42,	     0 */
    {-1.570796, 141.421356e-42, 2.356194}	/* o,	    0,-100e-42 */
};
#endif

void load_test_curve(spiro_cp *spiro, int *nextknot, int c) {
/* load a test curve (and nextknot locations) into memory */
    spiro_cp path0[] = { /* ...came with unit-test */
	{334, 117, 'v'},
	{305, 176, 'v'},
	{212, 142, 'c'},
	{159, 171, 'c'},
	{224, 237, 'c'},
	{347, 335, 'c'},
	{202, 467, 'c'},
	{ 81, 429, 'v'},
	{114, 368, 'v'},
	{201, 402, 'c'},
	{276, 369, 'c'},
	{218, 308, 'c'},
	{ 91, 211, 'c'},
	{124, 111, 'c'},
	{229,  82, 'c'},
	{0, 0, 'z'}
    };
    int knot0[] = {
	1, 1, 3, 3, 2, 3, 1, 1, 1, 3, 3, 2, 2, 2, 1, 1
    };
    spiro_cp path1[] = { /* do a test using "{"..."}" */
	{ 80, 738, '{'},
	{749, 540, 'o'},
	{671, 309, 'o'},
	{521, 396, 'o'},
	{377, 333, 'o'},
	{467, 231, '}'}
    };
    int knot1[] = {
	3, 4, 2, 3, 2, 0
    };
    spiro_cp path2[] = { /* this does many iterations */
	{233, 143, '{'},
	{341, 138, 'o'},
	{386,  72, 'o'},
	{444, 141, '}'}
    };
    int knot2[] = {
	2, 1, 9, 0
    };
    spiro_cp path3[] = { /* this will fail to converge */
	{233, 144, '{'}, /* will not pass (on purpose) */
	{341, 138, 'o'},
	{386,  72, 'o'},
	{443, 141, 'o'},
	{467, 231, 'o'},
	{377, 333, '}'}
    };
    int knot3[] = {
	2, 1, 9, 1, 1, 0
    };
    spiro_cp path4[] = { /* test a cyclic calculation. */
	{-100,    0, SPIRO_G4},
	{   0,  100, SPIRO_G4},
	{ 100,    0, SPIRO_G4},
	{   0, -100, SPIRO_G4}
    };
    int knot4[] = {
	2, 2, 2, 0
    };
    spiro_cp path5[] = { /* verify curve data with []. */
	{  0,   0, '{'},
	{100, 100, 'c'},
	{200, 200, '['},
	{300, 200, ']'},
	{400, 150, 'c'},
	{300, 100, '['},
	{200, 100, ']'},
	{150,  50, 'c'},
	{100,   0, '['},
	{  0,-100, ']'},
	{-50,-200, 'c'},
	{-80,-250, '}'} /* test 23 reverses this list. */
    };
    int knot5[] = {
	1, 4, 1, 3, 3, 1, 4, 2, 1, 2, 1, 0
    };
    spiro_cp path6[] = { /* verify curve data with ah. */
	{  0,   0, '{'},
	{100, 100, 'c'},
	{200, 200, 'a'},
	{300, 200, 'h'},
	{300, 150, 'c'},
	{200, 100, 'a'},
	{100, 100, 'h'},
	{150,  50, 'c'},
	{100,   0, 'a'},
	{  0,-100, 'h'},
	{ 50,-100, 'c'},
	{ 20,-150, '}'}
    };
    int knot6[] = {
	1, 4, 3, 3, 4, 2, 2, 1, 0, 0, 0, 0
    };
    spiro_cp path7[] = { /* loop stops with ah curves. */
	{  0, 200, 'o'},
	{ 50, 210, 'o'},
	{100,   0, 'a'},
	{100, -50, 'h'},
	{  0,-190, 'a'},
	{-50,-195, 'h'},
	{-90,   0, 'a'},
	{-95,  50, 'h'},/* tests 7, 8, 20, 21 end here */
	{  0,   0, 'z'} /* call_test 12 ends with ah z */
    };
    int knot7[] = {
	1, 1, 5, 5, 6, 3, 0, 0, 0
    };
    spiro_cp path10[] = { /* start loop with ah curves */
#ifndef TEST_10_11
	{100,   0, 'a'},
	{100, -50, 'h'},
	{  0,-190, 'a'},
	{-50,-195, 'h'},
	{-90,   0, 'a'},
	{-95,  50, 'h'},
	{  0, 200, 'o'},
	{ 50, 210, 'o'}
#else
	{100,   0, '['},
	{100, -50, ']'},
	{  0,-240, 'a'},
	{-50,-245, 'h'},
	{-90, -50, '['},
	{-95,   0, ']'},
	{ -5, 200, 'o'},
	{ 45, 210, 'o'}
#endif
    };
    int knot10[] = {
	5, 6, 3, 1, 1, 5, 0, 0
    };
    spiro_cp path13[] = { /* start open curve using {h */
	{100,   0, '{'},
	{100, -50, 'h'},
	{  0,-190, 'a'},
	{-50,-195, 'h'},
	{-90,   0, 'a'},
	{-95,  50, 'h'},
	{  0, 200, 'o'},
	{ 50, 210, '}'}
    };
    int knot13[] = {
	5, 6, 3, 1, 1, 5, 0, 0
    };
    spiro_cp path14[] = { /* very big path[4] version. */
	{-100e10,    0, SPIRO_G4},
	{   0,  100e10, SPIRO_G4},
	{ 100e10,    0, SPIRO_G4},
	{   0, -100e10, SPIRO_G4}
    };
    spiro_cp path15[] = { /* very tiny path[4] version */
	{-100e-42,    0, SPIRO_G4},
	{   0,  100e-42, SPIRO_G4},
	{ 100e-42,    0, SPIRO_G4},
	{   0, -100e-42, SPIRO_G4}
    };
    int i;

    /* Load static variable tables into memory because */
    /* SpiroCPsToBezier0() modifies start & end values */
    /* Later call-tests will also modify these so that */
    /* we can verify multi-threads aren't overwriting. */
    if ( c==0 ) for (i = 0; i < 16; i++) {
	spiro[i].x = path0[i].x;
	spiro[i].y = path0[i].y;
	spiro[i].ty = path0[i].ty;
	nextknot[i] = knot0[i];
    } else if ( c==1 ) for (i = 0; i < 6; i++) {
	spiro[i].x = path1[i].x;
	spiro[i].y = path1[i].y;
	spiro[i].ty = path1[i].ty;
	nextknot[i] = knot1[i];
    } else if ( c==2 ) for (i = 0; i < 4; i++) {
	spiro[i].x = path2[i].x;
	spiro[i].y = path2[i].y;
	spiro[i].ty = path2[i].ty;
	nextknot[i] = knot2[i];
    } else if ( c==3 ) for (i = 0; i < 6; i++) {
	spiro[i].x = path3[i].x;
	spiro[i].y = path3[i].y;
	spiro[i].ty = path3[i].ty;
	nextknot[i] = knot3[i];
    } else if ( c==4 ) for (i = 0; i < 4; i++) {
	spiro[i].x = path4[i].x;
	spiro[i].y = path4[i].y;
	spiro[i].ty = path4[i].ty;
	nextknot[i] = knot4[i];
    } else if ( c==5 ) for (i = 0; i < 12; i++) {
	spiro[i].x = path5[i].x;
	spiro[i].y = path5[i].y;
	spiro[i].ty = path5[i].ty;
	nextknot[i] = knot5[i];
    } else if ( c==6 ) for (i = 0; i < 12; i++) {
	spiro[i].x = path6[i].x;
	spiro[i].y = path6[i].y;
	spiro[i].ty = path6[i].ty;
	nextknot[i] = knot6[i];
    } else if ( c==7 || c==8 || c==21 ) for (i = 0; i < 8; i++) {
	/* path7[] is closed curve, path{8,21}[] are open curves. */
	spiro[i].x = path7[i].x;
	spiro[i].y = path7[i].y;
	spiro[i].ty = path7[i].ty;
	nextknot[i] = knot7[i];
    } else if ( c==9 ) for (i = 0; i < 4; i++) {
	/* path9[] is closed curve version of path4[] open curve. */
	spiro[i].x = path4[i].x;
	spiro[i].y = path4[i].y;
	spiro[i].ty = path4[i].ty;
	nextknot[i] = knot4[i];
    } else if ( c==10 || c==11 ) for (i = 0; i < 8; i++) {
	/* path10[]_co[10]=closedcurve, path11[]_co[10]=opencurve */
	spiro[i].x = path10[i].x;
	spiro[i].y = path10[i].y;
	spiro[i].ty = path10[i].ty;
	nextknot[i] = knot10[i];
    } else  if ( c==12 ) for (i = 0; i < 9; i++) {
	/* call_test12 checks curve ending in ah with following z */
	/* and declare cl[12] len=8 so run_spiro() can work okay. */
	spiro[i].x = path7[i].x;
	spiro[i].y = path7[i].y;
	spiro[i].ty = path7[i].ty;
	nextknot[i] = knot7[i];
    } else if ( c==13 ) for (i = 0; i < 8; i++) {
	/* path13[] is open curve based on path11[] using '{','}' */
	spiro[i].x = path13[i].x;
	spiro[i].y = path13[i].y;
	spiro[i].ty = path13[i].ty;
	nextknot[i] = knot13[i];
    } else if ( c==14 ) for (i = 0; i < 4; i++) {
	/* path14[] is a very big version of closed curve path9[] */
	spiro[i].x = path14[i].x;
	spiro[i].y = path14[i].y;
	spiro[i].ty = path14[i].ty;
	nextknot[i] = knot4[i];
    } else if ( c==15 ) for (i = 0; i < 4; i++) {
	/* path15[] is a very small copy of path9[] closed curve. */
	spiro[i].x = path15[i].x;
	spiro[i].y = path15[i].y;
	spiro[i].ty = path15[i].ty;
	nextknot[i] = knot4[i];
    } else if ( c==16 ) for (i = 0; i < 4; i++) {
	/* path16[] uses path4[] as first try at quadratic output */
	spiro[i].x = path4[i].x;
	spiro[i].y = path4[i].y;
	spiro[i].ty = path4[i].ty;
	nextknot[i] = knot4[i];
    } else if ( c==17 ) for (i = 0; i < 4; i++) {
	/* path17[] is closed curve version of path4[] open curve */
	spiro[i].x = path4[i].x;
	spiro[i].y = path4[i].y;
	spiro[i].ty = path4[i].ty;
	nextknot[i] = knot4[i];
    } else if ( c==18 ) for (i = 0; i < 4; i++) {
	spiro[i].x = path2[i].x;
	spiro[i].y = path2[i].y;
	spiro[i].ty = path2[i].ty;
	nextknot[i] = knot2[i];
    } else if ( c==19 ) for (i = 0; i < 16; i++) {
	spiro[i].x = path0[i].x;
	spiro[i].y = path0[i].y;
	spiro[i].ty = path0[i].ty;
	nextknot[i] = knot0[i];
    } else if ( c==20 || c==22) for (i = 0; i < 9; i++) {
	/* path20[]=closed, path21[]=open, path22[]=closed_with_z */
	spiro[i].x = path7[i].x;
	spiro[i].y = path7[i].y;
	spiro[i].ty = path7[i].ty;
    } else if ( c==23 ) for (i = 0; i < 12; i++) {
	spiro[i].x = path5[i].x;
	spiro[i].y = path5[i].y;
	spiro[i].ty = path5[i].ty;
    } else if ( c==24 ) for (i = 0; i < 16; i++) {
	spiro[i].x = path0[i].x;
	spiro[i].y = path0[i].y;
	spiro[i].ty = path0[i].ty;
	nextknot[i] = knot0[i];
    }
}
int cl[] = {16, 6, 4, 6, 4, 12, 12, 8, 8, 4, 8, 8, 8, 8, 4, 4, 4, 4, 4, 16,
	    8, 8, 9, 12, 16}; /* len. */
int ck[] = {16, 6, 4, 6, 4, 12, 9, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 16,
	    4, 4, 4, 12, 16};
int co[] = {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1,
	    1, 0, 1, 0, 1}; /* close=1 */

#ifndef DO_CALL_TESTM
/* Provide bare-bones do-nothing functions for testing. This only */
/* printf values that would normally be handled by user programs. */
void test_moveto(bezctx *bc, double x, double y, int is_open) {
    printf("test_moveto(%g,%g)_%d\n",x,y,is_open);
}
void test_lineto(bezctx *bc, double x, double y) {
    printf("test_lineto(%g,%g)\n",x,y);
}
void test_quadto(bezctx *bc, double x1, double y1, double x2, double y2) {
    printf("test_quadto(%g,%g, %g,%g)\n",x1,y1,x2,y2);
}
void test_curveto(bezctx *bc, double x1, double y1, double x2, double y2,
		  double x3, double y3) {
    printf("test_curveto(%g,%g, %g,%g, %g,%g)\n",x1,y1,x2,y2,x3,y3);
}
void test_mark_knot(bezctx *bc, int knot_idx) {
    printf("test_mark_knot()_%d\n",knot_idx);
}

bezctx *new_bezctx_test(void) {
    bezctx *result = malloc(1*sizeof(bezctx));

    result->moveto = test_moveto;
    result->lineto = test_lineto;
    result->quadto = test_quadto;
    result->curveto = test_curveto;
    result->mark_knot = test_mark_knot;
    return result;
}

int test_curve(int c) {
    spiro_cp spiro[16];
    int nextknot[17];
    double d[6];
    spiro_seg *segs = NULL;
    bezctx *bc;
    rs_check_vals *rsp;
    int i,j,done;

    /* Load sample data so that we can see if library is callable */
    load_test_curve(spiro,nextknot,c);

    d[0] = 1.; d[1] = d[2] = 0.;
#if defined(DO_CALL_TEST20)
    /* check if spiro values are reversed correctly on input path */
    printf("---\ntesting spiroreverse() using data=path%d[].\n",c);
    if ( (spiroreverse(spiro,cl[c])) ) {
	fprintf(stderr,"error with spiroreverse() using data=path%d[].\n",c);
	return -1;
    }
    /* just do a visual check to verify types and points look ok. */
    /* NOTE: spiro[] is NOT replaced if unable to reverse values. */
    for (i=0; i < cl[c]; i++) {
	printf("  reversed %d: ty=%c, x=%g, y=%g\n", i,spiro[i].ty,spiro[i].x,spiro[i].y);
    }
#else
#if defined(DO_CALL_TEST14) || defined(DO_CALL_TEST15) || defined(DO_CALL_TEST16) || defined(DO_CALL_TEST17) || defined(DO_CALL_TEST18) || defined(DO_CALL_TEST19)
#if defined(DO_CALL_TEST16) || defined(DO_CALL_TEST17) || defined(DO_CALL_TEST18) || defined(DO_CALL_TEST19)
    /* run_spiro0 with various paths to test a simple arc output. */
    d[0] = -1.;
    printf("---\ntesting run_spiro0(q) using data=path%d[].\n",c);
    if ( (segs=run_spiro0(spiro,d,SPIRO_ARC_MAYBE,cl[c]))==0 ) {
	fprintf(stderr,"error with run_spiro0(q) using data=path%d[].\n",c);
	return -1;
    }
#else
    /* Do run_spiro0 instead (these tests are far from -0.5..+0.5 */
    d[0] = -1.;
    printf("---\ntesting run_spiro0() using data=path%d[].\n",c);
    if ( (segs=run_spiro0(spiro,d,0,cl[c]))==0 ) {
	fprintf(stderr,"error with run_spiro0() using data=path%d[].\n",c);
	return -1;
    }
#endif
#else
    /* Check if run_spiro works okay (try backwards compatiblity) */
    printf("---\ntesting run_spiro() using data=path%d[].\n",c);
    if ( (segs=run_spiro(spiro,cl[c]))==0 ) {
	fprintf(stderr,"error with run_spiro() using data=path%d[].\n",c);
	return -1;
    }
#endif

    /* Load pointer to verification data to ensure it works right */
    if ( c==0 )	     rsp = verify_rs0;
    else if ( c==1 ) rsp = verify_rs1;
    else if ( c==2 ) rsp = verify_rs2;
    /* else if ( c==3 ) rsp = NULL; expecting failure to converge */
    else if ( c==4 ) rsp = verify_rs4;
    else if ( c==5 ) rsp = verify_rs5;
    else if ( c==6 ) rsp = verify_rs5; /* same angles used for #6 */
    else if ( c==7 ) rsp = verify_rs7; /* stop curve with ah code */
    else if ( c==8 ) rsp = verify_rs8;
    else if ( c==9 ) rsp = verify_rs9;
    else if ( c==10 ) rsp = verify_rs10; /* start curve using ah. */
    else if ( c==11 ) rsp = verify_rs11;
    else if ( c==12 ) rsp = verify_rs7; /* test #12 uses path7[]. */
    else if ( c==13 ) rsp = verify_rs13; /* almost same as path11 */
    else if ( c==14 ) rsp = verify_rs14; /* very large path9 copy */
    else if ( c==15 ) rsp = verify_rs15; /* sub-atomic path9 copy */
    else if ( c==16 ) rsp = verify_rs4; /* path4 arc curve output */
    else if ( c==17 ) rsp = verify_rs4; /* path4 arc curve closed */
    else if ( c==18 ) rsp = verify_rs2; /* trying many iterations */
    else	      rsp = verify_rs0; /* long list, arc output. */

    /* Quick visual check shows X,Y knots match with each pathN[] */
    for (i=j=0; i < cl[c]-1; i++,j++) {
	if ( spiro[i].ty == 'h' ) {
	    --j;
	    printf("curve %d ctrl %d t=%c x=%f y=%f (handle)\n", \
	      c,j,segs[i].ty,spiro[i].x,spiro[i].y);
	}
	printf("curve %d line %d t=%c x=%f y=%f bend=%f ch=%f th=%f ",c,j, \
	  segs[i].ty,segs[i].x,segs[i].y,segs[i].bend_th,segs[i].seg_ch,segs[i].seg_th);
	/* Let computer verify that run_spiro() data is PASS/FAIL */
	/* Tests including ah data more complicated to verify xy, */
	/* therefore, skip testing xy for call_tests shown below. */
	if ( (segs[i].ty != spiro[i].ty) ||
#if defined(DO_CALL_TEST14) || defined(DO_CALL_TEST15) || defined(DO_CALL_TEST16) || defined(DO_CALL_TEST17) || defined(DO_CALL_TEST18) || defined(DO_CALL_TEST19)
#if defined(DO_CALL_TEST14) || defined(DO_CALL_TEST16) || defined(DO_CALL_TEST17) || defined(DO_CALL_TEST18) || defined(DO_CALL_TEST19)
	     (fabs((segs[i].x * d[0] + d[1]) - spiro[i].x) > 1e5) ||
	     (fabs((segs[i].y * d[0] + d[2]) - spiro[i].y) > 1e5) ||
	     (fabs(segs[i].seg_ch * d[0] - rsp[i].ch) > 1e5) ||
#else
	     (fabs((segs[i].x * d[0] + d[1]) - spiro[i].x) > 1e-47) ||
	     (fabs((segs[i].y * d[0] + d[2]) - spiro[i].y) > 1e-47) ||
	     (fabs(segs[i].seg_ch * d[0] - rsp[i].ch) > 1e-47) ||
#endif
#else
#if !defined(DO_CALL_TEST6) && !defined(DO_CALL_TEST7) && !defined(DO_CALL_TEST8) && !defined(DO_CALL_TEST10) && !defined(DO_CALL_TEST11) && !defined(DO_CALL_TEST12) && !defined(DO_CALL_TEST13)
	     (fabs(segs[i].x - spiro[i].x) > 1e-5) ||
	     (fabs(segs[i].y - spiro[i].y) > 1e-5) ||
#endif
	     (fabs(segs[i].seg_ch - rsp[i].ch) > 1e-5) ||
#endif
	     (fabs(segs[i].bend_th - rsp[i].b) > 1e-5) ||
	     (fabs(segs[i].seg_th - rsp[i].th) > 1e-5) ) {
	  printf("FAIL\n");
	  fprintf(stderr,"Error found with run_spiro() data. Results are not the same.\n");
	    fprintf(stderr,"expected line %d t=%c x=%f y=%f bend=%f ch=%f th=%f\n", j, \
	      spiro[i].ty,spiro[i].x,spiro[i].y,rsp[i].b,rsp[i].ch,rsp[i].th);
	    free(segs);
	    return -2;
	} else
	    printf("PASS\n");
    }
    printf("curve %d ",c);
    if ( spiro[i].ty == '}' || spiro[i].ty == 'z' )
	printf("stop %d t=%c x=%f y=%f\n",j,segs[i].ty,segs[i].x,segs[i].y);
    else
	printf("line %d t=%c x=%f y=%f bend=%f ch=%f th=%f\n", j,segs[i].ty, \
	  segs[i].x,segs[i].y,segs[i].bend_th,segs[i].seg_ch,segs[i].seg_th);

    /* Quick visual check shows X,Y knots match with each pathN[] */
    printf("---\ntesting spiro_to_bpath() using data from run_spiro(data=path%d[],len=%d).\n",c,cl[c]);
    bc = new_bezctx_test();
#if defined(DO_CALL_TEST14) || defined(DO_CALL_TEST15) || defined(DO_CALL_TEST16) || defined(DO_CALL_TEST17) || defined(DO_CALL_TEST18) || defined(DO_CALL_TEST19)
#if defined(DO_CALL_TEST16) || defined(DO_CALL_TEST17) || defined(DO_CALL_TEST18) || defined(DO_CALL_TEST19)
    spiro_to_bpath0(spiro,segs,d,SPIRO_ARC_MAYBE,cl[c],bc);
#else
    spiro_to_bpath0(spiro,segs,d,0,cl[c],bc);
#endif
#else
    spiro_to_bpath(segs,cl[c],bc);
#endif
    free(segs);
#endif

#if !defined(DO_CALL_TEST20)
#if defined(DO_CALL_TEST14) || defined(DO_CALL_TEST15) || defined(DO_CALL_TEST16) || defined(DO_CALL_TEST17) || defined(DO_CALL_TEST18) || defined(DO_CALL_TEST19)
#if defined(DO_CALL_TEST14) || defined(DO_CALL_TEST15) || defined(DO_CALL_TEST16) || defined(DO_CALL_TEST17)
    /* Now verify we also have simple arc output too */
    printf("---\ntesting SpiroCPsToBezier2() using data=path%d[].\n",c);
    if ( SpiroCPsToBezier2(spiro,cl[c],SPIRO_ARC_MAYBE,co[c],bc)!=1 ) {
	fprintf(stderr,"error with SpiroCPsToBezier2() using data=path%d[].\n",c);
	return -7;
    }
#endif
#if defined(DO_CALL_TEST18) || defined(DO_CALL_TEST19)
    printf("---\ntesting TaggedSpiroCPsToBezier2() using data=path%d[].\n",c);
    if ( TaggedSpiroCPsToBezier2(spiro,SPIRO_ARC_MAYBE,bc)!=1 ) {
	fprintf(stderr,"error with TaggedSpiroCPsToBezier2() using data=path%d[].\n",c);
	return -8;
    }
#endif
#else
#if !defined(DO_CALL_TEST4) && !defined(DO_CALL_TEST6) && !defined(DO_CALL_TEST7) && !defined(DO_CALL_TEST8) && !defined(DO_CALL_TEST9) && !defined(DO_CALL_TEST10) && !defined(DO_CALL_TEST11)
    /* Check if TaggedSpiroCPsToBezier0() works okay */
    printf("---\ntesting TaggedSpiroCPsToBezier0() using data=path%d[].\n",c);
    if ( TaggedSpiroCPsToBezier0(spiro,bc)!=1 ) {
	fprintf(stderr,"error with TaggedSpiroCPsToBezier0() using data=path%d[].\n",c);
	return -3;
    }
#endif

#if !defined(DO_CALL_TEST12)
    /* Check if SpiroCPsToBezier0() works okay */
    printf("---\ntesting SpiroCPsToBezier0() using data=path%d[].\n",c);
    if ( SpiroCPsToBezier0(spiro,cl[c],co[c],bc)!=1 ) {
	fprintf(stderr,"error with SpiroCPsToBezier0() using data=path%d[].\n",c);
	return -4;
    }
#endif

#if !defined(DO_CALL_TEST4) && !defined(DO_CALL_TEST6) && !defined(DO_CALL_TEST7) && !defined(DO_CALL_TEST8) && !defined(DO_CALL_TEST9) && !defined(DO_CALL_TEST10) && !defined(DO_CALL_TEST11)
    /* Check if TaggedSpiroCPsToBezier1() works okay */
    printf("---\ntesting TaggedSpiroCPsToBezier1() using data=path%d[].\n",c);
    TaggedSpiroCPsToBezier1(spiro,bc,&done);
    if ( done!=1 ) {
	fprintf(stderr,"error with TaggedSpiroCPsToBezier1() using data=path%d[].\n",c);
	return -5;
    }
#endif

#if !defined(DO_CALL_TEST12)
    /* Check if SpiroCPsToBezier1() works okay */
    printf("---\ntesting SpiroCPsToBezier1() using data=path%d[].\n",c);
    SpiroCPsToBezier1(spiro,cl[c],co[c],bc,&done);
    if ( done!=1 ) {
	fprintf(stderr,"error with SpiroCPsToBezier1() using data=path%d[].\n",c);
	return -6;
    }
#if defined(DO_CALL_TEST13)
    /* Check if SpiroCPsToBezier2() works okay */
    printf("---\ntesting SpiroCPsToBezier2() using data=path%d[].\n",c);
    if ( SpiroCPsToBezier2(spiro,cl[c],SPIRO_INCLUDE_LAST_KNOT,co[c],bc)!=1 ) {
	fprintf(stderr,"error with SpiroCPsToBezier2() using data=path%d[].\n",c);
	return -6;
    }
#endif
#else
    /* Check if SpiroCPsToBezier2() works okay */
    printf("---\ntesting SpiroCPsToBezier2() using data=path%d[].\n",c);
    if ( SpiroCPsToBezier2(spiro,cl[c],SPIRO_INCLUDE_LAST_KNOT,co[c],bc)!=1 ) {
	fprintf(stderr,"error with SpiroCPsToBezier2() using data=path%d[].\n",c);
	return -6;
    }
    /* Check if TaggedSpiroCPsToBezier2() works okay */
    printf("---\ntesting TaggedSpiroCPsToBezier2() using data=path%d[].\n",c);
    if ( TaggedSpiroCPsToBezier2(spiro,SPIRO_INCLUDE_LAST_KNOT,bc)!=1 ) {
	fprintf(stderr,"error with TaggedSpiroCPsToBezier2() using data=path%d[].\n",c);
	return -6;
    }
#endif
#endif

#else
    /* We already visually checked output for spiroreverse above. */
    /* Some reversed paths (above) will fail (like path20), so we */
    /* reverse the reversed spiro path so we can use current test */
    /* functions & values (so that this actually tests something) */
    bc = new_bezctx_test();
    if (c == 20 || c == 21) {
	/* Check if SpiroCPsToBezier2() works okay */
	printf("---\ntesting SpiroCPsToBezier2(reverse) using data=path%d[].\n",c);
	if ( SpiroCPsToBezier2(spiro,cl[c],SPIRO_REVERSE_SRC,co[c],bc)!=1 ) {
	    fprintf(stderr,"error with SpiroCPsToBezier2(reverse) using data=path%d[].\n",c);
	    return -9;
	}
    } else { /* c==22 || c==23 || c==24 */
	/* Check if TaggedSpiroCPsToBezier2() works okay */
	printf("---\ntesting TaggedSpiroCPsToBezier2(reverse) using data=path%d[].\n",c);
	if ( TaggedSpiroCPsToBezier2(spiro,SPIRO_REVERSE_SRC,bc)!=1 ) {
	    fprintf(stderr,"error with TaggedSpiroCPsToBezier2(reverse) using data=path%d[].\n",c);
	    return -10;
	}
	if (c == 24) {
	    /* reverse it again, ends in v, visually check end points */
	    printf("---\ntesting TaggedSpiroCPsToBezier2(reverse) using data=path%d[].\n",c);
	    if ( TaggedSpiroCPsToBezier2(spiro,SPIRO_REVERSE_SRC|SPIRO_INCLUDE_LAST_KNOT,bc)!=1 ) {
		fprintf(stderr,"error with TaggedSpiroCPsToBezier2(reverse) using data=path%d[].\n",c);
		return -10;
	    }
	}
    }
#endif

    free(bc);
    return 0;
}
#endif

#ifdef DO_CALL_TESTM
/************************************************/
/************************************************/
/* multi-threaded, multi-user, multi-curve test */
/* exercise libspiro with multiple curves given */
/* all at the same time and then check that all */
/* returned curves contain the correct values.  */
#ifdef HAVE_PTHREADS
typedef struct {
    spiro_cp *spiro;
    bezctx *bc;
    int ret;
} pthread_pcurve;

void *test_a_curve(void *pdata) {
    pthread_pcurve *data = (pthread_pcurve*)pdata;
    /*printf("start pthread %d\n",data->ret);*/
    data->ret = TaggedSpiroCPsToBezier2(data->spiro,0,data->bc);
    /*printf("done\n");*/
    return 0;
}
#endif

typedef struct {
    double x1,y1,x2,y2,x3,y3;
    char ty;
} my_curve_data;

typedef struct {
/* This is a superclass of bezctx (to keep track of each curve). */
    bezctx base;
    my_curve_data *my_curve;
    int len;
    int is_open;
    int c_id;
} test_bezctx;

#define S_RESULTS 50

void test_s_moveto(bezctx *z, double x, double y, int is_open) {
    test_bezctx *p = (test_bezctx*)z;
    int i;

    if ( (i=p->len) < S_RESULTS ) {
#ifdef VERBOSE
	printf("test_s_moveto(%g,%g)_%d [%d]%d\n",x,y,is_open,p->c_id,i);
#endif
	p->is_open = is_open;
	p->my_curve[i].x1 = x;
	p->my_curve[i].y1 = y;
	p->my_curve[p->len++].ty = 'm';
    }
}

void test_s_lineto(bezctx *z, double x, double y) {
    test_bezctx *p = (test_bezctx*)z;
    int i;

    if ( (i=p->len) < S_RESULTS ) {
#ifdef VERBOSE
	printf("test_s_lineto(%g,%g) [%d]%d\n",x,y,p->c_id,i);
#endif
	p->my_curve[i].x1 = x;
	p->my_curve[i].y1 = y;
	p->my_curve[p->len++].ty = 'l';
    }
}

void test_s_quadto(bezctx *z, double x1, double y1, double x2, double y2) {
    test_bezctx *p = (test_bezctx*)z;
    int i;

    if ( (i=p->len) < S_RESULTS ) {
#ifdef VERBOSE
	printf("test_s_quadto(%g,%g, %g,%g) [%d]%d\n",x1,y1,x2,y2,p->c_id,i);
#endif
	p->my_curve[i].x1 = x1;
	p->my_curve[i].y1 = y1;
	p->my_curve[i].x2 = x2;
	p->my_curve[i].y2 = y2;
	p->my_curve[p->len++].ty = 'q';
    }
}

void test_s_curveto(bezctx *z, double x1, double y1, double x2, double y2,
		  double x3, double y3) {
    test_bezctx *p = (test_bezctx*)z;
    int i;

    if ( (i=p->len) < S_RESULTS ) {
#ifdef VERBOSE
	printf("test_s_curveto(%g,%g, %g,%g, %g,%g) [%d]%d\n",x1,y1,x2,y2,x3,y3,p->c_id,i);
#endif
	p->my_curve[i].x1 = x1;
	p->my_curve[i].y1 = y1;
	p->my_curve[i].x2 = x2;
	p->my_curve[i].y2 = y2;
	p->my_curve[i].x3 = x3;
	p->my_curve[i].y3 = y3;
	p->my_curve[p->len++].ty = 'c';
    }
}

void test_s_mark_knot(bezctx *z, int knot_idx) {
    test_bezctx *p = (test_bezctx*)z;

    if ( p->len < S_RESULTS )
#ifdef VERBOSE
	printf("test_s_mark_knot()_%d [%d]%d\n",knot_idx,p->c_id,p->len);
#endif
	p->my_curve[p->len++].ty = 'k';
}

int test_multi_curves(void) {
    spiro_cp **spiro = NULL;
    spiro_cp *temp;
    int *pk, **nextknot = NULL;
    int *scl = NULL;
    test_bezctx **bc;
    int i, j, k, l, ret;
    double x, y;
    char ty;

    /* our simple curve test-check breaks-down if we go more than */
    /* 10x larger curves due to rounding errors on double values, */
    /* so, we either need a more complex curve test-check at end, */
    /* or we can cleverly increase in increments of "1/S_TESTS".  */
#define S_TESTS S_TESTP*5

#ifdef HAVE_PTHREADS
    pthread_attr_t tattr;
    pthread_t curve_test[S_TESTS];
    pthread_pcurve pdata[S_TESTS];

    printf("---\nMulti-thread testing of libspiro.\n");
#else
    printf("---\nSequential tests of libspiro.\n");
#endif

    ret = -1;	/* return error if out of memory */


    /* Expect lots of results, therefore create available memory. */
    /* This way, we won't be wasting time doing malloc because we */
    /* really want to shoot a whole bunch of pthreads all at once.*/
    if ( (bc=(test_bezctx**)calloc(S_TESTS,sizeof(test_bezctx*)))==NULL )
	goto test_multi_curves_exit;
    for (i=0; i < S_TESTS; i++) {
	if ( (bc[i]=(test_bezctx*)malloc(sizeof(test_bezctx)))==NULL )
	    goto test_multi_curves_exit;
	bc[i]->base.moveto = test_s_moveto;
	bc[i]->base.lineto = test_s_lineto;
	bc[i]->base.quadto = test_s_quadto;
	bc[i]->base.curveto = test_s_curveto;
	bc[i]->base.mark_knot = test_s_mark_knot;
	if ( (bc[i]->my_curve=(my_curve_data*)calloc(S_RESULTS,sizeof(my_curve_data)))==NULL )
	    goto test_multi_curves_exit;
	bc[i]->len = 0;		/* no curve yet, len=0 */
	bc[i]->is_open = 0;
	bc[i]->c_id = i;	/* identify each curve */
    }

    if ( (scl=(int*)malloc(S_TESTS*sizeof(int)))==NULL || \
	 (spiro=(spiro_cp**)calloc(S_TESTS,sizeof(spiro_cp*)))==NULL || \
	 (nextknot=(int**)calloc(S_TESTS,sizeof(int*)))==NULL )
	goto test_multi_curves_exit;
    for (i=0; i < S_TESTS; ) {
	/* NOTE: S_TESTS has to be multiple of 5 here. */
	/* because we test with path[0/1/2/5/6]tables, */
	/* ...and path[3] is used to test NOT success, */
	/* ...and path[4] is too fast @ 4 interations, */
	/* ...and path[7/8] ah complicates node check. */
	if ( (spiro[i]=malloc(cl[0]*sizeof(spiro_cp)))==NULL || \
	      (nextknot[i]=calloc(cl[0],sizeof(int)))==NULL )
	    goto test_multi_curves_exit;
	load_test_curve(spiro[i],nextknot[i],0);
	scl[i++]=cl[0];
	if ( (spiro[i]=malloc(cl[1]*sizeof(spiro_cp)))==NULL || \
	      (nextknot[i]=calloc(cl[1],sizeof(int)))==NULL )
	    goto test_multi_curves_exit;
	load_test_curve(spiro[i],nextknot[i],1);
	scl[i++]=cl[1];
	if ( (spiro[i]=malloc(cl[2]*sizeof(spiro_cp)))==NULL || \
	      (nextknot[i]=calloc(cl[2],sizeof(int)))==NULL )
	    goto test_multi_curves_exit;
	load_test_curve(spiro[i],nextknot[i],2);
	scl[i++]=cl[2];
	if ( (spiro[i]=malloc(cl[5]*sizeof(spiro_cp)))==NULL || \
	      (nextknot[i]=calloc(cl[5],sizeof(int)))==NULL )
	    goto test_multi_curves_exit;
	load_test_curve(spiro[i],nextknot[i],5);
	scl[i++]=cl[5];
	if ( (spiro[i]=malloc(cl[6]*sizeof(spiro_cp)))==NULL || \
	      (nextknot[i]=calloc(cl[6],sizeof(int)))==NULL )
	    goto test_multi_curves_exit;
	load_test_curve(spiro[i],nextknot[i],6);
	scl[i++]=cl[6];
    }

    /* Change to different sizes to make sure no duplicates */
    /* ...to verify we do not overwrite different user data */
    /* while running multiple threads all at the same time. */
    for (i=0; i < S_TESTS; i++) {
	temp = spiro[i];
	for (j=0; j < scl[i]; j++) {
	    if ( i&1 ) {
		temp[j].x = temp[j].x * (i/S_TESTS+1) + i;
		temp[j].y = temp[j].y * (i/S_TESTS+1) + i;
	    } else {
		/* Scaling bug fixed. Scale & shift at will */
		temp[j].x *= (i+1); temp[j].x += i*1000. - 5000.;
		temp[j].y *= (i+1); temp[j].y += i*3000. - 9000.;
	    }
	}
    }

    --ret;

#ifdef HAVE_PTHREADS
    /* Data and memory prepared before Pthreads.  Ready? Set? GO! */
    /* Test all curves, all at same time, wait for all to finish. */
    /* This test could fail if we had globally set variables that */
    /* could affect other functions, eg: static n=4 was moved out */
    /* into passed variable so that one user won't affect others. */
    /* GEGL is a good example of multiple users all at same time. */
    for (i=0; i < S_TESTS; i++) {
	pdata[i].spiro = spiro[i];
	pdata[i].bc = (bezctx*)(bc[i]);
	pdata[i].ret = i;
    }

    j=0;
    for (k=0; k < S_TESTS;) {
	/* Initialize and set thread joinable attribute */
	pthread_attr_init(&tattr);
	pthread_attr_setdetachstate(&tattr,PTHREAD_CREATE_JOINABLE);

	/* Some processors can't do too many pthreads at once so then */
	/* we need to run threads in batches until completing S_TESTS */
	for (i=k; i < S_TESTS-1; i++) {
	    /* all values passed are joined at "->" (should be okay). */
	    if ( pthread_create(&curve_test[i],&tattr,test_a_curve,(void *)&pdata[i]) ) {
		if ( i-k < 20 ) {
		    printf("bad pthread_create[%d]\n",i); /* not many */
		    j=-1;
		}
		break;
	    }
	}
	pthread_attr_destroy(&tattr);	/* Free thread attribute */
	if ( j!=-1 ) {
	    /* Test another curve while waiting for threads to finish */
	    pdata[i].ret = TaggedSpiroCPsToBezier2(pdata[i].spiro,0,pdata[i].bc);
	    printf("running simultaneous threads[%d..%d]\n",k,i);
	}
	l=i;
	while (--i >= k)
	    if ( pthread_join(curve_test[i],NULL) ) {
		fprintf(stderr,"bad pthread_join[%d]\n",i);
		j=-1;
	    }
	k=++l;
	if (j) goto test_multi_curves_exit;
    }

    for (i=0; i < S_TESTS; i++)
	if ( pdata[i].ret!=1 ) {
	    ret=ret-i;
	    fprintf(stderr,"error %d with TaggedSpiroCPsToBezier2() using data=%d.\n",ret,i);
	    goto test_multi_curves_exit;
	}
    /* All threads returned okay, Now, go check all data is good. */
#else
    /* No pthreads.h, test all curves sequentially, one at a time */
    /* Just do a math check and leave the pthread check for other */
    /* operating systems to verify libspiro has no static values. */
    for (i=0; i < S_TESTS; i++) {
	if ( TaggedSpiroCPsToBezier0(spiro[i],(bezctx*)(bc[i]))!=1 ) {
	    ret=ret-i;
	    fprintf(stderr,"error %d with TaggedSpiroCPsToBezier0() using data=%d.\n",ret,i);
	    goto test_multi_curves_exit;
	}
    }
#endif
    ret=ret-S_TESTS;

    /* Check ending x,y points versus input spiro knot locations. */
    for (i=0; i < S_TESTS; i++) {
	temp = spiro[i];
	pk = nextknot[i];
	k=0;
#ifdef VERBOSE
	printf("test[%d], input spiro[0..%d], output bc->my_curve[0..%d]\n", \
		   i, scl[i]-1, bc[i]->len-1);
#endif
	for (j=l=0; j < scl[i] && temp[j].ty!='z'; j++,l++) {
	    if (temp[j].ty=='h') {
#ifdef VERBOSE
		printf("  s[%d][ty=%c x=%g y=%g] is handle 'h' to anchor 'a'.\n", \
			l-1,temp[j].ty,temp[j].x,temp[j].y);
#endif
		--l;
	    } else {
		ty = bc[i]->my_curve[k].ty;
		x  = bc[i]->my_curve[k].x1;
		y  = bc[i]->my_curve[k].y1;
		if ( ty=='q' ) {
		    x = bc[i]->my_curve[k].x2;
		    y = bc[i]->my_curve[k].y2;
		}
		if ( ty=='c' ) {
		    x = bc[i]->my_curve[k].x3;
		    y = bc[i]->my_curve[k].y3;
		}
#ifdef VERBOSE
		printf("  s[%d][ty=%c x=%g y=%g], pk=%d mc[%d][x=%g y=%g]\n", \
			l,temp[j].ty,temp[j].x,temp[j].y,pk[j],k,x,y);
#endif
		if ( (fabs(temp[j].x - x) > 1e-5) || (fabs(temp[j].y - y) > 1e-5) ) {
		    /* close-enough for testing 1..10000 value ranges */
		    if ( j == scl[i]-2 && temp[j+1].ty=='z' )
			; /* Exception: skip test of this code point. */
			/* x and/or y are not valid for 'this' check, */
			/* If ending in 'z', then prior code point is */
			/* changed to a curve end point, which is '}' */
		    else {
			ret=ret-i;
			fprintf(stderr,"error %d with test_multi_curves() using data %d\n",ret,i);
			goto test_multi_curves_exit;
		    }
		}
		k += pk[l]+1;
	    }
	}
    }

#ifdef HAVE_PTHREADS
    printf("Multi-thread testing of libspiro passed.\n");
#else
    printf("Sequential tests of libspiro passed.\n");
#endif
    ret = 0;

test_multi_curves_exit:
    if ( nextknot!=NULL ) for (i=0; i < S_TESTS; i++) free(nextknot[i]);
    if ( spiro!=NULL ) for (i=0; i < S_TESTS; i++) free(spiro[i]);
    free(nextknot); free(spiro); free(scl);
    if ( bc!=NULL ) for (i=0; i < S_TESTS; i++) {
	free(bc[i]->my_curve); free(bc[i]);
    }
    free(bc);
    return ret;
}
#endif

int main(int argc, char **argv) {
    double st, en;
    int ret;
    st = get_time();

#ifdef DO_CALL_TEST0
    ret=test_curve(0);	/* this comes with unit-test. */
#endif
#ifdef DO_CALL_TEST1
    ret=test_curve(1);	/* do a test using "{"..."}". */
#endif
#ifdef DO_CALL_TEST2
    ret=test_curve(2);	/* this does many iterations. */
#endif
#ifdef DO_CALL_TEST3
    if ( (ret=test_curve(3)==0) ) /* This curve will not converge */
	ret = -1 /* error found! ret=error value */;
    else
	ret = 0; /* expected failure to converge */
#endif
#ifdef DO_CALL_TEST4
    ret=test_curve(4);	/* test a cyclic calculation. */
#endif
#ifdef DO_CALL_TEST5
    ret=test_curve(5);	/* verify curve data with []. */
#endif
#ifdef DO_CALL_TEST6
    ret=test_curve(6);	/* verify curve data with ah. */
#endif
#ifdef DO_CALL_TEST7
    ret=test_curve(7);	/* loop stops with ah curves. */
#endif
#ifdef DO_CALL_TEST8
    ret=test_curve(8);	/* this open curve ends in ah */
#endif
#ifdef DO_CALL_TEST9
    ret=test_curve(9);	/* path4[] as a closed curve. */
#endif
#ifdef DO_CALL_TEST10
    /* TODO: see why can start using c, o, but not [. */
    /* TODO: see why can start using c, o, but not a. */
    ret=test_curve(10);	/* start loop with ah curves. */
    ret = 0; /* ignore result for now until improved. */
#endif
#ifdef DO_CALL_TEST11
    /* TODO: see why can start using c, o, but not [. */
    /* TODO: see why can start using c, o, but not a. */
    ret=test_curve(11);	/* start open curve using ah. */
    ret = 0; /* ignore result for now until improved. */
#endif
#ifdef DO_CALL_TEST12
    ret=test_curve(12);	/* do path7[] with a z ending */
#endif
#ifdef DO_CALL_TEST13
    ret=test_curve(13);	/* start open curve using {h. */
#endif
#ifdef DO_CALL_TEST14
    ret=test_curve(14);	/* go very big! go very tiny! */
#endif
#ifdef DO_CALL_TEST15
    ret=test_curve(15);	/* go very big! go very tiny! */
#endif
#ifdef DO_CALL_TEST16
    ret=test_curve(16);	/* testing arc output path4[] */
#endif
#ifdef DO_CALL_TEST17
    ret=test_curve(17);	/* do arc closed curve outut. */
#endif
#ifdef DO_CALL_TEST18
    ret=test_curve(18);	/* do iterative as arc output */
#endif
#ifdef DO_CALL_TEST19
    ret=test_curve(19);	/* do lengthy output with arc */
#endif
#ifdef DO_CALL_TEST20
    /* test spiroreverse and verify path[] directions */
    if ( (ret=test_curve(20))==0 )
	if ( (ret=test_curve(21))==0 )
	    if ( (ret=test_curve(22))==0 )
		if ( (ret=test_curve(23))==0 )
		    ret=test_curve(24);
#endif
#ifdef DO_CALL_TESTM
    ret=test_multi_curves();
#endif

    en = get_time();
    printf("time %g\n", (en - st));
    return ret;
}
