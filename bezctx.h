/* bezctx.h
libspiro - A sharable library of Spiro formula and functions.
*/
#ifndef _BEZCTX_H
#define _BEZCTX_H

#ifdef __cplusplus
extern "C" {
#endif

/* NOTE: The Bezier context; You must create a super-class of this abstract type that */
/* handles the creation of your particular representation of bézier splines. You will */
/* see examples and information in file bezctx.md */
typedef struct _bezctx bezctx;

struct _bezctx {
    /* Called by spiro to start a contour */
    void (*moveto)(bezctx *bc, double x, double y, int is_open);

    /* Called by spiro to move from the last point to the next one on a straight line */
    void (*lineto)(bezctx *bc, double x, double y);

    /* Called by spiro to move from the last point to the next along a quadratic bezier spline */
    /* (x1,y1) is the quadratic bezier control point and (x2,y2) will be the new end point */
    void (*quadto)(bezctx *bc, double x1, double y1, double x2, double y2);

    /* Called by spiro to move from the last point to the next along a cubic bezier spline */
    /* (x1,y1) and (x2,y2) are the two off-curve control point and (x3,y3) will be the new end point */
    void (*curveto)(bezctx *bc, double x1, double y1, double x2, double y2,
		    double x3, double y3);

    void (*mark_knot)(bezctx *bc, int knot_idx);
};


/* If you are using ncq definition "SPIRO_INTERNAL_BEZCTX", you also need these below */
typedef struct {
    char ty;		/* curve type */
    double x0,y0,x1,y1,x2,y2,x3,y3;
} curve_data;

typedef struct {
    bezctx base;	/* reserved */
    curve_data *cd;	/* bezier curve data */
    int l;		/* array data length */
    int max;		/* max array allowed */
    int is_open;
} ls_bezctx;

#ifdef __cplusplus
}
#endif
#endif
