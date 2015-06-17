# The Bézier context

```c
    struct _bezctx {
    void (*moveto)(bezctx *bc, double x, double y, int is_open);
    void (*lineto)(bezctx *bc, double x, double y);
    void (*quadto)(bezctx *bc, double x1, double y1, double x2, double y2);
    void (*curveto)(bezctx *bc, double x1, double y1, double x2, double y2,
                    double x3, double y3);
    void (*mark_knot)(bezctx *bc, int knot_idx);
    };
```

You must create a super-class of this abstract type that handles the creation of your particular representation of bézier splines.

I provide two examples here, one is the type Raph created for doing output to encapsulated postscript files, and the other is the type fontforge uses internally for converting from spiros into its native spline format.

## PostScript

First a header file. This declares a routine which will create a PostScript bézier context – this routine takes a file argument, splines will be written to the file as we gather more information.

```c
/* bezctx_ps.h */
const char *ps_prolog;		/* A string to be written to the file before conversion */
const char *ps_postlog;		/* A string to be written to the file after conversion */

bezctx *new_bezctx_ps(FILE *f);

void
bezctx_ps_close(bezctx *bc);
```

Then the file itself.

```c
/* bezctx_ps.c */
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
#include <stdio.h>

#include "zmisc.h"
#include "bezctx.h"
#include "bezctx_ps.h"

typedef struct {
    bezctx base;	/* This is a superclass of bezctx, and this is the entry for the base */
    int is_open;
    double x, y;
    FILE *f;
} bezctx_ps;

const char *ps_prolog = "%!PS\n"
"/m { moveto } bind def\n"
"/l { lineto } bind def\n"
"/c { curveto } bind def\n"
"/z { closepath } bind def\n"
"1 -1 scale\n"
"0 -792 translate\n";

const char *ps_postlog = "stroke\n"
"showpage\n";

/* This routine starts a new contour */
static void
bezctx_ps_moveto(bezctx *z, double x, double y, int is_open) {
    bezctx_ps *bc = (bezctx_ps *)z;

    if (!bc->is_open) fprintf(bc->f, "z\n");
    fprintf(bc->f, "%g %g m\n", x, y);
    bc->is_open = is_open;
    bc->x = x;
    bc->y = y;
}

/* This routine creates a linear spline from the previous point specified to this one */
void
bezctx_ps_lineto(bezctx *z, double x, double y) {
    bezctx_ps *bc = (bezctx_ps *)z;

    fprintf(bc->f, "%g %g l\n", x, y);
    bc->x = x;
    bc->y = y;
}

/* This could create a quadratic spline, except PostScript only is prepared to deal with */
/*  cubics, so convert the quadratic into the equivalent cubic */
void
bezctx_ps_quadto(bezctx *z, double xm, double ym, double x3, double y3)
{
    bezctx_ps *bc = (bezctx_ps *)z;
    double x0, y0;
    double x1, y1;
    double x2, y2;

    x0 = bc->x;
    y0 = bc->y;
    x1 = xm + (1./3) * (x0 - xm);
    y1 = ym + (1./3) * (y0 - ym);
    x2 = xm + (1./3) * (x3 - xm);
    y2 = ym + (1./3) * (y3 - ym);
    fprintf(bc->f, "%g %g %g %g %g %g c\n", x1, y1, x2, y2, x3, y3);
    bc->x = x3;
    bc->y = y3;
}

/* And this creates a cubic */
void
bezctx_ps_curveto(bezctx *z, double x1, double y1, double x2, double y2,
		  double x3, double y3)
{
    bezctx_ps *bc = (bezctx_ps *)z;

    fprintf(bc->f, "%g %g %g %g %g %g c\n", x1, y1, x2, y2, x3, y3);
    bc->x = x3;
    bc->y = y3;
}

/* Allocates and initializes a new PostScript bézier context */
bezctx *
new_bezctx_ps(FILE *f) {
    bezctx_ps *result = znew(bezctx_ps, 1);

    result->base.moveto = bezctx_ps_moveto;
    result->base.lineto = bezctx_ps_lineto;
    result->base.quadto = bezctx_ps_quadto;
    result->base.curveto = bezctx_ps_curveto;
    result->base.mark_knot = NULL;
    result->is_open = 1;
    result->f = f;
    return &amp;result->base;
}

/* Finishes an old PostScript bézier context */
void
bezctx_ps_close(bezctx *z)
{
    bezctx_ps *bc = (bezctx_ps *)z;

    if (!bc->is_open) fprintf(bc->f, "z\n");
    zfree(bc);
}
```

## FontForge

First the header file. This declares a routine which will create a fontforge bézier context, and another which will finish off (and free) the context. This last routine returns a collection of splines, a splineset, fontforge's internal bézier contour type.

```c
/* bezctx_ff.h */
#include "spiroentrypoints.h"
#include "bezctx.h"

bezctx *new_bezctx_ff(void);

struct splineset;

struct splineset *bezctx_ff_close(bezctx *bc);
```

Then the file itself.

```c
/* bezctx_ff.c */
#include "bezctx_ff.h"
#include "pfaeditui.h"
#ifdef HAVE_IEEEFP_H
# include <ieeefp.h>		/* Solaris defines isnan in ieeefp rather than math.h */
#else
# include <math.h>
#endif

typedef struct {
    bezctx base;			/* This is a superclass of bezctx, and this is the entry for the base */
    int is_open;
    int gotnans;			/* Sometimes spiro fails to converge and we get NaNs. Complain the first */
					/* time this happens, but not thereafter */
    SplineSet *ss;			/* The fontforge contour which we build up as we go along */
} bezctx_ff;

static void
nancheck(bezctx_ff *bc) {

    if ( !bc->gotnans ) {		/* Called when we get passed a NaN. Complain the first time that happens */
	LogError("Spiros did not converge" );
	bc->gotnans = true;
    }
}

/* This routine starts a new contour */
/* So we allocate a new SplineSet, and then add the first point to it */
static void
bezctx_ff_moveto(bezctx *z, double x, double y, int is_open) {
    bezctx_ff *bc = (bezctx_ff *)z;

    if ( !finite(x) || !finite(y)) {	/* Protection against NaNs */
	nancheck(bc);
	x = y = 0;
    }
    if (!bc->is_open) {
	SplineSet *ss = chunkalloc(sizeof(SplineSet));
	ss->next = bc->ss;
	bc->ss = ss;
    }
    bc->ss->first = bc->ss->last = SplinePointCreate(x,y);
    bc->is_open = is_open;
}

/* This routine creates a linear spline from the previous point specified to this one */
static void
bezctx_ff_lineto(bezctx *z, double x, double y) {
    bezctx_ff *bc = (bezctx_ff *)z;
    SplinePoint *sp;

    if ( !finite(x) || !finite(y)) {
	nancheck(bc);
	x = y = 0;
    }
    sp = SplinePointCreate(x,y);
    SplineMake3(bc->ss->last,sp);
    bc->ss->last = sp;
}

/* This could create a quadratic spline, except fontforge only is prepared to deal with */
/*  cubics, so convert the quadratic into the equivalent cubic */
static void
bezctx_ff_quadto(bezctx *z, double xm, double ym, double x3, double y3)
{
    bezctx_ff *bc = (bezctx_ff *)z;
    double x0, y0;
    double x1, y1;
    double x2, y2;
    SplinePoint *sp;

    if ( !finite(xm) || !finite(ym) || !finite(x3) || !finite(y3)) {
	nancheck(bc);
	xm = ym = x3 = y3 = 0;
    }
    sp = SplinePointCreate(x3,y3);
    x0 = bc->ss->last->me.x;
    y0 = bc->ss->last->me.y;
    x1 = xm + (1./3) * (x0 - xm);
    y1 = ym + (1./3) * (y0 - ym);
    x2 = xm + (1./3) * (x3 - xm);
    y2 = ym + (1./3) * (y3 - ym);
    bc->ss->last->nextcp.x = x1;
    bc->ss->last->nextcp.y = y1;
    bc->ss->last->nonextcp = false;
    sp->prevcp.x = x2;
    sp->prevcp.y = y2;
    sp->noprevcp = false;
    SplineMake3(bc->ss->last,sp);
    bc->ss->last = sp;
}

/* And this creates a cubic */
static void
bezctx_ff_curveto(bezctx *z, double x1, double y1, double x2, double y2,
		  double x3, double y3)
{
    bezctx_ff *bc = (bezctx_ff *)z;
    SplinePoint *sp;

    if ( !finite(x1) || !finite(y1) || !finite(x2) || !finite(y2) || !finite(x3) || !finite(y3)) {
	nancheck(bc);
	x1 = y1 = x2 = y2 = x3 = y3 = 0;
    }
    sp = SplinePointCreate(x3,y3);
    bc->ss->last->nextcp.x = x1;
    bc->ss->last->nextcp.y = y1;
    bc->ss->last->nonextcp = false;
    sp->prevcp.x = x2;
    sp->prevcp.y = y2;
    sp->noprevcp = false;
    SplineMake3(bc->ss->last,sp);
    bc->ss->last = sp;
}

/* Allocates and initializes a new fontforge bézier context */
bezctx *
new_bezctx_ff(void) {
    bezctx_ff *result = chunkalloc(sizeof(bezctx_ff));

    result->base.moveto = bezctx_ff_moveto;
    result->base.lineto = bezctx_ff_lineto;
    result->base.quadto = bezctx_ff_quadto;
    result->base.curveto = bezctx_ff_curveto;
    result->base.mark_knot = NULL;
    result->is_open = 0;
    result->gotnans = 0;
    result->ss = NULL;
    return &amp;result->base;
}

/* Finishes an old fontforge bézier context, and returns the contour which was created */
struct splinepointlist *
bezctx_ff_close(bezctx *z)
{
    bezctx_ff *bc = (bezctx_ff *)z;
    SplineSet *ss = bc->ss;

    if (!bc->is_open &amp;&amp; ss!=NULL ) {
	if ( ss->first!=ss->last &amp;&amp;
		RealNear(ss->first->me.x,ss->last->me.x) &amp;&amp;
		RealNear(ss->first->me.y,ss->last->me.y)) {
	    ss->first->prevcp = ss->last->prevcp;
	    ss->first->noprevcp = ss->last->noprevcp;
	    ss->first->prev = ss->last->prev;
	    ss->first->prev->to = ss->first;
	    SplinePointFree(ss->last);
	    ss->last = ss->first;
	} else {
	    SplineMake3(ss->last,ss->first);
	    ss->last = ss->first;
	}
    }
    chunkfree(bc,sizeof(bezctx_ff));
return( ss );
}
```
