# Spiro

![](spiral16.png)
![](spiral32.png)

## Introduction

![](spiro-a.png)

Spiro is the creation of [Raph Levien](http://www.levien.com/). It simplifies the drawing of beautiful curves.

Using bézier splines an artist can easily draw curves with the same slope on either side of an on-curve point. Spiros, on the other hand, are based on clothoid splines which make it easy to maintain constant curvature as well as constant slope. Such curves will simply look nicer.

Raph Levien's spiro splines only use on-curve points and so are easier to use and more intuitive to the artist.

This library will take an array of spiro control points and convert them into a series of bézier splines which can then be used in the myriad of ways the world has come to use béziers.

### Two methods of using libspiro

- [C](#programming-with-libspiro-in-c)
- [Java](#programming-with-libspiro-in-java)

## Programming with libspiro in C

- Basic Type
  - [spiro control point](#the-spiro-control-point)
  - [bezier context](#the-bzier-context)
- [Header file](#calling-into-libspiro)
- Entry points
  - [SpiroCPsToBezier](#spirocpstobezier)(spiros_cp *,int n,int is_closed,bezctx *)
  - [TaggedSpiroCPsToBezier](#taggedspirocpstobezier)(spiros_cp *,bezctx *) 

### Basic Types

#### The spiro control point

```c
typedef struct {
    double x;
    double y;
    char ty;
} spiro_cp;

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
```

A spiro control point contains a location and a point type. There are five basic types of spiro control points:

- A corner point -- where the slopes and curvatures of the incoming and outgoing splines are unconstrained
- A G4 curve point -- Continuous up to the fourth derivative
- A G2 curve point -- Continuous up to the second derivative.
- A left constraint point -- Used to connect a curved line to a straight one
- A right constraint point -- Used to connect a straight line to a curved one.
  If you have a contour which is drawn clockwise, and you have a straight segment at the top, then the left point of that straight segment should be a left constraint, and the right point should be a right constraint.

#### The bézier context

```c
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

    /* I'm not entirely sure what this does -- I just leave it blank */
    void (*mark_knot)(bezctx *bc, int knot_idx);
};
```

You must create a super-class of this abstract type that handles the creation of your particular representation of bézier splines. As an [example I provide the one used by Raph to generate PostScript output](bezctx.md) (cubic beziers). Spiro will convert a set of spiro_cps into a set of bezier curves. As it does so it will call the appropriate routine in your bezier context with this information -- this should allow you to create your own internal representation of those curves.

### Calling into libspiro

Libspiro needs a header file:

```c
#include <spiroentrypoints.h>
```

You must define a bezier context that is appropriate for your internal splines (See [Raph's PostScript example](bezctx.md)).

#### SpiroCPsToBezier

You must create an array of spiro control points:

```c
   spiro_cp points[4];

     /* This defines something very like a circle, centered at the origin with radius 100 */
   ![](closedspiro.png)
   points[0].x = -100; points[0].y =    0; points[0].ty = SPIRO_G4;
   points[1].x =    0; points[1].y =  100; points[1].ty = SPIRO_G4;
   points[2].x =  100; points[2].y =    0; points[2].ty = SPIRO_G4;
   points[3].x =    0; points[3].y = -100; points[3].ty = SPIRO_G4;
```

Then call `SpiroCPsToBezier`, a routine which takes 4 arguments

1. An array of spiros
2. The number of elements in the array
3. Whether this describes a closed (True) or open (False) contour
4. A bezier context
   ```c
    bc = new_bezctx_ps();
    SpiroCPsToBezier(points,4,True,bc)
    bezctx_ps_close(bc);
   ```

#### TaggedSpiroCPsToBezier

Or call `TaggedSpiroCPsToBezier`. This routine requires that the array of spiro control points be tagged according to Raph's internal conventions. A closed curve will have an extra control point attached to the end of it with a type of `SPIRO_END`;

```c
   spiro_cp points[5];

   points[0].x = -100; points[0].y =    0; points[0].ty = SPIRO_G4;
   points[1].x =    0; points[1].y =  100; points[1].ty = SPIRO_G4;
   points[2].x =  100; points[2].y =    0; points[2].ty = SPIRO_G4;
   points[3].x =    0; points[3].y = -100; points[3].ty = SPIRO_G4;
   points[4].x =    0; points[4].y =    0; points[4].ty = SPIRO_END;
```

(The location of this last point is irrelevant).

An open curve will have the type of the first control point set to `SPIRO_OPEN_CONTOUR` and the last to `SPIRO_END_OPEN_CONTOUR`.

```c
   spiro_cp points[4];
   ![](openspiro.png)
   points[0].x = -100; points[0].y =    0; points[0].ty = SPIRO_OPEN_CONTOUR;
   points[1].x =    0; points[1].y =  100; points[1].ty = SPIRO_G4;
   points[2].x =  100; points[2].y =    0; points[2].ty = SPIRO_G4;
   points[3].x =    0; points[3].y = -100; points[3].ty = SPIRO_END_OPEN_CONTOUR;
```

(In an open contour the point types of the first and last control points are going to be ignored).

In this case there is no need to provide a point count nor an open/closed contour flag. That information can be obtained from the control points themselves. So `TaggedSpiroCPsToBezier` only takes 2 arguments

1. An array of spiros
2. A bezier context
   ```c
    bc = new_bezctx_ps();
    TaggedSpiroCPsToBezier(points,bc)
    bezctx_ps_close(bc);
   ```

## Programming with libspiro in Java

**CAVEAT:** I'm not proficient in Java.

### Classes

- `SpiroPointType` -- this is an enumerated type which defines the same pointtypes used by the C interface: `CORNER`, `G4`, `G2`, `LEFT`, `RIGHT`, `END`, `OPEN`, `OPEN_END`

- `SpiroCP`
  ```java
    public class SpiroCP {
    public double x,y;
    SpiroPointType type;
    public SpiroCP(double xx, double yy, SpiroPointType ty);
    public String toString();
}
  ```

- `SpiroBezierContext` -- a Java interface used in conversion of an array of SpiroCPs to a Bezier contour.

  ```java
    public interface SpiroBezierContext {
    void MoveTo(double x, double y, boolean isOpen);
    void LineTo(double x, double y);
    void QuadTo(double x1, double y1, double x2, double y2);
    void CubicTo(double x1, double y1, double x2, double y2, double x3, double y3);
    void MarkKnot(int knotIdx);
    }
  ```

- `Spiro` -- a class with only static members:

  ```java
    public class Spiro {
    // takes an array of SpiroCPs and converts to a Bezier
    static public void
      SpiroCPsToBezier(SpiroCP [] spiros,int n,boolean isclosed,
          SpiroBezierContext bc);
    // takes an array of SpiroCPs (the array contains its own terminator and indication of whether the contour is open or closed)
    static public void
      TaggedSpiroCPsToBezier(SpiroCP [] spiros,SpiroBezierContext bc);

    // Two routines for reading and writing one of Raph's plate files
    static public void
      SavePlateFile(Writer output,SpiroCP [][] spirocontours)
      throws IOException;
    static public SpiroCP [][]
      ReadPlateFile(BufferedReader input) throws IOException;
    }
  ```
