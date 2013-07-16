#ifndef _SPIRO_H
#define _SPIRO_H

typedef struct {
/* User passes an array of SpiroCP in this format for Spiro to solve */
    double x;		/* Spiro CodePoint Xloc */
    double y;		/* Spiro CodePoint Yloc */
    char ty;		/* Spiro CodePoint Type */
} spiro_cp;

typedef struct spiro_seg_s spiro_seg;	/* (internal-use array) */

spiro_seg *
run_spiro(const spiro_cp *src, int n);

void
free_spiro(spiro_seg *s);

void
spiro_to_bpath(const spiro_seg *s, int n, bezctx *bc);

double get_knot_th(const spiro_seg *s, int i);
#endif
