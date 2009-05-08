/*
libspiro - conversion between spiro control points and bezier's
CopySpiroPointType.RIGHT (C) 2007 Raph Levien
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

import java.lang.Math;
import java.util.ArrayList;
import java.io.Writer;
import java.io.BufferedReader;
import java.io.IOException;
import static net.sourceforge.libspiro.SpiroPointType.*;

public class Spiro {
    // Spiro has no constructors. It simply provides (static) methods to convert
    // between a spiro contour (an array of SpiroCP) and a cubic Bezier contour
    // and routines for reading/writing plate files

    static protected class spiro_seg {
	double x;
	double y;
	SpiroPointType type;
	double bend_th;
	double [/*4*/] ks;
	double seg_ch;
	double seg_th;
	double l;
    }

    static protected class bandmat {
	double [/*11*/] a;  /* band-diagonal matrix */
	double [/*5 */] al; /* lower part of band-diagonal decomposition */

	void copyfrom(final bandmat from) {
	    System.arraycopy(a ,0,from.a ,0,11);
	    System.arraycopy(al,0,from.al,0,5 );
	}

	static void arraycopy(bandmat [] from, int fromi, bandmat [] to, int toi, int nelem ) {
	    int i;
    
	    for ( i=0; i<nelem; ++i )
		to[i+toi].copyfrom( from[i+fromi] );
	}
    }

    static final int N = 4;
    /* ORDER = 12; */


/* Integrate polynomial spiral curve over range -.5 .. .5. */
    static protected void
      integrate_spiro(final double [/*4*/] ks, double [/*2*/] xy) {
	double th1 = ks[0];
	double th2 = .5 * ks[1];
	double th3 = (1./6) * ks[2];
	double th4 = (1./24) * ks[3];
	double x, y;
	double ds = 1. / N;
	double ds2 = ds * ds;
	double ds3 = ds2 * ds;
	double k0 = ks[0] * ds;
	double k1 = ks[1] * ds;
	double k2 = ks[2] * ds;
	double k3 = ks[3] * ds;
	int i;
	double s = .5 * ds - .5;

	x = 0;
	y = 0;

	for (i = 0; i < N; i++) {
	    double u, v;
	    double km0, km1, km2, km3;

	    km0 = (((1./6) * k3 * s + .5 * k2) * s + k1) * s + k0;
	    km1 = ((.5 * k3 * s + k2) * s + k1) * ds;
	    km2 = (k3 * s + k2) * ds2;
	    km3 = k3 * ds3;
	    {
		double t1_1 = km0;
		double t1_2 = .5 * km1;
		double t1_3 = (1./6) * km2;
		double t1_4 = (1./24) * km3;
		double t2_2 = t1_1 * t1_1;
		double t2_3 = 2 * (t1_1 * t1_2);
		double t2_4 = 2 * (t1_1 * t1_3) + t1_2 * t1_2;
		double t2_5 = 2 * (t1_1 * t1_4 + t1_2 * t1_3);
		double t2_6 = 2 * (t1_2 * t1_4) + t1_3 * t1_3;
		double t2_7 = 2 * (t1_3 * t1_4);
		double t2_8 = t1_4 * t1_4;
		double t3_4 = t2_2 * t1_2 + t2_3 * t1_1;
		double t3_6 = t2_2 * t1_4 + t2_3 * t1_3 + t2_4 * t1_2 + t2_5 * t1_1;
		double t3_8 = t2_4 * t1_4 + t2_5 * t1_3 + t2_6 * t1_2 + t2_7 * t1_1;
		double t3_10 = t2_6 * t1_4 + t2_7 * t1_3 + t2_8 * t1_2;
		double t4_4 = t2_2 * t2_2;
		double t4_5 = 2 * (t2_2 * t2_3);
		double t4_6 = 2 * (t2_2 * t2_4) + t2_3 * t2_3;
		double t4_7 = 2 * (t2_2 * t2_5 + t2_3 * t2_4);
		double t4_8 = 2 * (t2_2 * t2_6 + t2_3 * t2_5) + t2_4 * t2_4;
		double t4_9 = 2 * (t2_2 * t2_7 + t2_3 * t2_6 + t2_4 * t2_5);
		double t4_10 = 2 * (t2_2 * t2_8 + t2_3 * t2_7 + t2_4 * t2_6) + t2_5 * t2_5;
		double t5_6 = t4_4 * t1_2 + t4_5 * t1_1;
		double t5_8 = t4_4 * t1_4 + t4_5 * t1_3 + t4_6 * t1_2 + t4_7 * t1_1;
		double t5_10 = t4_6 * t1_4 + t4_7 * t1_3 + t4_8 * t1_2 + t4_9 * t1_1;
		double t6_6 = t4_4 * t2_2;
		double t6_7 = t4_4 * t2_3 + t4_5 * t2_2;
		double t6_8 = t4_4 * t2_4 + t4_5 * t2_3 + t4_6 * t2_2;
		double t6_9 = t4_4 * t2_5 + t4_5 * t2_4 + t4_6 * t2_3 + t4_7 * t2_2;
		double t6_10 = t4_4 * t2_6 + t4_5 * t2_5 + t4_6 * t2_4 + t4_7 * t2_3 + t4_8 * t2_2;
		double t7_8 = t6_6 * t1_2 + t6_7 * t1_1;
		double t7_10 = t6_6 * t1_4 + t6_7 * t1_3 + t6_8 * t1_2 + t6_9 * t1_1;
		double t8_8 = t6_6 * t2_2;
		double t8_9 = t6_6 * t2_3 + t6_7 * t2_2;
		double t8_10 = t6_6 * t2_4 + t6_7 * t2_3 + t6_8 * t2_2;
		double t9_10 = t8_8 * t1_2 + t8_9 * t1_1;
		double t10_10 = t8_8 * t2_2;
		u = 1;
		v = 0;
		v += (1./12) * t1_2 + (1./80) * t1_4;
		u -= (1./24) * t2_2 + (1./160) * t2_4 + (1./896) * t2_6 + (1./4608) * t2_8;
		v -= (1./480) * t3_4 + (1./2688) * t3_6 + (1./13824) * t3_8 + (1./67584) * t3_10;
		u += (1./1920) * t4_4 + (1./10752) * t4_6 + (1./55296) * t4_8 + (1./270336) * t4_10;
		v += (1./53760) * t5_6 + (1./276480) * t5_8 + (1./1.35168e+06) * t5_10;
		u -= (1./322560) * t6_6 + (1./1.65888e+06) * t6_8 + (1./8.11008e+06) * t6_10;
		v -= (1./1.16122e+07) * t7_8 + (1./5.67706e+07) * t7_10;
		u += (1./9.28973e+07) * t8_8 + (1./4.54164e+08) * t8_10;
		v += (1./4.08748e+09) * t9_10;
		u -= (1./4.08748e+10) * t10_10;
	    }

	    {
		double th = (((th4 * s + th3) * s + th2) * s + th1) * s;
		double cth = Math.cos(th);
		double sth = Math.sin(th);

		x += cth * u - sth * v;
		y += cth * v + sth * u;
		s += ds;
	    }
	}

	xy[0] = x * ds;
	xy[1] = y * ds;
    }

    static protected double
      compute_ends(final double [/*4*/] ks, double [/*2*/][/*4*/] ends, double seg_ch) {
	double [/*2*/] xy = new double[2];
	double ch, th;
	double l, l2, l3;
	double th_even, th_odd;
	double k0_even, k0_odd;
	double k1_even, k1_odd;
	double k2_even, k2_odd;

	integrate_spiro(ks, xy);
	ch = Math.hypot(xy[0], xy[1]);
	th = Math.atan2(xy[1], xy[0]);
	l = ch / seg_ch;

	th_even = .5 * ks[0] + (1./48) * ks[2];
	th_odd = .125 * ks[1] + (1./384) * ks[3] - th;
	ends[0][0] = th_even - th_odd;
	ends[1][0] = th_even + th_odd;
	k0_even = l * (ks[0] + .125 * ks[2]);
	k0_odd = l * (.5 * ks[1] + (1./48) * ks[3]);
	ends[0][1] = k0_even - k0_odd;
	ends[1][1] = k0_even + k0_odd;
	l2 = l * l;
	k1_even = l2 * (ks[1] + .125 * ks[3]);
	k1_odd = l2 * .5 * ks[2];
	ends[0][2] = k1_even - k1_odd;
	ends[1][2] = k1_even + k1_odd;
	l3 = l2 * l;
	k2_even = l3 * ks[2];
	k2_odd = l3 * .5 * ks[3];
	ends[0][3] = k2_even - k2_odd;
	ends[1][3] = k2_even + k2_odd;

	return l;
    }

    static protected void
      compute_pderivs(final spiro_seg s, double [/*2*/][/*4*/] ends,
	  double [/*4*/][/*2*/][/*4*/] derivs, int jinc) {
	double recip_d = 2e6;
	double delta = 1./ recip_d;
	double [] try_ks = new double[4];
	double [][] try_ends = new double [2][4];
	int i, j, k;

	compute_ends(s.ks, ends, s.seg_ch);
	for (i = 0; i < jinc; i++) {
	    for (j = 0; j < 4; j++)
		try_ks[j] = s.ks[j];
	    try_ks[i] += delta;
	    compute_ends(try_ks, try_ends, s.seg_ch);
	    for (k = 0; k < 2; k++)
		for (j = 0; j < 4; j++)
		    derivs[j][k][i] = recip_d * (try_ends[k][j] - ends[k][j]);
	}
    }

    static protected double
      mod_2pi(double th) {
	double u = th / (2 * Math.PI);
	return 2 * Math.PI * (u - Math.floor(u + 0.5));
    }

    static protected spiro_seg []
      setup_path(final SpiroCP [] src, int n) {
	int n_seg = src[0].type == OPEN ? n - 1 : n;
	spiro_seg [] r = new spiro_seg[n_seg +1];
	int i;
	int ilast;

	for (i = 0; i < n_seg; i++) {
	    r[i] = new spiro_seg();
	    r[i].x = src[i].x;
	    r[i].y = src[i].y;
	    r[i].type = src[i].type;
	    r[i].ks = new double[] { 0., 0., 0., 0. };
	}
	r[n_seg] = new spiro_seg();
	r[n_seg].x = src[n_seg % n].x;
	r[n_seg].y = src[n_seg % n].y;
	r[n_seg].type = src[n_seg % n].type;

	for (i = 0; i < n_seg; i++) {
	    double dx = r[i + 1].x - r[i].x;
	    double dy = r[i + 1].y - r[i].y;
	    r[i].seg_ch = Math.hypot(dx, dy);
	    r[i].seg_th = Math.atan2(dy, dx);
	}

	ilast = n_seg - 1;
	for (i = 0; i < n_seg; i++) {
	    if (r[i].type == OPEN || r[i].type == OPEN_END || r[i].type == CORNER)
		r[i].bend_th = 0.;
	    else
		r[i].bend_th = mod_2pi(r[i].seg_th - r[ilast].seg_th);
	    ilast = i;
	}
	return r;
    }

    static protected void
      bandec11(bandmat [] m, int [] perm, int n) {
	int i, j, k;
	int l;

	/* pack top triangle to the LEFT. */
	for (i = 0; i < 5; i++) {
	    for (j = 0; j < i + 6; j++)
		m[i].a[j] = m[i].a[j + 5 - i];
	    for (; j < 11; j++)
		m[i].a[j] = 0.;
	}
	l = 5;
	for (k = 0; k < n; k++) {
	    int pivot = k;
	    double pivot_val = m[k].a[0];
	    double pivot_scale;

	    l = l < n ? l + 1 : n;

	    for (j = k + 1; j < l; j++)
		if (Math.abs(m[j].a[0]) > Math.abs(pivot_val)) {
		    pivot_val = m[j].a[0];
		    pivot = j;
		}

	    perm[k] = pivot;
	    if (pivot != k) {
		for (j = 0; j < 11; j++) {
		    double tmp = m[k].a[j];
		    m[k].a[j] = m[pivot].a[j];
		    m[pivot].a[j] = tmp;
		}
	    }

	    if (Math.abs(pivot_val) < 1e-12) pivot_val = 1e-12;
	    pivot_scale = 1. / pivot_val;
	    for (i = k + 1; i < l; i++) {
		double x = m[i].a[0] * pivot_scale;
		m[k].al[i - k - 1] = x;
		for (j = 1; j < 11; j++)
		    m[i].a[j - 1] = m[i].a[j] - x * m[k].a[j];
		m[i].a[10] = 0.;
	    }
	}
    }

    static protected void
      banbks11(final bandmat [] m, final int [] perm, double [] v, int n) {
	int i, k, l;

	/* forward substitution */
	l = 5;
	for (k = 0; k < n; k++) {
	    i = perm[k];
	    if (i != k) {
		double tmp = v[k];
		v[k] = v[i];
		v[i] = tmp;
	    }
	    if (l < n) l++;
	    for (i = k + 1; i < l; i++)
		v[i] -= m[k].al[i - k - 1] * v[k];
	}

	/* back substitution */
	l = 1;
	for (i = n - 1; i >= 0; i--) {
	    double x = v[i];
	    for (k = 1; k < l; k++)
		x -= m[i].a[k] * v[k + i];
	    v[i] = x / m[i].a[0];
	    if (l < 11) l++;
	}
    }

    static protected int
      compute_jinc(SpiroPointType ty0, SpiroPointType ty1) {
	if (ty0 == G4 || ty1 == G4 ||
	    ty0 == RIGHT || ty1 == LEFT)
	    return 4;
	else if (ty0 == G2 && ty1 == G2)
	    return 2;
	else if (((ty0 == OPEN || ty0 == CORNER || ty0 == LEFT) && ty1 == G2) ||
		 (ty0 == G2 && (ty1 == OPEN_END || ty1 == CORNER || ty1 == RIGHT)))
	    return 1;
	else
	    return 0;
    }

    static protected int
      count_vec(final spiro_seg [] s, int nseg) {
	int i;
	int n = 0;

	for (i = 0; i < nseg; i++)
	    n += compute_jinc(s[i].type, s[i + 1].type);
	return n;
    }

    static protected void
      add_mat_line(bandmat [] m, double [] v,
		 double [/*4*/] derivs, double x, double y, int j, int jj, int jinc,
		 int nmat) {
	int k;

	if (jj >= 0) {
	    int joff =  (j + 5 - jj + nmat) % nmat;
	    if (nmat < 6) {
		joff = j + 5 - jj;
	    } else if (nmat == 6) {
		joff = 2 + (j + 3 - jj + nmat) % nmat;
	    }
	    v[jj] += x;
	    for (k = 0; k < jinc; k++)
		m[jj].a[joff + k] += y * derivs[k];
	}
    }

    static protected double
      spiro_iter(spiro_seg [] s, bandmat [] m, int [] perm, double [] v, int n) {
	boolean cyclic = s[0].type != OPEN && s[0].type != CORNER;
	int i, j, jj;
	int nmat = count_vec(s, n);
	double norm;
	int n_invert;

	for (i = 0; i < nmat; i++) {
	    v[i] = 0.;
	    m[i] = new bandmat();
	    m[i].a  = new double[] { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	    m[i].al = new double[] { 0, 0, 0, 0, 0 };
	}

	j = 0;
	if (s[0].type == G4)
	    jj = nmat - 2;
	else if (s[0].type == G2)
	    jj = nmat - 1;
	else
	    jj = 0;
	for (i = 0; i < n; i++) {
	    SpiroPointType ty0 = s[i].type;
	    SpiroPointType ty1 = s[i + 1].type;
	    int jinc = compute_jinc(ty0, ty1);
	    double th = s[i].bend_th;
	    double [][] ends = new double[2][4];
	    double [][][] derivs = new double[4][2][4];
	    int jthl = -1, jk0l = -1, jk1l = -1, jk2l = -1;
	    int jthr = -1, jk0r = -1, jk1r = -1, jk2r = -1;

	    compute_pderivs(s[i], ends, derivs, jinc);

	    /* constraints crossing LEFT */
	    if (ty0 == G4 || ty0 == G2 || ty0 == LEFT || ty0 == RIGHT) {
		jthl = jj++;
		jj %= nmat;
		jk0l = jj++;
	    }
	    if (ty0 == G4) {
		jj %= nmat;
		jk1l = jj++;
		jk2l = jj++;
	    }

	    /* constraints on LEFT */
	    if ((ty0 == LEFT || ty0 == CORNER || ty0 == OPEN || ty0 == G2) &&
		jinc == 4) {
		if (ty0 != G2)
		    jk1l = jj++;
		jk2l = jj++;
	    }

	    /* constraints on RIGHT */
	    if ((ty1 == RIGHT || ty1 == CORNER || ty1 == OPEN_END || ty1 == G2) &&
		jinc == 4) {
		if (ty1 != G2)
		    jk1r = jj++;
		jk2r = jj++;
	    }

	    /* constraints crossing RIGHT */
	    if (ty1 == G4 || ty1 == G2 || ty1 == LEFT || ty1 == RIGHT) {
		jthr = jj;
		jk0r = (jj + 1) % nmat;
	    }
	    if (ty1 == G4) {
		jk1r = (jj + 2) % nmat;
		jk2r = (jj + 3) % nmat;
	    }

	    add_mat_line(m, v, derivs[0][0], th - ends[0][0], 1, j, jthl, jinc, nmat);
	    add_mat_line(m, v, derivs[1][0], ends[0][1], -1, j, jk0l, jinc, nmat);
	    add_mat_line(m, v, derivs[2][0], ends[0][2], -1, j, jk1l, jinc, nmat);
	    add_mat_line(m, v, derivs[3][0], ends[0][3], -1, j, jk2l, jinc, nmat);
	    add_mat_line(m, v, derivs[0][1], -ends[1][0], 1, j, jthr, jinc, nmat);
	    add_mat_line(m, v, derivs[1][1], -ends[1][1], 1, j, jk0r, jinc, nmat);
	    add_mat_line(m, v, derivs[2][1], -ends[1][2], 1, j, jk1r, jinc, nmat);
	    add_mat_line(m, v, derivs[3][1], -ends[1][3], 1, j, jk2r, jinc, nmat);
	    j += jinc;
	}
	if (cyclic) {
	    bandmat.arraycopy(m, 0, m,     nmat, nmat );
	    bandmat.arraycopy(m, 0, m, 2 * nmat, nmat );
	    System.arraycopy(v, 0, v,     nmat, nmat);
	    System.arraycopy(v, 0, v, 2 * nmat, nmat);
	    n_invert = 3 * nmat;
	    j = nmat;
	} else {
	    n_invert = nmat;
	    j = 0;
	}
	bandec11(m, perm, n_invert);
	banbks11(m, perm, v, n_invert);
	norm = 0.;
	for (i = 0; i < n; i++) {
	    SpiroPointType ty0 = s[i].type;
	    SpiroPointType ty1 = s[i + 1].type;
	    int jinc = compute_jinc(ty0, ty1);
	    int k;
    
	    for (k = 0; k < jinc; k++) {
		double dk = v[j++];
		s[i].ks[k] += dk;
		norm += dk * dk;
	    }
	}
	return norm;
    }

    static protected int
      solve_spiro(spiro_seg [] s, int nseg) {
	bandmat [] m;
	double [] v;
	int [] perm;
	int nmat = count_vec(s, nseg);
	int n_alloc = nmat;
	double norm;
	int i;

	if (nmat == 0)
	    return 0;
	if (s[0].type != OPEN && s[0].type != CORNER)
	    n_alloc *= 3;
	if (n_alloc < 5)
	    n_alloc = 5;
	m = new bandmat[n_alloc];
	v = new double [n_alloc];
	perm = new int [n_alloc];

	for (i = 0; i < 10; i++) {
	    norm = spiro_iter(s, m, perm, v, nseg);
	    if (norm < 1e-12) break;
	}

	return 0;
    }

    static protected void
      spiro_seg_to_bpath(final double [/*4*/] ks,
		       double x0, double y0, double x1, double y1,
		       SpiroBezierContext bc, int depth) {
	double bend = Math.abs(ks[0]) + Math.abs(.5 * ks[1]) + Math.abs(.125 * ks[2]) +
	    Math.abs((1./48) * ks[3]);

	if ( bend <= 1e-8) {
	    bc.LineTo(x1, y1);
	} else {
	    double seg_ch = Math.hypot(x1 - x0, y1 - y0);
	    double seg_th = Math.atan2(y1 - y0, x1 - x0);
	    double [] xy = new double[2];
	    double ch, th;
	    double scale, rot;
	    double th_even, th_odd;
	    double ul, vl;
	    double ur, vr;
    
	    integrate_spiro(ks, xy);
	    ch = Math.hypot(xy[0], xy[1]);
	    th = Math.atan2(xy[1], xy[0]);
	    scale = seg_ch / ch;
	    rot = seg_th - th;
	    if (depth > 5 || bend < 1.) {
		th_even = (1./384) * ks[3] + (1./8) * ks[1] + rot;
		th_odd = (1./48) * ks[2] + .5 * ks[0];
		ul = (scale * (1./3)) * Math.cos(th_even - th_odd);
		vl = (scale * (1./3)) * Math.sin(th_even - th_odd);
		ur = (scale * (1./3)) * Math.cos(th_even + th_odd);
		vr = (scale * (1./3)) * Math.sin(th_even + th_odd);
		bc.CubicTo(x0 + ul, y0 + vl, x1 - ur, y1 - vr, x1, y1);
	    } else {
		/* subdivide */
		double [] ksub = new double[4];
		double thsub;
		double [] xysub = new double[2];
		double xmid, ymid;
		double cth, sth;
    
		ksub[0] = .5 * ks[0] - .125 * ks[1] + (1./64) * ks[2] - (1./768) * ks[3];
		ksub[1] = .25 * ks[1] - (1./16) * ks[2] + (1./128) * ks[3];
		ksub[2] = .125 * ks[2] - (1./32) * ks[3];
		ksub[3] = (1./16) * ks[3];
		thsub = rot - .25 * ks[0] + (1./32) * ks[1] - (1./384) * ks[2] + (1./6144) * ks[3];
		cth = .5 * scale * Math.cos(thsub);
		sth = .5 * scale * Math.sin(thsub);
		integrate_spiro(ksub, xysub);
		xmid = x0 + cth * xysub[0] - sth * xysub[1];
		ymid = y0 + cth * xysub[1] + sth * xysub[0];
		spiro_seg_to_bpath(ksub, x0, y0, xmid, ymid, bc, depth + 1);
		ksub[0] += .25 * ks[1] + (1./384) * ks[3];
		ksub[1] += .125 * ks[2];
		ksub[2] += (1./16) * ks[3];
		spiro_seg_to_bpath(ksub, xmid, ymid, x1, y1, bc, depth + 1);
	    }
	}
    }

    static protected spiro_seg []
      run_spiro(final SpiroCP []src, int n) {
	int nseg = src[0].type == OPEN ? n - 1 : n;
	spiro_seg [] s = setup_path(src, n);
	if (nseg > 1)
	    solve_spiro(s, nseg);
	return s;
    }

    static protected void
      spiro_to_bpath(final spiro_seg []s, int n, SpiroBezierContext bc) {
	int i;
	int nsegs = s[n - 1].type == OPEN_END ? n - 1 : n;

	for (i = 0; i < nsegs; i++) {
	    double x0 = s[i].x;
	    double y0 = s[i].y;
	    double x1 = s[i + 1].x;
	    double y1 = s[i + 1].y;

	    if (i == 0)
		bc.MoveTo(x0, y0, s[0].type == OPEN);
	    bc.MarkKnot(i);
	    spiro_seg_to_bpath(s[i].ks, x0, y0, x1, y1, bc, 0);
	}
    }

    static protected double
      get_knot_th(final spiro_seg []s, int i) {
	double [][] ends = new double[2][4];

	if (i == 0) {
	    compute_ends(s[i].ks, ends, s[i].seg_ch);
	    return s[i].seg_th - ends[0][0];
	} else {
	    compute_ends(s[i - 1].ks, ends, s[i - 1].seg_ch);
	    return s[i - 1].seg_th + ends[1][0];
	}
    }

 /* ************************************************************************* */
 /* *************************** public interface **************************** */
 /* ************************************************************************* */

    static public void
      SpiroCPsToBezier(SpiroCP [] spiros,int n,boolean isclosed,SpiroBezierContext bc) {
	spiro_seg []s;

	if ( n<1 )
    return;
	if ( !isclosed ) {
	    SpiroPointType oldty_start = spiros[0].type;
	    SpiroPointType oldty_end   = spiros[n-1].type;
	    spiros[0].type= OPEN;
	    spiros[n-1].type= OPEN_END;
	    s = run_spiro(spiros,n);
	    spiros[n-1].type= oldty_end;
	    spiros[0].type= oldty_start;
	} else
	    s = run_spiro(spiros,n);
	spiro_to_bpath(s,n,bc);
    }

    static public void
      TaggedSpiroCPsToBezier(SpiroCP [] spiros,SpiroBezierContext bc) {
	spiro_seg []s;
	int n;

	for ( n=0; spiros[n].type!=END && spiros[n].type!=OPEN_END; ++n );
	if ( spiros[n].type == OPEN_END ) ++n;

	if ( n<1 )
    return;
	s = run_spiro(spiros,n);
	spiro_to_bpath(s,n,bc);
    }

 /* ************************************************************************* */
 /* ***************************** plate file IO ***************************** */
 /* ************************************************************************* */
    static public void
      SavePlateFile(Writer output,SpiroCP [][] spirocontours) throws IOException {
        int i,j;

	output.write("(plate\n");
	for ( i=0; i<spirocontours.length; ++i ) {
	    SpiroCP [] spiros = spirocontours[i];
	    for ( j=0; j<spiros.length; ++j ) {
		if ( spiros[j].type==END )
	    break;
		output.write("  " + spiros[j].toString() + "\n" );
	    }
	    if ( spiros.length>0 && spiros[0].type!=OPEN )
		output.write("  (z)\n");
	}
	output.write(")\n");
    }

    static public SpiroCP [][]
      ReadPlateFile(BufferedReader input) throws IOException {
	ArrayList<SpiroCP []> sofar;
	ArrayList<SpiroCP> contour;
	int i,j;
	String str;
	char ch;
	SpiroPointType pt;
	double x,y;

	str = input.readLine();
	if ( str==null )
    return( null );		/* EOF already? */
	if ( !str.trim().equals("(plate") ) {
	    // Doesn't look like a plate file
    return( null );
	}

	sofar = new ArrayList<SpiroCP []>();
	contour = new ArrayList<SpiroCP>();
	while ( true ) {
	    while( true ) {
		str = input.readLine();
		if ( str==null )
	    break;
		str = str.trim();
		if ( str.equals(")") || str.equals("(z)") )
	    break;
		if ( str.charAt(0)!='(' )
    return( null );
		for ( i=1; i<str.length() && str.charAt(i)==' ' ; ++i );
		if ( i>=str.length())
    return( null );
		ch = str.charAt(i);
		if ( ch=='v' )
		    pt = CORNER;
		else if ( ch=='o' )
		    pt = G4;
		else if ( ch=='c' )
		    pt = G2;
		else if ( ch=='[' )
		    pt = LEFT;
		else if ( ch==']' )
		    pt = RIGHT;
		else if ( ch=='{' )
		    pt = OPEN;
		else if ( ch=='}' )
		    pt = OPEN_END;
		else
    return( null );
		for ( ++i ; i<str.length() && str.charAt(i)==' ' ; ++i );
		for ( j=i ; j<str.length() && str.charAt(j)!=' ' ; ++j );
		if ( j>=str.length() )
    return( null );
		try {
		    x = Double.parseDouble(str.substring(i,j-1));
		} catch ( NumberFormatException e ) {
    return( null );
		}
		for ( i=j+1 ; i<str.length() && str.charAt(i)==' ' ; ++i );
		for ( j=i ; j<str.length() && str.charAt(j)!=' ' && str.charAt(j)!=')'; ++j );
		if ( j>=str.length() )
    return( null );
		try {
		    y = Double.parseDouble(str.substring(i,j-1));
		} catch ( NumberFormatException e ) {
    return( null );
		}
		contour.add( new SpiroCP(x,y,pt));
	    }
	    contour.add( new SpiroCP(0,0,END));
	    sofar.add( contour.toArray(new SpiroCP[contour.size()]));
	    contour.clear();
	    if ( str==null || str.equals(")") )
	break;
	}
    return ( sofar.toArray( new SpiroCP [sofar.size()][] ) );
    }
}
