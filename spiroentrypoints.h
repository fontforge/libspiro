#ifndef _SPIROENTRYPOINTS_H
#define _SPIROENTRYPOINTS_H
#include "bezctx_intf.h"
#include "spiro.h"

#ifdef __cplusplus
extern "C" {
#endif

    /* Possible values of the "ty" field. */
#define SPIRO_CORNER		'v'
#define SPIRO_G4		'o'
#define SPIRO_G2		'c'
#define SPIRO_LEFT		'['
#define SPIRO_RIGHT		']'

    /* For a closed contour add an extra cp with a ty set to */
#define SPIRO_END		'z'
    /* For an open contour the first cp must have a ty set to*/
#define SPIRO_OPEN_CONTOUR	'{'
    /* For an open contour the last cp must have a ty set to */
#define SPIRO_END_OPEN_CONTOUR	'}'

/* These 6 functions are kept for backwards compatibility for older */
/* programs. Please use the functions listed afterwards that return */
/* success/failure replies when done. See version 20190731 or later */
extern void TaggedSpiroCPsToBezier(spiro_cp *spiros,bezctx *bc);
extern void SpiroCPsToBezier(spiro_cp *spiros,int n,int isclosed,bezctx *bc);

/* These functions are available in libspiro-0.2.20130930 or higher */
extern int TaggedSpiroCPsToBezier0(spiro_cp *spiros,bezctx *bc);
extern int SpiroCPsToBezier0(spiro_cp *spiros,int n,int isclosed,bezctx *bc);

/* These functions are available in libspiro-0.3.20150131 or higher */
extern void TaggedSpiroCPsToBezier1(spiro_cp *spiros,bezctx *bc,int *done);
extern void SpiroCPsToBezier1(spiro_cp *spiros,int n,int isclosed,bezctx *bc,int *done);

/* These functions are available in libspiro-20190731 or higher and */
/* give you best flexibility compared to earlier libspiro versions. */
extern const char *LibSpiroVersion(void);

/* The spiros array should indicate it's own end... So              */
/* Open contours must have the ty field of the first cp set to '{'  */
/*               and have the ty field of the last cp set to '}'    */
/* Closed contours must have an extra cp at the end whose ty is 'z' */
/*               the x&y values of this extra cp are ignored        */
/* ncq allows you to toggle different outputs independent of spiros */
extern int TaggedSpiroCPsToBezier2(spiro_cp *spiros,int ncq,bezctx *bc);

/* The first argument is an array of spiro control points.          */
/* Open contours do not need to start with '{', nor to end with '}' */
/* Close contours do not need to end with 'z'                       */
/* ncq allows you to toggle different outputs independent of spiros */
extern int SpiroCPsToBezier2(spiro_cp *spiros,int n,int ncq,int isclosed,bezctx *bc);

    /* Curve crossing point with a 'fixed angle' of crossing */
#define SPIRO_ANCHOR		'a'
    /* Curve crossing handle cp(hx,hy) relative to cp(ax,ay) */
#define SPIRO_HANDLE		'h'

/* int ncq flags and values */
#define SPIRO_INCLUDE_LAST_KNOT	0x0100
#define SPIRO_RETRO_VER1	0x0400
#define SPIRO_REVERSE_SRC	0x0800
#define SPIRO_ARC_CUB_QUAD_CLR	0x7FFF
#define SPIRO_ARC_CUB_QUAD_MASK	0x7000
#define SPIRO_CUBIC_TO_BEZIER	0x0000
#define SPIRO_CUBIC_MIN_MAYBE	0x1000
#define SPIRO_ARC_MAYBE		0x2000
#define SPIRO_ARC_MIN_MAYBE	0x3000
#define SPIRO_QUAD0_TO_BEZIER	0x4000

#ifdef __cplusplus
}
#endif
#endif
