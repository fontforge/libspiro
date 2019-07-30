/*
libspiro - conversion between spiro control points and bezier's
Copyright (C) 2007... Raph Levien
Copyright (C) 2013... Joe Da Silva

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
/* Interface routines to Raph's spiro package. */

#include "spiroentrypoints.h"

/* These six functions are kept for backwards compatibility */
void SpiroCPsToBezier(spiro_cp *spiros,int n,int isclosed,bezctx *bc) {
    SpiroCPsToBezier2(spiros,n,SPIRO_RETRO_VER1,isclosed,bc);
}
void TaggedSpiroCPsToBezier(spiro_cp *spiros,bezctx *bc) {
    TaggedSpiroCPsToBezier2(spiros,SPIRO_RETRO_VER1,bc);
}
int SpiroCPsToBezier0(spiro_cp *spiros,int n,int isclosed,bezctx *bc) {
    return SpiroCPsToBezier2(spiros,n,SPIRO_RETRO_VER1,isclosed,bc);
}
int TaggedSpiroCPsToBezier0(spiro_cp *spiros,bezctx *bc) {
    return TaggedSpiroCPsToBezier2(spiros,SPIRO_RETRO_VER1,bc);
}
void SpiroCPsToBezier1(spiro_cp *spiros,int n,int isclosed,bezctx *bc,int *done) {
    *done = SpiroCPsToBezier2(spiros,n,SPIRO_RETRO_VER1,isclosed,bc);
}
void TaggedSpiroCPsToBezier1(spiro_cp *spiros,bezctx *bc,int *done) {
    *done = TaggedSpiroCPsToBezier2(spiros,SPIRO_RETRO_VER1,bc);
}


int SpiroCPsToBezier2(spiro_cp *spiros,int n,int ncq,int isclosed,bezctx *bc) {
    double dm[6];
    spiro_seg *s;

    if ( n<=0 || ncq<0 ) return 0; /* invalid input */
    if ( n > 1 && (ncq & SPIRO_REVERSE_SRC) && spiroreverse(spiros,n) )
	return 0;

    if ( isclosed )
	s = run_spiro0(spiros,dm,ncq,n);
    else {
	char oldty_start = spiros[0].ty;
	char oldty_end   = spiros[n-1].ty;
	spiros[0].ty = '{';
	spiros[n-1].ty = '}';
	s = run_spiro0(spiros,dm,ncq,n);
	spiros[n-1].ty = oldty_end;
	spiros[0].ty = oldty_start;
    }
    if (s) {
	spiro_to_bpath0(spiros,s,dm,ncq,n,bc);
	free_spiro(s);
	return 1; /* success */
    }
    return 0; /* spiro did not converge or encountered non-finite values */
}

int TaggedSpiroCPsToBezier2(spiro_cp *spiros,int ncq,bezctx *bc) {
    double dm[6];
    spiro_seg *s;
    int n;

    for ( n=0; spiros[n].ty!='z' && spiros[n].ty!='}'; ++n );
    if ( spiros[n].ty == '}' ) ++n;

    if ( n<=0 || ncq<0 ) return 0; /* invalid input */
    if ( n > 1 && (ncq & SPIRO_REVERSE_SRC) && spiroreverse(spiros,n) )
	return 0;
    s = run_spiro0(spiros,dm,ncq,n);
    if (s) {
	spiro_to_bpath0(spiros,s,dm,ncq,n,bc);
	free_spiro(s);
	return 1; /* success */
    }
    return 0; /* spiro did not converge or encountered non-finite values */
}
