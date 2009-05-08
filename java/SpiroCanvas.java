
package net.sourceforge.libspiro;

import java.awt.*;

class SpiroCanvas extends java.awt.Canvas {
    java.awt.geom.GeneralPath path;

    public SpiroCanvas(java.awt.geom.GeneralPath p) {
	super();
	path = p;
    }

    public void paint(Graphics g) {
	Graphics2D g2 = (Graphics2D)g;
	g2.draw(path);
    }
}
