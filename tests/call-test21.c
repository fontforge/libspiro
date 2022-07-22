/* call-test21.c, 2022jul22, libspiro-20220722
This test is an example for SPIRO_INTERNAL_BEZCTX & functions
*/
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "spiroentrypoints.h"	/* call spiro through here */
#include "bezctx.h"		/* bezctx structure */

static void load_test_curve(spiro_cp *spiro, int c) {
/* load a test curve (and nextknot locations) into memory. */
    spiro_cp path0[] = { /* this data came with unit-test. */
	{334, 117, 'v'},
	{305, 176, 'v'},
	{212, 142, 'c'},
	{159, 171, 'c'},
	{224, 237, 'c'},
	{347, 335, 'c'},
	{202, 467, 'c'},
	{ 81, 429, 'v'},
	{114, 368, 'v'},
	{201, 402, 'c'},
	{276, 369, 'c'},
	{218, 308, 'c'},
	{ 91, 211, 'c'},
	{124, 111, 'c'},
	{229,  82, 'c'},
	{0, 0, 'z'}
    };
    spiro_cp path6[] = { /* verify curve data with ah. */
	{  0,   0, '{'},
	{100, 100, 'c'},
	{200, 200, 'a'},
	{300, 200, 'h'},
	{300, 150, 'c'},
	{200, 100, 'a'},
	{100, 100, 'h'},
	{150,  50, 'c'},
	{100,   0, 'a'},
	{  0,-100, 'h'},
	{ 50,-100, 'c'},
	{ 20,-150, '}'}
    };

    int i;
    if (c == 0) for (i = 0; i < 16; i++) {
	spiro[i].x = path0[i].x;
	spiro[i].y = path0[i].y;
	spiro[i].ty = path0[i].ty;
    }
    if (c == 1) {
	for (i = 0; i < 12; i++) {
	    spiro[i].x = path6[i].x;
	    spiro[i].y = path6[i].y;
	    spiro[i].ty = path6[i].ty;
	}
	for (i = 12; i < 16; i++) {
	    spiro[i].x = spiro[i].y = 0.0; spiro[i].ty = 0;
	}
    }
}

static int verify_results(ls_bezctx *bc, int c) {
    /* These are the expected cubic curve results */
    curve_data expect0[] = {
	{'m', 0, 0, 334, 117, 0, 0, 0, 0},
	{'k', 334, 117, 0, 0, 0, 0, 0, 0},
	{'l', 334, 117, 305, 176, 0, 0, 0, 0},
	{'k', 305, 176, 0, 0, 0, 0, 0, 0},
	{'c', 305, 176, 279.449, 153.974, 245.733, 141.648, 212, 142},
	{'k', 212, 142, 0, 0, 0, 0, 0, 0},
	{'c', 212, 142, 201.226, 142.112, 190.267, 143.529, 180.511, 148.103},
	{'c', 180.511, 148.103, 175.633, 150.39, 171.099, 153.461, 167.348, 157.327},
	{'c', 167.348, 157.327, 163.596, 161.194, 160.641, 165.869, 159, 171},
	{'k', 159, 171, 0, 0, 0, 0, 0, 0},
	{'c', 159, 171, 156.364, 179.243, 157.251, 188.403, 160.654, 196.36},
	{'c', 160.654, 196.36, 164.057, 204.316, 169.853, 211.112, 176.664, 216.45},
	{'c', 176.664, 216.45, 190.287, 227.126, 207.444, 231.953, 224, 237},
	{'k', 224, 237, 0, 0, 0, 0, 0, 0},
	{'c', 224, 237, 250.322, 245.025, 276.649, 254.369, 299.324, 269.96},
	{'c', 299.324, 269.96, 321.998, 285.551, 340.921, 308.162, 347, 335},
	{'k', 347, 335, 0, 0, 0, 0, 0, 0},
	{'c', 347, 335, 351.166, 353.392, 349.066, 373.009, 341.95, 390.473},
	{'c', 341.95, 390.473, 334.833, 407.936, 322.824, 423.251, 308.134, 435.075},
	{'c', 308.134, 435.075, 278.752, 458.724, 239.706, 467.893, 202, 467},
	{'k', 202, 467, 0, 0, 0, 0, 0, 0},
	{'c', 202, 467, 159.176, 465.986, 116.715, 452.651, 81, 429},
	{'k', 81, 429, 0, 0, 0, 0, 0, 0},
	{'l', 81, 429, 114, 368, 0, 0, 0, 0},
	{'k', 114, 368, 0, 0, 0, 0, 0, 0},
	{'c', 114, 368, 140.251, 385.152, 170.075, 396.808, 201, 402},
	{'k', 201, 402, 0, 0, 0, 0, 0, 0},
	{'c', 201, 402, 216.006, 404.519, 231.742, 405.476, 246.158, 400.607},
	{'c', 246.158, 400.607, 253.366, 398.172, 260.137, 394.269, 265.488, 388.861},
	{'c', 265.488, 388.861, 270.839, 383.452, 274.72, 376.5, 276, 369},
	{'k', 276, 369, 0, 0, 0, 0, 0, 0},
	{'c', 276, 369, 277.287, 361.46, 275.914, 353.582, 272.701, 346.641},
	{'c', 272.701, 346.641, 269.488, 339.699, 264.498, 333.665, 258.672, 328.709},
	{'c', 258.672, 328.709, 247.02, 318.796, 232.367, 313.255, 218, 308},
	{'k', 218, 308, 0, 0, 0, 0, 0, 0},
	{'c', 218, 308, 191.978, 298.481, 165.57, 289.162, 142.279, 274.152},
	{'c', 142.279, 274.152, 118.988, 259.143, 98.7273, 237.609, 91, 211},
	{'k', 91, 211, 0, 0, 0, 0, 0, 0},
	{'c', 91, 211, 85.821, 193.166, 86.5807, 173.749, 92.5866, 156.176},
	{'c', 92.5866, 156.176, 98.5925, 138.603, 109.749, 122.908, 124, 111},
	{'k', 124, 111, 0, 0, 0, 0, 0, 0},
	{'c', 124, 111, 138.28, 99.0677, 155.539, 90.9339, 173.578, 86.3634},
	{'c', 173.578, 86.3634, 191.617, 81.7928, 210.435, 80.7131, 229, 82},
	{'k', 229, 82, 0, 0, 0, 0, 0, 0},
	{'c', 229, 82, 266.199, 84.5786, 302.694, 96.7435, 334, 117}
    };
    /* These are the expected quadratic curve results */
    curve_data expect6[] = {
	{'m', 0, 0, 0, 0, 0, 0, 0, 0},
	{'k', 0, 0, 0, 0, 0, 0, 0, 0},
	{'q', 0, 0, 23.3715, 7.35678, 62.2475, 37.7525, 0, 0},
	{'q', 62.2475, 37.7525, 92.6432, 76.6285, 100, 100, 0, 0},
	{'k', 100, 100, 0, 0, 0, 0, 0, 0},
	{'q', 100, 100, 101.992, 106.327, 105.137, 119.22, 0, 0},
	{'q', 105.137, 119.22, 108.262, 132.117, 110.182, 138.466, 0, 0},
	{'q', 110.182, 138.466, 112.101, 144.815, 117.302, 157.024, 0, 0},
	{'q', 117.302, 157.024, 123.765, 168.643, 128.019, 173.733, 0, 0},
	{'q', 128.019, 173.733, 132.272, 178.822, 142.741, 187.019, 0, 0},
	{'q', 142.741, 187.019, 154.306, 193.575, 160.624, 195.595, 0, 0},
	{'q', 160.624, 195.595, 166.942, 197.614, 180.118, 199.382, 0, 0},
	{'q', 180.118, 199.382, 193.367, 200, 200, 200, 0, 0},
	{'k', 200, 200, 0, 0, 0, 0, 0, 0},
	{'q', 200, 200, 210.377, 200, 231.117, 199.326, 0, 0},
	{'q', 231.117, 199.326, 251.834, 197.146, 261.683, 193.879, 0, 0},
	{'q', 261.683, 193.879, 266.607, 192.246, 275.86, 187.525, 0, 0},
	{'q', 275.86, 187.525, 284.483, 181.698, 288.06, 177.939, 0, 0},
	{'q', 288.06, 177.939, 291.636, 174.181, 296.725, 165.102, 0, 0},
	{'q', 296.725, 165.102, 300, 155.188, 300, 150, 0, 0},
	{'k', 300, 150, 0, 0, 0, 0, 0, 0},
	{'q', 300, 150, 300, 144.812, 296.725, 134.898, 0, 0},
	{'q', 296.725, 134.898, 291.636, 125.819, 288.06, 122.061, 0, 0},
	{'q', 288.06, 122.061, 284.483, 118.302, 275.86, 112.475, 0, 0},
	{'q', 275.86, 112.475, 266.607, 107.754, 261.683, 106.121, 0, 0},
	{'q', 261.683, 106.121, 251.834, 102.854, 231.117, 100.674, 0, 0},
	{'q', 231.117, 100.674, 210.377, 100, 200, 100, 0, 0},
	{'k', 200, 100, 0, 0, 0, 0, 0, 0},
	{'q', 200, 100, 196.719, 100, 190.166, 99.6669, 0, 0},
	{'q', 190.166, 99.6669, 183.656, 98.7176, 180.555, 97.6447, 0, 0},
	{'q', 180.555, 97.6447, 177.454, 96.5719, 171.848, 93.1288, 0, 0},
	{'q', 171.848, 93.1288, 166.838, 88.8674, 164.84, 86.2641, 0, 0},
	{'q', 164.84, 86.2641, 162.843, 83.6608, 159.844, 77.8069, 0, 0},
	{'q', 159.844, 77.8069, 157.408, 71.7127, 156.434, 68.579, 0, 0},
	{'q', 156.434, 68.579, 155.461, 65.4454, 153.593, 59.1539, 0, 0},
	{'q', 153.593, 59.1539, 151.474, 52.9318, 150, 50, 0, 0},
	{'k', 150, 50, 0, 0, 0, 0, 0, 0},
	{'q', 150, 50, 147.34, 44.7095, 139.446, 35.7988, 0, 0},
	{'q', 139.446, 35.7988, 130.781, 27.7291, 126.249, 23.9179, 0, 0},
	{'q', 126.249, 23.9179, 121.717, 20.1067, 112.792, 12.3232, 0, 0},
	{'q', 112.792, 12.3232, 104.187, 4.18708, 100, 0, 0, 0},
	{'k', 100, 0, 0, 0, 0, 0, 0, 0},
	{'q', 100, 0, 93.2929, -6.70709, 80.9277, -21.0842, 0, 0},
	{'q', 80.9277, -21.0842, 70.4319, -36.9236, 66.6872, -45.6384, 0, 0},
	{'q', 66.6872, -45.6384, 62.9425, -54.3532, 57.7639, -72.6315, 0, 0},
	{'q', 57.7639, -72.6315, 53.1047, -91.0372, 50, -100, 0, 0},
	{'k', 50, -100, 0, 0, 0, 0, 0, 0},
	{'q', 50, -100, 46.7962, -109.249, 37.5962, -126.558, 0, 0},
	{'q', 37.5962, -126.558, 26.653, -142.821, 20, -150, 0, 0}
    };
    int i,j,k,ret;

    ret = 1;
    if ( c == 0 && bc->l == 45 ) {
	printf("test: verify results[0..%d] vs expected values[0..44]\n",bc->l - 1);
	ret = 0;
	for (i=0; i < bc->l && ret == 0; i++) {
	    if ( bc->cd[i].ty != expect0[i].ty ) ret = 1;
	    /* not integers! test approximate equivalents. */
	    if ( fabs(bc->cd[i].x0 - expect0[i].x0) > 0.0005 ) ret = 1;
	    if ( fabs(bc->cd[i].y0 - expect0[i].y0) > 0.0005 ) ret = 1;
	    if ( fabs(bc->cd[i].x1 - expect0[i].x1) > 0.0005 ) ret = 1;
	    if ( fabs(bc->cd[i].y1 - expect0[i].y1) > 0.0005 ) ret = 1;
	    if ( fabs(bc->cd[i].x2 - expect0[i].x2) > 0.0005 ) ret = 1;
	    if ( fabs(bc->cd[i].y2 - expect0[i].y2) > 0.0005 ) ret = 1;
	    if ( fabs(bc->cd[i].x3 - expect0[i].x3) > 0.0005 ) ret = 1;
	    if ( fabs(bc->cd[i].y3 - expect0[i].y3) > 0.0005 ) ret = 1;
	}
    }
    if ( c == 1 && bc->l == 49 ) {
	printf("test: verify results[0..%d] vs expected values[0..48]\n",bc->l - 1);
	ret = 0;
	for (i=0; i < bc->l && ret == 0; i++) {
	    if ( bc->cd[i].ty != expect6[i].ty ) ret = 1;
	    /* not integers! test approximate equivalents. */
	    if ( fabs(bc->cd[i].x0 - expect6[i].x0) > 0.0005 ) ret = 1;
	    if ( fabs(bc->cd[i].y0 - expect6[i].y0) > 0.0005 ) ret = 1;
	    if ( fabs(bc->cd[i].x1 - expect6[i].x1) > 0.0005 ) ret = 1;
	    if ( fabs(bc->cd[i].y1 - expect6[i].y1) > 0.0005 ) ret = 1;
	    if ( fabs(bc->cd[i].x2 - expect6[i].x2) > 0.0005 ) ret = 1;
	    if ( fabs(bc->cd[i].y2 - expect6[i].y2) > 0.0005 ) ret = 1;
	    if ( fabs(bc->cd[i].x3 - expect6[i].x3) > 0.0005 ) ret = 1;
	    if ( fabs(bc->cd[i].y3 - expect6[i].y3) > 0.0005 ) ret = 1;
	}
    }
    return ret;
}

static int show_results(ls_bezctx *bc) {
    int i,ret;

    ret = 0;
    for (i=0; i < bc->l || ret; i++) {
	printf("%d: (t=%c x0=%g y0=%g",i,bc->cd[i].ty,bc->cd[i].x0,bc->cd[i].y0);
	switch (bc->cd[i].ty) {
	case 'k':
	    printf(")\n");
	    break;
	case 'l':
	case 'm':
	    printf(" x1=%g y1=%g)\n",bc->cd[i].x1,bc->cd[i].y1);
	    break;
	case 'q':
	    printf(" x1=%g y1=%g x2=%g y2=%g)\n", \
		   bc->cd[i].x1,bc->cd[i].y1,bc->cd[i].x2,bc->cd[i].y2);
	    break;
	case 'c':
	    printf(" x1=%g y1=%g x2=%g y2=%g x3=%g y3=%g)\n", \
		   bc->cd[i].x1,bc->cd[i].y1,bc->cd[i].x2,bc->cd[i].y2, \
		   bc->cd[i].x3,bc->cd[i].y3);
	    break;
	default:
	    printf(" -> error with type!!\n");
	    ret = -1;
	}
    }
    return ret;
}

int main(int argc, char **argv) {
    spiro_cp spiro[16];
    ls_bezctx *bc;
    int i,ncq,max,ret;

    load_test_curve(spiro, 0);

    ncq = SPIRO_CUBIC_TO_BEZIER|SPIRO_INTERNAL_BEZCTX;
    max = 16 * 10; /* Oversize approximate 10x spiro size. */
    bc = new_ls_bezctx(max, ncq);
    if ( bc == NULL ) {
	printf("error: new_ls_bezctx().\n");
	return -1;
    }

    printf("make: cubic curve using test0 spiro array\n");
    ret = TaggedSpiroCPsToBezier2(spiro,ncq,(bezctx *)(bc));
    if ( ret==0 ) {
	printf("error: TaggedSpiroCPsToBezier2() failed to build cubic curve.\n");
	free_ls_bezctx(bc);
	return -2;
    }
    printf("show: resulting cubic curve using test0 spiro array\n");
    ret = show_results(bc);
    if ( ret ) {
	printf("error: show_results() had wrong curve type.\n");
	free_ls_bezctx(bc);
	return -2;
    }
    ret = verify_results(bc, 0);
    if ( ret ) {
	printf("error: verify_results() did not match with expected values.\n");
	free_ls_bezctx(bc);
	return -3;
    }

    /* Resize/release/keep old array, or start a new array
     * curve_data *tmp = (curve_data *)realloc(bc->cd,(bc->l)*sizeof(curve_data));
     * free_ls_bezctx(bc);		// old array
     * bc = new_ls_bezctx(max, ncq);	// fresh new array

    /* Reuse the result array again for quadratics instead */
    load_test_curve(spiro, 1);
    ncq = SPIRO_QUAD0_TO_BEZIER|SPIRO_INTERNAL_BEZCTX;
    bc->l = 0; /* reset starting length as 0 */

    printf("make: quadratic curve using test0 spiro array\n");
    ret = TaggedSpiroCPsToBezier2(spiro,ncq,(bezctx *)(bc));
    if ( ret==0 ) {
	printf("error: TaggedSpiroCPsToBezier2() failed to build quadratic curve.\n");
	free_ls_bezctx(bc);
	return -4;
    }
    printf("show: resulting quadratic curve using test0 spiro array\n");
    ret = show_results(bc);
    if ( ret ) {
	printf("error: show_results() had wrong curve type.\n");
	free_ls_bezctx(bc);
	return -5;
    }
    ret = verify_results(bc, 1);
    if ( ret ) {
	printf("error: verify_results() did not match with expected values.\n");
	free_ls_bezctx(bc);
	return -6;
    }
    printf("pass: results matches expected values\n");

    free_ls_bezctx(bc);

    printf("done.\n");
    return 0;
}