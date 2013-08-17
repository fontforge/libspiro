/* Test libspiro normal library calls
Copyright (C) 2013 Jose Da Silva

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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>		/* for gettimeofday */

#include "spiroentrypoints.h"	/* call spiro through here */
#include "bezctx.h"		/* bezctx structure */

#include "spiro-config.h"	/* for ./configure test settings like VERBOSE */

static double get_time (void) {
  struct timeval tv;
  struct timezone tz;

  gettimeofday(&tv, &tz);

  return tv.tv_sec + 1e-6 * tv.tv_usec;
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
	1, 1, 3, 3, 2, 3, 1, 1, 1, 3, 3, 2, 3, 7, 1, 1
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
    int i;

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
    } else for (i = 0; i < 4; i++) {
	spiro[i].x = path2[i].x;
	spiro[i].y = path2[i].y;
	spiro[i].ty = path2[i].ty;
	nextknot[i] = knot2[i];
    }
}
int cl[] = {16, 6, 4};

/* Provide bare-bones do-nothing functions for testing. This only */
/* printf values that would normally be handled by user programs. */
void test_moveto(bezctx *z, double x, double y, int is_open) {
    printf("test_moveto(%g,%g)_%d\n",x,y,is_open);
}
void test_lineto(bezctx *z, double x, double y) {
    printf("test_lineto(%g,%g)\n",x,y);
}
void test_quadto(bezctx *z, double x1, double y1, double x2, double y2) {
    printf("test_quadto(%g,%g, %g,%g)\n",x1,y1,x2,y2);
}
void test_curveto(bezctx *z, double x1, double y1, double x2, double y2,
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
    int i;

    /* Load sample data so that we can see if library is callable */
    load_test_curve(spiro,nextknot,c);

    /* Check if run_spiro works okay */
    printf("testing run_spiro() using data=path%d[].\n",c);
    if ( (segs=run_spiro(spiro,cl[c]))==NULL ) {
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

    free(bc);
    free(segs);
    return 0;
}

int main(int argc, char **argv) {
    double st, en;
    int ret = 0;
    st = get_time();

    if ( test_curve(0) || test_curve(1) || test_curve(2) )
	ret = -1;

    en = get_time();
    printf("time %g\n", (en - st));
    return ret;
}
