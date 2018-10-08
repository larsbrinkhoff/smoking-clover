FILE *movie_f;
void
make_movie_init (void)
{
	char cmd[10000];
	sprintf (cmd, "sh -c 'ppmtoy4m | mpeg2enc --format 1 -o output.mpg'");
	if ((movie_f = popen (cmd, "w")) == NULL) {
		fprintf (stderr, "error trying to run %s\n", cmd);
		exit (1);
	}
}

void
movie_finish (void)
{
	pclose (movie_f);
	movie_f = NULL;
}

void
write_frame (void)
{
	static int frame;
	int x, y, val;
	int rgb, r, g, b;

	fprintf (movie_f, "P6\n");
	fprintf (movie_f, "%d %d\n", width, height);
	fprintf (movie_f, "255\n");
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			val = getPixel (x, y);
			rgb = cmap.colors[val % ncolors];
			r = (rgb >> 16) & 0xff;
			g = (rgb >> 8) & 0xff;
			b = rgb & 0xff;
			putc (r, movie_f);
			putc (g, movie_f);
			putc (b, movie_f);
		}
	}

	frame++;
	if (frame >= frame_limit) {
		movie_finish ();
		exit (0);
	}
}
