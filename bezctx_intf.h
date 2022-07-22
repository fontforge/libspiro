/* bezctx_inf.h
libspiro - A sharable library of Spiro formula and functions.

This file is a fork from ppedit for use by the libspiro project.
Please see Changelog or git history for description of changes.
=============================================================
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
#ifndef _BEZCTX_INTF_H
#define _BEZCTX_INTF_H
#include "bezctx.h"

void
bezctx_moveto(bezctx *bc, double x, double y, int is_open, int si);

void
bezctx_lineto(bezctx *bc, double x, double y, int si);

void
bezctx_quadto(bezctx *bc, double x1, double y1, double x2, double y2, int si);

void
bezctx_curveto(bezctx *bc, double x1, double y1, double x2, double y2, \
	       double x3, double y3, int si);

void
bezctx_mark_knot(bezctx *bc, int knot_idx, int i);

#endif
