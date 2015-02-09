/* Test libspiro normal library calls
Copyright (C) 2013, Joe Da Silva

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
#include <sys/timeb.h>		/* for get_time */

#include "spiroentrypoints.h"	/* call spiro through here */
#include "bezctx.h"		/* bezctx structure */

#include "spiro-config.h"	/* for ./configure test settings like VERBOSE */
#if HAVE_PTHREADS
#include <pthread.h>		/* multi-thread check. Not part of libspiro */
#endif

static double get_time (void) {
    struct timeb tb;

    ftime(&tb);

    return tb.time + 1e-3 * tb.millitm;
}

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
	{81, 429, 'v'},
	{114, 368, 'v'},
	{201, 402, 'c'},
	{276, 369, 'c'},
	{218, 308, 'c'},
	{91, 211, 'c'},
	{124, 111, 'c'},
	{229, 82, 'c'},
	{0, 0, 'z'}
    };
    int knot0[] = {
	1, 1, 3, 3, 2, 3, 1, 1, 1, 3, 3, 2, 2, 2, 1, 1
    };
    spiro_cp path1[] = {
	{80, 738, '{'},
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
	{386, 72, 'o'},
	{444, 141, '}'}
    };
    int knot2[] = {
	2, 1, 9, 0
    };
    spiro_cp path3[] = { /* this will fail to converge */
	{233, 144, '{'}, /* will not pass (on purpose) */
	{341, 138, 'o'},
	{386, 72,  'o'},
	{443, 141, 'o'},
	{467, 231, 'o'},
	{377, 333, '}'}
    };
    int knot3[] = {
	2, 1, 9, 1, 1, 0
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
    } else for (i = 0; i < 6; i++) {
	spiro[i].x = path3[i].x;
	spiro[i].y = path3[i].y;
	spiro[i].ty = path3[i].ty;
	nextknot[i] = knot3[i];
    }
}
int cl[] = {16, 6, 4, 6};

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
    int i,done;

    /* Load sample data so that we can see if library is callable */
    load_test_curve(spiro,nextknot,c);

    /* Check if run_spiro works okay */
    printf("testing run_spiro() using data=path%d[].\n",c);
    if ( (segs=run_spiro(spiro,cl[c]))==0 ) {
	printf("error with run_spiro() using data=path%d[].\n",c);
	return -1;
    }

    /* Quick visual check shows X,Y knots match with each pathN[] */
    for (i=0; i < cl[c]; i++) {
	printf("curve %d, line %d, x=%f y=%f t=%c bend=%f ch=%f th=%f l=%f \n",c,i,segs[i].x,segs[i].y,segs[i].ty,segs[i].bend_th,segs[i].seg_ch,segs[i].seg_th,segs[i].l);
    }

    /* Quick visual check shows X,Y knots match with each pathN[] */
    printf("testing spiro_to_bpath() using data from run_spiro(data=path%d[],len=%d).\n",c,cl[c]);
    bc = new_bezctx_test();
    spiro_to_bpath(segs,cl[c],bc);

    free(segs);

    /* Check if TaggedSpiroCPsToBezier0() works okay */
    printf("---\ntesting TaggedSpiroCPsToBezier0() using data=path%d[].\n",c);
    if ( TaggedSpiroCPsToBezier0(spiro,bc)!=1 ) {
	printf("error with TaggedSpiroCPsToBezier0() using data=path%d[].\n",c);
	return -1;
    }

    /* Check if SpiroCPsToBezier0() works okay */
    printf("---\ntesting SpiroCPsToBezier0() using data=path%d[].\n",c);
    if ( SpiroCPsToBezier0(spiro,cl[c],(c==0 ? 1 : 0),bc)!=1 ) {
	printf("error with SpiroCPsToBezier0() using data=path%d[].\n",c);
	return -1;
    }

    /* Check if TaggedSpiroCPsToBezier1() works okay */
    printf("---\ntesting TaggedSpiroCPsToBezier1() using data=path%d[].\n",c);
    TaggedSpiroCPsToBezier1(spiro,bc,&done);
    if ( done!=1 ) {
	printf("error with TaggedSpiroCPsToBezier1() using data=path%d[].\n",c);
	return -1;
    }

    /* Check if SpiroCPsToBezier1() works okay */
    printf("---\ntesting SpiroCPsToBezier1() using data=path%d[].\n",c);
    SpiroCPsToBezier1(spiro,cl[c],(c==0 ? 1 : 0),bc,&done);
    if ( done!=1 ) {
	printf("error with SpiroCPsToBezier1() using data=path%d[].\n",c);
	return -1;
    }

    free(bc);
    return 0;
}

/************************************************/
/************************************************/
/* multi-threaded, multi-user, multi-curve test */
/* exercise libspiro with multiple curves given */
/* all at the same time and then check that all */
/* returned curves contain the correct values.  */
#if HAVE_PTHREADS
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
    pthread_exit(NULL);
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
    int *pk, **nextknot = NULL;
    int *scl = NULL;
    test_bezctx **bc;
    int i, j, k, ret;

#if HAVE_PTHREADS
    printf("---\nMulti-thread testing of libspiro.\n");
    /* pthread default limit is currently about 380 without mods. */
#define S_TESTS 300
#else
    printf("---\nSequential tests of libspiro.\n");
#define S_TESTS 3000
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
	/* NOTE: S_TESTS has to be multiple of 3 here. */
	/* ...because we test using path[0/1/2]tables. */
	if ( (spiro[i]=malloc(cl[0]*sizeof(spiro_cp)))==NULL || \
	      (nextknot[i]=calloc(cl[0]+1,sizeof(int)))==NULL )
	    goto test_multi_curves_exit;
	load_test_curve(spiro[i], nextknot[i], 0);
	scl[i++]=cl[0];
	if ( (spiro[i]=malloc(cl[1]*sizeof(spiro_cp)))==NULL || \
	      (nextknot[i]=calloc(cl[1],sizeof(int)))==NULL )
	    goto test_multi_curves_exit;
	load_test_curve(spiro[i], nextknot[i], 1);
	scl[i++]=cl[1];
	if ( (spiro[i]=malloc(cl[2]*sizeof(spiro_cp)))==NULL || \
	      (nextknot[i]=calloc(cl[2],sizeof(int)))==NULL )
	    goto test_multi_curves_exit;
	load_test_curve(spiro[i], nextknot[i], 2);
	scl[i++]=cl[2];
    }

    /* Change to different sizes to make sure no duplicates */
    /* ...to verify we do not overwrite different user data */
    /* while running multiple threads all at the same time. */
    for (i=0; i < S_TESTS; i++) {
	spiro_cp *temp;
	temp = spiro[i];
	for (j=0; j < scl[i]; j++) {
	    temp[j].x = temp[j].x * ((i+1.0)/100);
	    temp[j].y = temp[j].y * ((i+1.0)/100);
	}
    }

#if HAVE_PTHREADS
    /* Data and memory prepared before Pthreads.  Ready? Set? GO! */
    /* Test all curves, all at same time, wait for all to finish. */
    /* This test could fail if we had globally set variables that */
    /* could affect other functions, eg: static n=4 was moved out */
    /* into passed variable so that one user won't affect others. */
    /* GEGL is a good example of multiple users all at same time. */
    pthread_t curve_test[S_TESTS];
    pthread_pcurve pdata[S_TESTS];
    for (i=0; i < S_TESTS; i++) {
	pdata[i].spiro = spiro[i];
	pdata[i].bc = (bezctx*)(bc[i]);
	pdata[i].ret = i;
    }

    j=0;
    for (i=0; i < S_TESTS; i++)
	/* all values passed are joined at "->" (should be okay). */
	if ( pthread_create(&curve_test[i],NULL,test_a_curve,(void *)&pdata[i]) ) {
	    printf("bad pthread_create[%d]\n",i);
	    j=-1;
	    break;
	}
    while (--i >= 0)
	if ( pthread_join(curve_test[i],NULL) ) {
	    printf("bad pthread_join[%d]\n",i);
	    j=-1;
	}
    if (j) goto test_multi_curves_exit;

    for (i=0; i < S_TESTS; i++)
	if ( pdata[i].ret!=1 ) {
	    printf("error with TaggedSpiroCPsToBezier0() using data=%d.\n",i);
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
	    goto test_multi_curves_exit;
	}
    }
#endif

    /* Check ending x,y points versus input spiro knot locations. */
    for (i=0; i < S_TESTS; i++) {
	spiro_cp *temp;
	double x, y;
	char ty;
	temp = spiro[i];
	pk = nextknot[i];
	k=0;
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
	    printf("len=%d s[%d].ty=%c x=%g y=%g, len=%d pk=%d mc[%d] x=%g y=%g\n", \
		   scl[i],j,temp[j].ty,temp[j].x,temp[j].y, bc[i]->len,pk[j],k,x,y);
#endif
	    if ( (fabs(temp[j].x - x) > 1e-8) || (fabs(temp[j].y - y) > 1e-8) ) {
		printf("error with test_multi_curves() using data %d\n",i);
		goto test_multi_curves_exit;
	    }
	    k += pk[j]+1;
	}
    }

#if HAVE_PTHREADS
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

int main(int argc, char **argv) {
    double st, en;
    int ret = 0;
    st = get_time();

    if ( test_curve(0) || test_curve(1) || test_curve(2) || \
	 !test_curve(3) || /* This curve won't converge. */ \
    	 test_multi_curves() )
	ret = -1;

    en = get_time();
    printf("time %g\n", (en - st));
    return ret;
}
