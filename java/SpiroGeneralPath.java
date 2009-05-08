
package net.sourceforge.libspiro;

import java.awt.*;

class SpiroGeneralPath implements SpiroBezierContext {
    boolean wasOpen;
    java.awt.geom.GeneralPath gp;

    public SpiroGeneralPath() {
	gp = new java.awt.geom.GeneralPath();
    }

    public java.awt.geom.GeneralPath getPath() {
	if ( wasOpen ) {
	    gp.closePath();
	    wasOpen = false;
	}
    return( gp );
    }

    public void MoveTo(double x, double y, boolean isOpen) {
	if ( wasOpen )
	    gp.closePath();
	gp.moveTo((float) x,(float) y);
	wasOpen = isOpen;
    }

    public void LineTo(double x, double y) {
	gp.lineTo((float) x,(float) y);
    }

    public void QuadTo(double x1, double y1, double x2, double y2) {
	gp.quadTo((float) x1,(float) y1,(float) x2,(float) y2);
    }

    public void CubicTo(double x1, double y1, double x2, double y2, double x3, double y3) {
	gp.curveTo((float) x1,(float) y1,(float) x2,(float) y2,(float) x3,(float) y3);
    }

    public void MarkKnot(int knotIdx) {}
}
