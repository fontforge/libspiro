/* Test libspiro normal library calls
Copyright (C) 2013,2014,2015, Joe Da Silva

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
/* Expected run_spiro() test data results for path{0/1/2/4/5}[]. */
typedef struct {
    double b, ch, th;
} rs_check_vals;

rs_check_vals verify_rs0[] = {		/* iteration5 */
    {0.000000, 65.741920, 2.027641},	/* v, 34,117 */
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
    {-2.528266, 243.238566, -2.797739}	/* c,229, 82 */
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

rs_check_vals verify_rs5[] = {		/* iteration4 */
    {0.000000, 141.421356, 0.785398},	/* {,  0,   0 */
    {0.000000, 141.421356, 0.785398},	/* c,100, 100 */
    {-0.785398, 100.000000, 0.000000},	/* [,200, 200 */
    {-0.463648, 111.803399, -0.463648},	/* ],300, 200 */
    {-2.214297, 111.803399, -2.677945},	/* c,400, 150 */
    {-0.463648, 100.000000, 3.141593},	/* [,300, 100 */
    {0.785398, 70.710678, -2.356194},	/* ],200, 100 */
    {0.000000, 70.710678, -2.356194},	/* c,150,  50 */
    {0.000000, 141.421356, -2.356194},	/* [,100,   0 */
    {0.321751, 111.803399, -2.034444},	/* ],  0,-100 */
    {-0.076772, 58.309519, -2.111216}	/* c,-50,-200 */
};					/* },-80,-250 */
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
	{   0, -100, SPIRO_G4},
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
	{-80,-250, '}'}
    };
    int knot5[] = {
	1, 4, 1, 3, 3, 1, 4, 2, 1, 2, 1, 0
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
    } else for (i = 0; i < 12; i++) {
	spiro[i].x = path5[i].x;
	spiro[i].y = path5[i].y;
	spiro[i].ty = path5[i].ty;
	nextknot[i] = knot5[i];
    }
}
int cl[] = {16, 6, 4, 6, 4, 12};

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
    spiro_seg *segs = NULL;
    bezctx *bc;
    rs_check_vals *rsp;
    int i,done;

    /* Load sample data so that we can see if library is callable */
    load_test_curve(spiro,nextknot,c);

    /* Check if run_spiro works okay */
    printf("---\ntesting run_spiro() using data=path%d[].\n",c);
    if ( (segs=run_spiro(spiro,cl[c]))==0 ) {
	printf("error with run_spiro() using data=path%d[].\n",c);
	return -1;
    }

    /* Load pointer to verification data to ensure it works right */
    if ( c==0 )	     rsp = verify_rs0;
    else if ( c==1 ) rsp = verify_rs1;
    else if ( c==2 ) rsp = verify_rs2;
    /* else if ( c==3 ) rsp = NULL; expecting failure to converge */
    else if ( c==4 ) rsp = verify_rs4;
    else	     rsp = verify_rs5;

    /* Quick visual check shows X,Y knots match with each pathN[] */
    for (i=0; i < cl[c]-1; i++) {
	printf("curve %d, line %d x=%f y=%f t=%c bend=%f ch=%f th=%f ",c,i,segs[i].x,segs[i].y,segs[i].ty,segs[i].bend_th,segs[i].seg_ch,segs[i].seg_th);
	/* Let computer verify that run_spiro() data is PASS/FAIL */
	if ( (fabs(segs[i].x - spiro[i].x) > 1e-5) ||
	     (fabs(segs[i].y - spiro[i].y) > 1e-5) ||
	     (segs[i].ty != spiro[i].ty) ||
	     (fabs(segs[i].bend_th - rsp[i].b) > 1e-5) ||
	     (fabs(segs[i].seg_ch - rsp[i].ch) > 1e-5) ||
	     (fabs(segs[i].seg_th - rsp[i].th) > 1e-5) ) {
	    printf("FAIL\nerror found with run_spiro() data. Results are not the same.\n");
	    printf("expected line %d x=%f y=%f t=%c bend=%f ch=%f th=%f \n",i,spiro[i].x,spiro[i].y,spiro[i].ty,rsp[i].b,rsp[i].ch,rsp[i].th);
	    free(segs);
	    return -2;
	} else
	    printf("PASS\n");
    }
    printf("curve %d, line %d x=%f y=%f t=%c\n",c,i,segs[i].x,segs[i].y,segs[i].ty);

    /* Quick visual check shows X,Y knots match with each pathN[] */
    printf("---\ntesting spiro_to_bpath() using data from run_spiro(data=path%d[],len=%d).\n",c,cl[c]);
    bc = new_bezctx_test();
    spiro_to_bpath(segs,cl[c],bc);

    free(segs);

#ifndef DO_CALL_TEST4
    /* Check if TaggedSpiroCPsToBezier0() works okay */
    printf("---\ntesting TaggedSpiroCPsToBezier0() using data=path%d[].\n",c);
    if ( TaggedSpiroCPsToBezier0(spiro,bc)!=1 ) {
	printf("error with TaggedSpiroCPsToBezier0() using data=path%d[].\n",c);
	return -3;
    }
#endif

    /* Check if SpiroCPsToBezier0() works okay */
    printf("---\ntesting SpiroCPsToBezier0() using data=path%d[].\n",c);
    if ( SpiroCPsToBezier0(spiro,cl[c],(c==0 ? 1 : 0),bc)!=1 ) {
	printf("error with SpiroCPsToBezier0() using data=path%d[].\n",c);
	return -4;
    }

#ifndef DO_CALL_TEST4
    /* Check if TaggedSpiroCPsToBezier1() works okay */
    printf("---\ntesting TaggedSpiroCPsToBezier1() using data=path%d[].\n",c);
    TaggedSpiroCPsToBezier1(spiro,bc,&done);
    if ( done!=1 ) {
	printf("error with TaggedSpiroCPsToBezier1() using data=path%d[].\n",c);
	return -5;
    }
#endif

    /* Check if SpiroCPsToBezier1() works okay */
    printf("---\ntesting SpiroCPsToBezier1() using data=path%d[].\n",c);
    SpiroCPsToBezier1(spiro,cl[c],(c==0 ? 1 : 0),bc,&done);
    if ( done!=1 ) {
	printf("error with SpiroCPsToBezier1() using data=path%d[].\n",c);
	return -6;
    }

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
    data->ret = TaggedSpiroCPsToBezier0(data->spiro,data->bc);
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
#define S_TESTS S_TESTP*4

#ifdef HAVE_PTHREADS
    pthread_attr_t tattr;
    pthread_t curve_test[S_TESTS];
    pthread_pcurve pdata[S_TESTS];

    printf("---\nMulti-thread testing of libspiro.\n");
    /* pthread default limit is currently about 380 without mods. */
    /* new processors will allow more, older processors use less. */
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
	/* NOTE: S_TESTS has to be multiple of 4 here. */
	/* because we test using path[0/1/2/5]tables,  */
	/* ...and path[3] is used to test NOT success. */
	/* ...and path[4] is too fast @ 4 interations. */
	if ( (spiro[i]=malloc(cl[0]*sizeof(spiro_cp)))==NULL || \
	      (nextknot[i]=calloc(cl[0]+1,sizeof(int)))==NULL )
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
    }

    /* Change to different sizes to make sure no duplicates */
    /* ...to verify we do not overwrite different user data */
    /* while running multiple threads all at the same time. */
    for (i=0; i < S_TESTS; i++) {
	temp = spiro[i];
	for (j=0; j < scl[i]; j++) {
	    temp[j].x = temp[j].x * (i/S_TESTS+1) + i;
	    temp[j].y = temp[j].y * (i/S_TESTS+1) + i;
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
	    pdata[i].ret = TaggedSpiroCPsToBezier0(pdata[i].spiro,pdata[i].bc);
	    printf("running simultaneous threads[%d..%d]\n",k,i);
	}
	l=i;
	while (--i >= k)
	    if ( pthread_join(curve_test[i],NULL) ) {
		printf("bad pthread_join[%d]\n",i);
		j=-1;
	    }
	k=++l;
	if (j) goto test_multi_curves_exit;
    }

    for (i=0; i < S_TESTS; i++)
	if ( pdata[i].ret!=1 ) {
	    printf("error with TaggedSpiroCPsToBezier0() using data=%d.\n",i);
	    ret=ret-i;
	    goto test_multi_curves_exit;
	}
    /* All threads returned okay, Now, go check all data is good. */
#else
    /* No pthreads.h, test all curves sequentially, one at a time */
    /* Just do a math check and leave the pthread check for other */
    /* operating systems to verify libspiro has no static values. */
    for (i=0; i < S_TESTS; i++) {
	if ( TaggedSpiroCPsToBezier0(spiro[i],(bezctx*)(bc[i]))!=1 ) {
	    printf("error with TaggedSpiroCPsToBezier0() using data=%d.\n",i);
	    ret=ret-i;
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
	for (j=0; j < scl[i] && temp[j].ty!='z'; j++) {
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
		   j,temp[j].ty,temp[j].x,temp[j].y,pk[j],k,x,y);
#endif
	    if ( (fabs(temp[j].x - x) > 1e-8) || (fabs(temp[j].y - y) > 1e-8) ) {
		/* close-enough for testing 10x range of doubles. */
		printf("error with test_multi_curves() using data %d\n",i);
		ret=ret-i;
		goto test_multi_curves_exit;
	    }
	    k += pk[j]+1;
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
#ifdef DO_CALL_TESTM
    ret=test_multi_curves();
#endif

    en = get_time();
    printf("time %g\n", (en - st));
    return ret;
}
