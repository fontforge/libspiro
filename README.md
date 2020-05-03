# Spiro ![](spiral32.png) [![Coverity Scan Build Status](https://scan.coverity.com/projects/794/badge.svg?flat=1)](https://scan.coverity.com/projects/794)

## Introduction

![](spiro-a.png)

Spiro is the creation of [Raph Levien](http://www.levien.com/). It simplifies the drawing of beautiful curves.

Using bézier splines an artist can easily draw curves with the same slope on either side of an on-curve point. Spiros, on the other hand, are based on clothoid splines which make it easy to maintain constant curvature as well as constant slope. Such curves will simply look nicer.

Raph Levien's spiro splines only use on-curve points and so are easier to use and more intuitive to the artist.

This library will take an array of spiro control points and convert them into a series of bézier splines which can then be used in the myriad of ways the world has come to use béziers.

## Installation

Installing from Git master requires 2 preparatory steps:

First, you need to create the ./configure script if you do not have it yet
```sh
autoreconf -i  (or use 'autoreconf --install --force' for more modern setups)
automake --foreign -Wall
```

Second, you then use the usual steps to compile the library.
Various operating systems and setups will need ./configure options set.
The INSTALLATION file has detailed info for `configure' options.
Example install steps for Linux, FreeBSD, Win32/64 are shown below:

Installing on Linux
```sh
./configure
make
make check
sudo make install
```

Installing on FreeBSD
```sh
./configure --prefix=$(pwd)/BUILD
make clean
make
make install
```

Installing on Windows 32-bit
```sh
./configure --host=i686-w64-mingw32 --prefix=$(pwd)/build-w32
make clean
make
make install
```

Installing on Windows 64-bit
```sh
./configure --host=x86_64-w64-mingw32 --prefix=$(pwd)/build-w64
make clean
make
make install
```

NOTE: Some Distros and Operating Systems may require you to run 'ldconfig'
to recognize LibSpiro if you are not rebooting your computer first before
installing another program that depends on LibSpiro. To do this, you may
need to run 'ldconfig' in 'su -' mode after you have done 'make install':
```sh
	$ su -
	# ldconfig
	# exit
	$
```

## Usage

### In FontForge

FontForge will autodetect libspiro when it is installed in the usual way.

An exception to this is with the Mac bundled version (where `FontForge.app` is copied to `/Applications`). To install your compiled version into the bundle, run ```sh ./configure --prefix=/Applications/FontForge.app/Contents/Resources/opt/local/ ```

#### Crash Reporting

Mac OS X: A helping script, `./fontforge.sh` is provided to run FontForge inside a debugger to get useful information on solving crashes. An example issue is at https://github.com/fontforge/libspiro/issues/4

# Developing

### Two methods of using libspiro in your programs

- [C](#programming-with-libspiro-in-c)
- [Java](#programming-with-libspiro-in-java)

### Programming with libspiro in C

- Basic Types
  - [spiro control point](#the-spiro-control-point)
  - [ncq control value](#the-ncq-control-value)
  - [bézier context](#the-bezier-context)
- [Header file](#calling-into-libspiro)
- Entry points
  - int [SpiroCPsToBezier2](#spirocpstobezier2)(spiro_cp *,int n,int ncq,int is_closed,bezctx *)
  - int [TaggedSpiroCPsToBezier2](#taggedspirocpstobezier2)(spiro_cp *,int ncq,bezctx *)

#### Basic Types

#### The spiro control point

```c
typedef struct {
    double x;
    double y;
    char ty;
} spiro_cp;

    /* Possible values of the "ty" field. */
#define SPIRO_CORNER	'v'
#define SPIRO_G4	'o'
#define SPIRO_G2	'c'
#define SPIRO_LEFT	'['
#define SPIRO_RIGHT	']'
#define SPIRO_ANCHOR	'a'
#define SPIRO_HANDLE	'h'

    /* For a closed contour add an extra cp with a ty set to */
#define SPIRO_END		'z'
    /* For an open contour the first cp must have a ty set to*/
#define SPIRO_OPEN_CONTOUR	'{'
    /* For an open contour the last cp must have a ty set to */
#define SPIRO_END_OPEN_CONTOUR	'}'
```

A spiro control point contains a location and a point type. There are six basic types of spiro control points:

- A corner point – where the slopes and curvatures of the incoming and outgoing splines are unconstrained
- A G4 curve point – Continuous up to the fourth derivative
- A G2 curve point – Continuous up to the second derivative.
- A left constraint point – Used to connect a curve to a straight line
- A right constraint point – Used to connect a straight line to a curve.
  If you have a contour which is drawn clockwise, and you have a straight segment at the top, then the left point of that straight segment should be a left constraint, and the right point should be a right constraint.
- An anchor - Is a knot point with a fixed angle (followed by the handle cp which creates the angle).
  The anchor behaves as both left and right constraint points at one single point.

The left constraint, right constraint, anchor and handle points are easiest explained using
examples from path5 and path6 which are tested in tests/call-test5.c and tests/call-test6.c

![](path5.png) ![](path6.png)
```c
path5[]		path6[]
{  0,   0,'{'},	{  0,   0,'{'},
{100, 100,'c'},	{100, 100,'c'},
{200, 200,'['},	{200, 200,'a'},
{300, 200,']'},	{300, 200,'h'},
{400, 150,'c'},	{300, 150,'c'},
{300, 100,'['},	{200, 100,'a'},
{200, 100,']'},	{150, 100,'h'},
{150,  50,'c'},	{150,  50,'c'},
{100,   0,'['},	{100,   0,'a'},
{  0,-100,']'},	{  0,-100,'h'},
{-50,-200,'c'},	{ 50,-100,'c'},
{-80,-250,'}'},	{ 20,-150,'}'},
```


## The ncq control value

There is a need to pass additional information to libspiro, and therefore the 'ncq' value was added.
'ncq' can be thought of as toggle switches telling libspiro how to work with the source spiro control points.
Below is the current toggle switch definitions, and default 'ncq' value is zero.

```c
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
```

The definitions for ncq (above) are:

SPIRO_INCLUDE_LAST_KNOT:
Existing libspiro behaviour is to add a knot point to match each spiro point, but does not include the last knot.
This option includes the last knot with the existing output results while the spiro is still open. Closed spiros should refer to the first knot point since the last and first knot are the same.

SPIRO_RETRO_VER1:
This newer version of libspiro has modified the way path calculations are made.
The reason for this was seen as an advantage, because it allows a user to scale and move spiro paths, which is a common expectation in graphics, and there are other added advantages, such as making the path as part of templates, and more.
An effort was made to keep results as close to original as possible, but this was not possible due to scaling factors in the calculations.
As the main user for libspiro is FontForge, users such as font designers may see the least change since scaling targets x={0..1000}, y={0..1000}, while other users in graphics may see changes since they can be using scales much larger than 1000.
The good news here is 'SPIRO_RETRO_VER1' allows the user to toggle libspiro to use the older calculation method if the user needs backwards compatibility, otherwise, leaving this off allows spiros to use the new calculation method which allows scaling and moving spiro paths.
Older programs that use the older libspiro interfaces will see no-change since they use the older calculation method to maintain backwards compatibility.

SPIRO_REVERSE_SRC:
There may be a need to reverse the spiro path direction.
This option edits the source spiro path, and reverses the information, then proceeds to continue doing libspiro calculations with the reversed path.
When libspiro is done calculating bezier output, you will also have a reversed (input) spiro path, therefore save the new spiro path if you need it.
This simplifies this process for the calling program to a simple option 'SPIRO_REVERSE_SRC', and the results are up to date as per ths version of libspiro.
NOTE - libspiro calculations are a one-way calculation, so you are not likely to see the same results in the reverse spiro path direction, but if you need this option, it is available here.

SPIRO_CUBIC_TO_BEZIER:
LibSpiro default action is to create cubic bezier curves.

SPIRO_CUBIC_MIN_MAYBE:
Cubic arcs can potentially be made with greater bends and less points.

SPIRO_ARC_MAYBE and SPIRO_ARC_MIN_MAYBE:
Instead of the default cubic output, this exposes the midpoint, which might be useful to someone.

SPIRO_QUAD0_TO_BEZIER:
Rough approximation of quadratic to bezier curves. Knot points will have smooth connection but midpoints may be visually okay or not.


#### The bezier context

```c
struct _bezctx {
    /* Called by spiro to start a contour */
    void (*moveto)(bezctx *bc, double x, double y, int is_open);

    /* Called by spiro to move from the last point to the next one on a straight line */
    void (*lineto)(bezctx *bc, double x, double y);

    /* Called by spiro to move from the last point to the next along a quadratic bézier spline */
    /* (x1,y1) is the quadratic bézier control point and (x2,y2) will be the new end point */
    void (*quadto)(bezctx *bc, double x1, double y1, double x2, double y2);

    /* Called by spiro to move from the last point to the next along a cubic bézier spline */
    /* (x1,y1) and (x2,y2) are the two off-curve control point and (x3,y3) will be the new end point */
    void (*curveto)(bezctx *bc, double x1, double y1, double x2, double y2,
            double x3, double y3);

    /* Called by spiro to notify calling function this is a knot point */
    void (*mark_knot)(bezctx *bc, int knot_idx);
};
```

You must create a super-class of this abstract type that handles the creation of your particular representation of bézier splines. As an [example I provide the one used by Raph to generate PostScript output](bezctx.md) (cubic béziers). Spiro will convert a set of spiro_cps into a set of bézier curves. As it does so it will call the appropriate routine in your bézier context with this information – this should allow you to create your own internal representation of those curves.

#### Calling into libspiro

Your program needs this Libspiro header file:

```c
#include <spiroentrypoints.h>
```

You must define a bézier context that is appropriate for your internal splines (See [Raph's PostScript example](bezctx.md)).

#### SpiroCPsToBezier2

You must create an array of spiro control points:

```c
   spiro_cp points[4];

     /* This defines something very like a circle, centered at the origin with radius 100 */

   points[0].x = -100; points[0].y =    0; points[0].ty = SPIRO_G4;
   points[1].x =    0; points[1].y =  100; points[1].ty = SPIRO_G4;
   points[2].x =  100; points[2].y =    0; points[2].ty = SPIRO_G4;
   points[3].x =    0; points[3].y = -100; points[3].ty = SPIRO_G4;
```

![](closedspiro.png)

Then call `SpiroCPsToBezier2`, a routine which takes 5 arguments and returns bc and an integer pass/fail flag.

1. An array of input spiros
2. The number of elements in the spiros array (this example has 4)
3. Additional ncq control variable (default==0)
4. Whether this describes a closed (True=1) or open (False=0) contour
5. A bézier results output context
6. An integer success flag. 1 = completed task and have valid bézier results, or  0 = unable to complete task, bézier results are invalid.

  ```c
    bc = new_bezctx_ps();
    success = SpiroCPsToBezier2(points,4,ncq,True,bc)
    bezctx_ps_close(bc);
  ```

#### TaggedSpiroCPsToBezier2

Or call `TaggedSpiroCPsToBezier2`. This routine requires that the array of spiro control points be tagged according to Raph's internal conventions. A closed curve will have an extra control point attached to the end of it with a type of `SPIRO_END`;

```c
   spiro_cp points[5];

   points[0].x = -100; points[0].y =    0; points[0].ty = SPIRO_G4;
   points[1].x =    0; points[1].y =  100; points[1].ty = SPIRO_G4;
   points[2].x =  100; points[2].y =    0; points[2].ty = SPIRO_G4;
   points[3].x =    0; points[3].y = -100; points[3].ty = SPIRO_G4;
   points[4].x =    0; points[4].y =    0; points[4].ty = SPIRO_END;
```

(The x,y location of this last SPIRO_END point is irrelevant).

An open curve will have the type of the first control point set to `SPIRO_OPEN_CONTOUR` and the last to `SPIRO_END_OPEN_CONTOUR`.

```c
   spiro_cp points[4];

   points[0].x = -100; points[0].y =    0; points[0].ty = SPIRO_OPEN_CONTOUR;
   points[1].x =    0; points[1].y =  100; points[1].ty = SPIRO_G4;
   points[2].x =  100; points[2].y =    0; points[2].ty = SPIRO_G4;
   points[3].x =    0; points[3].y = -100; points[3].ty = SPIRO_END_OPEN_CONTOUR;
```

![](openspiro.png)

(In an open contour the point types of the first and last control points are going to be ignored).

In this case there is no need to provide a point count nor an open/closed contour flag. That information can be obtained from the control points themselves. So `TaggedSpiroCPsToBezier2` only takes 3 arguments and returns bc and an integer pass/fail flag.

1. An array of input spiros
2. A bézier results output context
3. Additional ncq control variable (default==0)
4. An integer success flag. 1 = completed task and have valid bézier results, or  0 = unable to complete task, bézier results are invalid.

   ```c
    bc = new_bezctx_ps();
    success = TaggedSpiroCPsToBezier2(points,ncq,bc)
    bezctx_ps_close(bc);
   ```

### Programming with libspiro in Java

**CAVEAT:** I'm not proficient in Java.

### Classes

- `SpiroPointType` – this is an enumerated type which defines the same pointtypes used by the C interface: `CORNER`, `G4`, `G2`, `LEFT`, `RIGHT`, `END`, `OPEN`, `OPEN_END`

- `SpiroCP`
  ```java
  public class SpiroCP {
    public double x,y;
    SpiroPointType type;
    public SpiroCP(double xx, double yy, SpiroPointType ty);
    public String toString();
  }
  ```

- `SpiroBezierContext` – a Java interface used in conversion of an array of SpiroCPs to a Bézier contour.

  ```java
  public interface SpiroBezierContext {
    void MoveTo(double x, double y, boolean isOpen);
    void LineTo(double x, double y);
    void QuadTo(double x1, double y1, double x2, double y2);
    void CubicTo(double x1, double y1, double x2, double y2, double x3, double y3);
    void MarkKnot(int knotIdx);
  }
  ```

- `Spiro` – a class with only static members:

  ```java
  public class Spiro {
    // takes an array of SpiroCPs and converts to a Bézier
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
