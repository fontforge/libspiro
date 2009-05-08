/*
libspiro - conversion between spiro control points and bezier's
Copyright (C) 2007 Raph Levien
              2009 converted to Java by George Williams

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

package net.sourceforge.libspiro;

import static net.sourceforge.libspiro.SpiroPointType.*;

public class SpiroCP {
    public double x,y;
    SpiroPointType type;

    public SpiroCP(double xx, double yy, SpiroPointType ty) {
	x = xx; y = yy; type = ty;
    }

    public String toString() {
	return( String.format("(%c %g %g)",
		    type==CORNER ? 'v' :
		    type==G4     ? 'o' :
		    type==G2     ? 'c' :
		    type==LEFT   ? '[' :
		    type==RIGHT  ? ']' :
		    type==OPEN   ? '{' :
		    type==OPEN_END ? '}' :
		         'z',
		    x, y ));
    }
}
