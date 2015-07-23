/* Unit test for testing spiro.c internal routines
Copyright (C) 2007 Raph Levien   (original code was from spiro.c)
Copyright (C) 2013 Jose Da Silva (moved from spiro.c, plus edits)

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

#include "spiro-config.h"	/* for ./configure test settings like VERBOSE */
#ifdef DO_TIME_DAY
#include <sys/time.h>		/* for gettimeofday */
#else
#include <sys/timeb.h>		/* for old get_time */
#endif

#include "bezctx.c"
#include "spiro.c"

int n; /* = 4; */

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

int test_integ(void) {
    double ks[] = {1, 2, 3, 4};
    double xy[2];
    double xynom[2];
    double ch, th;
    int i, j;
    int nsubdiv;

    n = ORDER < 6 ? 4096 : 1024;
    integrate_spiro(ks, xynom, n);
    nsubdiv = ORDER < 12 ? 8 : 7;
    for (i = 0; i < nsubdiv; i++) {
	double st, en;
	double err;
	int n_iter = (1 << (20 - i));

	n = 1 << i;
	st = get_time();
	for (j = 0; j < n_iter; j++)
	    integrate_spiro(ks, xy, n);
	en = get_time();
	err = hypot(xy[0] - xynom[0], xy[1] - xynom[1]);
#ifdef VERBOSE
	printf("%d %d %g %g\n", ORDER, n, (en - st) / n_iter, err);
#endif
	ch = hypot(xy[0], xy[1]);
	th = atan2(xy[1], xy[0]);
#if 0
	printf("n = %d: integ(%g %g %g %g) = %g %g, ch = %g, th = %g\n", n,
	       ks[0], ks[1], ks[2], ks[3], xy[0], xy[1], ch, th);
	printf("%d: %g %g\n", n, xy[0] - xynom[0], xy[1] - xynom[1]);
#endif
    }
    return 0;
}

void print_seg(const double ks[4], double x0, double y0, double x1, double y1) {
    double bend = fabs(ks[0]) + fabs(.5 * ks[1]) + fabs(.125 * ks[2]) +
	fabs((1./48) * ks[3]);

    if (bend < 1e-8) {
#ifdef VERBOSE
	printf("%g %g lineto\n", x1, y1);
#endif
    } else {
	double seg_ch = hypot(x1 - x0, y1 - y0);
	double seg_th = atan2(y1 - y0, x1 - x0);
	double xy[2];
	double ch, th;
	double scale, rot;
	double th_even, th_odd;
	double ul, vl;
	double ur, vr;

	integrate_spiro(ks, xy, n);
	ch = hypot(xy[0], xy[1]);
	th = atan2(xy[1], xy[0]);
	scale = seg_ch / ch;
	rot = seg_th - th;
	if (bend < 1.) {
	    th_even = (1./384) * ks[3] + (1./8) * ks[1] + rot;
	    th_odd = (1./48) * ks[2] + .5 * ks[0];
	    ul = (scale * (1./3)) * cos(th_even - th_odd);
	    vl = (scale * (1./3)) * sin(th_even - th_odd);
	    ur = (scale * (1./3)) * cos(th_even + th_odd);
	    vr = (scale * (1./3)) * sin(th_even + th_odd);
#ifdef VERBOSE
	    printf("%g %g %g %g %g %g curveto\n",
		   x0 + ul, y0 + vl, x1 - ur, y1 - vr, x1, y1);
#endif
	} else {
	    /* subdivide */
	    double ksub[4];
	    double thsub;
	    double xysub[2];
	    double xmid, ymid;
	    double cth, sth;

	    ksub[0] = .5 * ks[0] - .125 * ks[1] + (1./64) * ks[2] - (1./768) * ks[3];
	    ksub[1] = .25 * ks[1] - (1./16) * ks[2] + (1./128) * ks[3];
	    ksub[2] = .125 * ks[2] - (1./32) * ks[3];
	    ksub[3] = (1./16) * ks[3];
	    thsub = rot - .25 * ks[0] + (1./32) * ks[1] - (1./384) * ks[2] + (1./6144) * ks[3];
	    cth = .5 * scale * cos(thsub);
	    sth = .5 * scale * sin(thsub);
	    integrate_spiro(ksub, xysub, n);
	    xmid = x0 + cth * xysub[0] - sth * xysub[1];
	    ymid = y0 + cth * xysub[1] + sth * xysub[0];
	    print_seg(ksub, x0, y0, xmid, ymid);
	    ksub[0] += .25 * ks[1] + (1./384) * ks[3];
	    ksub[1] += .125 * ks[2];
	    ksub[2] += (1./16) * ks[3];
	    print_seg(ksub, xmid, ymid, x1, y1);
	}
    }
}

void print_segs(const spiro_seg *segs, int nsegs) {
    int i;

    for (i = 0; i < nsegs; i++) {
	double x0 = segs[i].x;
	double y0 = segs[i].y;
	double x1 = segs[i + 1].x;
	double y1 = segs[i + 1].y;

	if (i == 0)
	    printf("%g %g moveto\n", x0, y0);
	printf("%% ks = [ %g %g %g %g ]\n",
	       segs[i].ks[0], segs[i].ks[1], segs[i].ks[2], segs[i].ks[3]);
	print_seg(segs[i].ks, x0, y0, x1, y1);
    }
    printf("stroke\n");
}

int test_curve(void) {
    spiro_cp path[] = {
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
	{229, 82, 'c'}
    };
    spiro_seg *segs = 0;
    int i;

    for (i = 0; i < 1000; i++) {
	free_spiro(segs);
	if ( (segs = setup_path(path, 15))==0 || solve_spiro(segs, 15)==0 ) {
	    printf("error in test_curve()\n");
	    free_spiro(segs);
	    return -1;
	  ;
	}
    }
    printf("100 800 translate 1 -1 scale 1 setlinewidth\n");
    print_segs(segs, 15);
    printf("showpage\n");
    free_spiro(segs);
    return 0;
}

int main(int argc, char **argv) {
    return test_curve();
}
