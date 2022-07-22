/*
libspiro - A sharable library of Spiro formula and functions.

This file is a fork from ppedit for use by the libspiro project.
Please see Changelog or git history for description of changes.
=============================================================
ppedit - A pattern plate editor for Spiro splines.
Copyright (C) 2007 Raph Levien
libspiro - A sharable library of Spiro formula and functions.
Copyright (C) 2007... George Williams (libspiro fork)
Copyright (C) 2013... Joe Da Silva (improvements...)

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
#include "spiro-config.h"
#include <math.h>
#include <stdlib.h>
#ifdef VERBOSE
#include <stdio.h>
#endif

#include "spiroentrypoints.h"
#include "bezctx.h"
#include "spiro_intf.h"
#include "bezctx_intf.h"

LS_DLL_EXPORT ls_bezctx *new_ls_bezctx(int max, int ncq) {
    ls_bezctx *r;

    if ( (ncq&SPIRO_INTERNAL_BEZCTX)==0 || max<1 || \
	 (r=(ls_bezctx *)calloc(1,sizeof(ls_bezctx)))==NULL ) {
#ifdef VERBOSE
	printf("error: new_ls_bezctx() memory\n");
#endif
	return NULL;
    }

    if ( (r->cd=(curve_data *)malloc(max*sizeof(curve_data)))==NULL ) {
#ifdef VERBOSE
	printf("error: new_ls_bezctx() memory\n");
#endif
	free(r);
    } else {
	r->l = 0;
	r->max = max;
#ifdef VERBOSE
	printf("new_ls_bezctx() created new space\n");
#endif
    }
    return r;
}

LS_DLL_EXPORT void free_ls_bezctx(ls_bezctx *bd) {
    free(bd->cd);
    free(bd);
}

static int prep_row_bc(ls_bezctx *bd) {
    double x,y;
    int i;
    char t;

    i = bd->l;
    if ( i < 0 ) return 0;
    if ( i >= bd->max || bd->max < 1 ) {
	bd->l = -1; /* error: too big! not enough array! */
	return 0;
    }
    if ( i ) {
	t = bd->cd[i - 1].ty;
	switch ( t ) {
	    case 'k':
		x = bd->cd[i - 1].x0; y = bd->cd[i - 1].y0; break;
	    case 'l':
	    case 'm':
		x = bd->cd[i - 1].x1; y = bd->cd[i - 1].y1; break;
	    case 'q':
		x = bd->cd[i - 1].x2; y = bd->cd[i - 1].y2; break;
	    case 'c':
		x = bd->cd[i - 1].x3; y = bd->cd[i - 1].y3; break;
	    default:
		x = y = 0.0;
	}
	bd->cd[i].x0 = x; bd->cd[i].y0 = y;
    } else
	bd->cd[0].x0 = bd->cd[0].y0 = 0.0;
    return 1;
}

LS_DLL_LOCAL void bezctx_moveto(bezctx *bc, double x, double y, int is_open, int si) {
#ifdef VERBOSE
    printf("moveto(%g,%g)_%d\n",x,y,is_open);
#endif
    if ( si ) {
	ls_bezctx *bd = (ls_bezctx *)(bc);
	if ( prep_row_bc(bd) ) {
	    int i = bd->l;
	    bd->cd[i].x1 = x; bd->cd[i].y1 = y;
	    bd->cd[i].x2 = bd->cd[i].y2 = bd->cd[i].x3 = bd->cd[i].y3 = 0.0;
	    bd->cd[i].ty = 'm';
	    i++;
	    bd->l = i;
	    bd->is_open = is_open;
	}
    } else
	bc->moveto(bc, x, y, is_open);
}

LS_DLL_LOCAL void bezctx_lineto(bezctx *bc, double x, double y, int si) {
#ifdef VERBOSE
    printf("lineto(%g,%g)\n",x,y);
#endif
    if ( si ) {
	ls_bezctx *bd = (ls_bezctx *)(bc);
	if ( prep_row_bc(bd) ) {
	    int i = bd->l;
	    bd->cd[i].x1 = x; bd->cd[i].y1 = y;
	    bd->cd[i].x2 = bd->cd[i].y2 = bd->cd[i].x3 = bd->cd[i].y3 = 0.0;
	    bd->cd[i].ty = 'l';
	    i++;
	    bd->l = i;
	}
    } else
	bc->lineto(bc, x, y);
}

LS_DLL_LOCAL void bezctx_quadto(bezctx *bc, double x1, double y1, double x2, double y2, int si) {
#ifdef VERBOSE
    printf("quadto(%g,%g, %g,%g)\n",x1,y1,x2,y2);
#endif
    if ( si ) {
	ls_bezctx *bd = (ls_bezctx *)(bc);
	if ( prep_row_bc(bd) ) {
	    int i = bd->l;
	    bd->cd[i].x1 = x1; bd->cd[i].y1 = y1;
	    bd->cd[i].x2 = x2; bd->cd[i].y2 = y2;
	    bd->cd[i].x3 = bd->cd[i].y3 = 0.0;
	    bd->cd[i].ty = 'q';
	    i++;
	    bd->l = i;
	}
    } else
	bc->quadto(bc, x1, y1, x2, y2);
}

LS_DLL_LOCAL void bezctx_curveto(bezctx *bc, double x1, double y1, double x2, double y2, \
				 double x3, double y3, int si) {
#ifdef VERBOSE
    printf("curveto(%g,%g, %g,%g, %g,%g)\n",x1,y1,x2,y2,x3,y3);
#endif
    if ( si ) {
	ls_bezctx *bd = (ls_bezctx *)(bc);
	if ( prep_row_bc(bd) ) {
	    int i = bd->l;
	    bd->cd[i].x1 = x1; bd->cd[i].y1 = y1;
	    bd->cd[i].x2 = x2; bd->cd[i].y2 = y2;
	    bd->cd[i].x3 = x3; bd->cd[i].y3 = y3;
	    bd->cd[i].ty = 'c';
	    i++;
	    bd->l = i;
	}
    } else
	bc->curveto(bc, x1, y1, x2, y2, x3, y3);
}

LS_DLL_LOCAL void bezctx_mark_knot(bezctx *bc, int knot_idx, int si) {
#ifdef VERBOSE
    printf("mark_knot()_%d\n",knot_idx);
#endif
    if ( si ) {
	ls_bezctx *bd = (ls_bezctx *)(bc);
	if ( prep_row_bc(bd) ) {
	    int i = bd->l;
	    bd->cd[i].x1 = bd->cd[i].y1 = bd->cd[i].x2 = bd->cd[i].y2 = \
	    bd->cd[i].x3 = bd->cd[i].y3 = 0.0;
	    bd->cd[i].ty = 'k';
	    i++;
	    bd->l = i;
	}
    } else if (bc->mark_knot)
	bc->mark_knot(bc, knot_idx);
}
