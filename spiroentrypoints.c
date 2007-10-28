/*
libspiro - conversion between spiro control points and bezier's
Copyright (C) 2007 Raph Levien

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
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

void
SpiroCPsToBezier(spiro_cp *spiros,int n,int isclosed,bezctx *bc)
{
    spiro_seg *s;

    if ( n<1 )
return;
    if ( !isclosed ) {
	char oldty_start = spiros[0].ty;
	char oldty_end   = spiros[n-1].ty;
	spiros[0].ty = '{';
	spiros[n-1].ty = '}';
	s = run_spiro(spiros,n);
	spiros[n-1].ty = oldty_end;
	spiros[0].ty = oldty_start;
    } else
	s = run_spiro(spiros,n);
    spiro_to_bpath(s,n,bc);
    free_spiro(s);
}

void
TaggedSpiroCPsToBezier(spiro_cp *spiros,bezctx *bc)
{
    spiro_seg *s;
    int n;

    for ( n=0; spiros[n].ty!='z' && spiros[n].ty!='}'; ++n );
    if ( spiros[n].ty == '}' ) ++n;

    if ( n<1 )
return;
    s = run_spiro(spiros,n);
    spiro_to_bpath(s,n,bc);
    free_spiro(s);
}
