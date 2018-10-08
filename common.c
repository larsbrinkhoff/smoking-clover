#include "clover.h"

#ifndef MIN
#define	MIN(x, y)	((x) < (y))?(x):(y)
#define	MAX(x, y)	((x) > (y))?(x):(y)
#endif
#define	CEIL(a, b)	((a)+(b)-1)/(b)
#define	FLOOR(a, b)	CEIL((a)-(b), (b))
void clipLine(int x0, int y0, int xn, int yn, int xe, int ye, int xf, int yf);
void line(int fun, int x0, int y0, int dx, int dy,
	  int xe, int ye, int xf, int yf);

#define	get_count(x, y)		(counts[(y)*width+(x)])
#define	put_count(x, y, v)	(counts[(y)*width+(x)] = (v))

int line_x, line_y, line_f;
int radius = 5432;
int dup_x;

struct colors *colors_tmp;

double
rnd (double lo, double hi)
{
	static char state[8];
	static int beenhere;
	char *old;
	int seed;
	double r;

	if (beenhere == 0) {
		beenhere = 1;

		seed = 1;
		old = initstate (seed, state, sizeof state);
		setstate (old);
	}

	old = setstate (state);
	r = random ();
	setstate (old);

	return ((r / RAND_MAX) * (hi - lo) + lo);
}

void
make_colors (void)
{
	int i;

	colors = calloc (ncolors, sizeof *colors);
	colors_tmp = calloc (ncolors, sizeof *colors);

	for (i = 0; i < ncolors; i++) {
		colors[i].r = (double)random () / RAND_MAX;
		colors[i].g = (double)random () / RAND_MAX;
		colors[i].b = (double)random () / RAND_MAX;
	}
	colors[0].r = 0;
	colors[0].g = 0;
	colors[0].b = 0;
}

void
clear_colors ()
{
	int i;
	for (i = 0; i < ncolors; i++) {
		colors[i].r = 0;
		colors[i].g = 0;
		colors[i].b = 0;
	}
}

void
rotate_colors (int steps)
{
	if (steps >= ncolors)
		return;

	memcpy (&colors_tmp[0], &colors[steps],
		(ncolors - steps) * sizeof colors_tmp[0]);
	memcpy (&colors_tmp[ncolors - steps], &colors[0],
		steps * sizeof colors_tmp[0]);

	memcpy (colors, colors_tmp, ncolors * sizeof *colors);
}

struct cstate {
	double cur;
	double vel;
};

void
next_color (int steps)
{
	static struct cstate cstates[3];
	static int beenhere;
	int i;
	struct cstate *cp;
	int step;
	double maxvel, minvel;

	minvel = .003;
	maxvel = .15;
	if (beenhere == 0) {
		beenhere = 1;
		cstates[0].vel = rnd (minvel, maxvel);
		cstates[1].vel = rnd (minvel, maxvel);
		cstates[2].vel = rnd (minvel, maxvel);
	}

	for (step = 0; step < steps; step++) {
		for (i = 0, cp = cstates; i < 3; i++, cp++) {
			cp->cur += cp->vel;
			if (cp->cur > 1) {
				cp->cur = 1;
				cp->vel = rnd (-maxvel, -minvel);
			}
			if (cp->cur < 0) {
				cp->cur = 0;
				cp->vel = rnd (minvel, maxvel);
			}
		}
		
		rotate_colors (1);
		colors[ncolors-1].r = cstates[0].cur;
		colors[ncolors-1].g = cstates[1].cur;
		colors[ncolors-1].b = cstates[2].cur;
	}
}

void
set_size (int w, int h)
{
	width = w;
	height = h;
	maxX = width - 1;
	maxY = height - 1;
	midX = maxX / 2;
	midY = maxY / 2;

	if (counts)
		free (counts);
	counts = calloc (width * height, sizeof *counts);

	lines_state = s_lines;
	line_x = radius;
	line_y = 0;
	line_f = 0;
}

void
redraw_lines (void)
{
	int nsteps, step;

	nsteps = 500;
	for (step = 0; step < nsteps; step++) {
		if (line_f > line_x) {
			line_x--;
			line_f = line_f-line_x - (line_x-1);
		}
		clipLine (midX, midY, line_x+midX, line_y+midY,
			  0, 0,
			  maxX, maxY);
		line_f = line_f+line_y + line_y+1;
		
		line_y++;
		if (line_y >= line_x) {
			lines_state = s_dup;
			dup_x = midX;
			break;
		}
	}
}

void
redraw_dup (void)
{
	int nsteps, step;

	nsteps = 4;
	for (step = 0; step < nsteps; step++) {
		int yy, y, val, x1, y1, i, o;
		
		/* set vals on diagonal to 2*v-1 */
		if (dup_x - midX + midY <= maxY)
			put_count(dup_x, dup_x-midX+midY,
				 (get_count(dup_x, dup_x-midX+midY)<<1)-1);
		
		/* now do a column from horizontal, down to diag */
		yy = MIN(maxY, dup_x - midX + midY);
		for (y = midY; y <= yy; y++) {
			val = get_count(dup_x, y);
			x1 = dup_x;
			y1 = y;
			for (i = 0; i < 4; i++) {
				if ((y1 < maxY) && (y1 > 0)) {
					put_count(midX + midX - x1,
						 y1, val);
					put_count(x1, y1, val);
				}
				o = x1;
				x1 = midX + midY - y1;
				y1 = midY + o - midX;
			}
		}
		
		dup_x++;
		if (dup_x >= maxX) {
			lines_state = s_run;
			clear_colors ();
			break;
		}
	}
}

/*
 * (xe, ye) and (xf, yf) are the corners of a rectangle to clip a line to.
 * (x0, y0) and (xn, yn) are the endpoints of the line to clip.
 * The function argument that's being computed is the semi-quadrant;
 *  dx and dy are used to determine whether we're above or below the diagonal,
 *  since (x0, y0) is always the midpoint of the pattern.
 * (The LispM has the origin at lower left, instead of upper left, so
 * the numbers don't correspond to the normal Cartesian plane quadrants.)
 *
 * This routine is very general, but the calling code only builds lines in the
 * first semi-quadrant and then copies them everywhere else.
 */

void
clipLine(int x0, int y0, int xn, int yn, int xe, int ye, int xf, int yf)
{
  int	dx, dy;
  
  dx = abs(xn - x0);
  dy = abs(yn - y0);
  
  if (xn > x0) {				/* moving right */
    if (yn >= y0) {			/* moving up */
      if (dx > dy)		/* below diagonal */
	line(0, x0, y0, dx, dy, xe, ye, xf, yf);
      else
	line(1, y0, x0, dy, dx, ye, xe, yf, xf);
    } else {
      if (dx > dy)
	line(7, x0, -y0, dx, dy, xe, -yf, xf, -ye);
      else
	line(6, -y0, x0, dy, dx, -yf, xe, -ye, xf);
    }
  } else {
    if (yn >= y0) {
      if (dx > dy)
	line(3, -x0, y0, dx, dy, -xf, ye, -xe, yf);
      else
	line(2, y0, -x0, dy, dx, ye, -xf, yf, -xe);
    } else {
      if (dx > dy)
	line(4, -x0, -y0, dx, dy, -xf, -yf, -xe, -ye);
      else
	line(5, -y0, -x0, dy, dx, -yf, -xf, -ye, -xe);
    }
  }
}

#define	plot(x, y)	put_count((x), (y), get_count((x), (y))+1)

/*
 * Clip symmetric segment (x0, y0) thru (xn, yn) to the rectangle
 * (xe, ye) < (xf, yf).
 *
 * The original says:
 *
 * "This routine incorrectly assumes that the subsegment starts prior to the
 * midpoint of the supersegment.  The 'divide for nearest integer' (i.e.,
 * divide for remainder of minimum magnitude), which is simulated by the FLOOR
 * and CEIL of num and (dx <<1), always rounds up on the half integer case, but
 * should round down (for symmetry) if startup is in 2nd half. It would be
 * nice to have these other flavors of divide.'
 */

void
line(int fun, int x0, int y0, int dx, int dy, int xe, int ye, int xf, int yf)
{
  int	x, num, lx;
  int	xx, y, x00, f;
  int	x11;
  
  x = MAX(x0,  MAX(xe,
		   (dy == 0)? xe :
		   x0 + CEIL(dx * (((ye - y0)<<1) - 1),
			     (dy << 1))));
  num = dx + 2*dy*(x - x0);
  lx = MIN(xf, (dy == 0) ? xf :
	   x0 + CEIL(dx * (((yf - y0)<<1) - 1),
		     (dy << 1)));
  xx = MIN(lx, x0 + (dx>>1));
  y = y0 + FLOOR(num, (dx<<1));
  f = (FLOOR(num, (dx<<1)) - dx) >> 1;
  
  for (x00 = x; x00 < xx; x00++,f+=dy) {
    if (f+f > dx) {
      f -= dx;
      y++;
    }
    switch(fun) {
    case 0:	plot(x00, y);	break;
    case 1:	plot(y, x00);	break;
    case 2:	plot(-y, x00);	break;
    case 3:	plot(-x00, y);	break;
    case 4:	plot(-x00, -y);	break;
    case 5:	plot(-y, -x00);	break;
    case 6:	plot(y, -x00);	break;
    case 7:	plot(x00, -y);	break;
    }
  }
  
  for (x11 = x00; x11 < lx; x11++, f+=dy) {
    if (f + f > dx) {
      f -= dx;
      y++;
    }
    switch(fun) {
    case 0:	plot(x11, y);	break;
    case 1:	plot(y, x11);	break;
    case 2:	plot(-y, x11);	break;
    case 3:	plot(-x11, y);	break;
    case 4:	plot(-x11, -y);	break;
    case 5:	plot(-y, -x11);	break;
    case 6:	plot(y, -x11);	break;
    case 7:	plot(x11, -y);	break;
    }
  }
}
