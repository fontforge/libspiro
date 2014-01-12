/*
ppedit - A pattern plate editor for Spiro splines.
Copyright (C) 2007 Raph Levien

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
#ifdef VERBOSE
#include <stdio.h>
#endif

#include "bezctx.h"

void bezctx_moveto(bezctx *bc, double x, double y, int is_open)
{
#ifdef VERBOSE
    printf("moveto(%g,%g)_%d\n",x,y,is_open);
#endif
    bc->moveto(bc, x, y, is_open);
}

void bezctx_lineto(bezctx *bc, double x, double y)
{
#ifdef VERBOSE
    printf("lineto(%g,%g)\n",x,y);
#endif
    bc->lineto(bc, x, y);
}

void bezctx_quadto(bezctx *bc, double x1, double y1, double x2, double y2)
{
#ifdef VERBOSE
    printf("quadto(%g,%g, %g,%g)\n",x1,y1,x2,y2);
#endif
    bc->quadto(bc, x1, y1, x2, y2);
}

void bezctx_curveto(bezctx *bc, double x1, double y1, double x2, double y2,
		    double x3, double y3)
{
#ifdef VERBOSE
    printf("curveto(%g,%g, %g,%g, %g,%g)\n",x1,y1,x2,y2,x3,y3);
#endif
    bc->curveto(bc, x1, y1, x2, y2, x3, y3);
}

void bezctx_mark_knot(bezctx *bc, int knot_idx)
{
#ifdef VERBOSE
    printf("mark_knot()_%d\n",knot_idx);
#endif
    if (bc->mark_knot)
	bc->mark_knot(bc, knot_idx);
}
