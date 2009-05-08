
package net.sourceforge.libspiro;

import java.awt.*;

class ShowPlate {

    public static void main( String [] args) {
	SpiroCP [][] contours;
	String arg;
	SpiroGeneralPath sgp;
	java.awt.geom.GeneralPath gp;
	int i,j;
	java.awt.Rectangle size;

	for ( i=0; i<args.length; ++i ) {
	    arg = args[i];
	    try {
		contours = Spiro.ReadPlateFile(new java.io.BufferedReader(new java.io.FileReader(arg)));
	    } catch ( java.io.FileNotFoundException foo ) {
	        contours = null;
	    } catch ( java.io.IOException foo ) {
	        contours = null;
	    }
	    if ( contours == null )
		System.err.println("Cannot open file: " + arg );
	    else {
		Frame f;
		SpiroCanvas c;
		sgp = new SpiroGeneralPath();
		for ( j=0; j<contours.length; ++j )
		    Spiro.TaggedSpiroCPsToBezier(contours[j],sgp);
		gp = sgp.getPath();
		size = gp.getBounds();
		f = new Frame(arg);
		System.out.printf( "Width: %d Height %d\n", size.width, size.height );
		f.setSize(size.width,2*size.height+10);
		c = new SpiroCanvas(gp);
		f.add(c);
		f.setVisible(true);
	    }
	}
	try {
	    Thread.currentThread().sleep(10*1000);
	} catch ( java.lang.InterruptedException foo ) {
	}
    }
}
