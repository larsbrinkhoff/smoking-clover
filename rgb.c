#include "clover.h"

#include <gtk/gtk.h>

GdkImage *rgb_image;
GdkVisual *rgb_visual;
int *rgb_pixels;

GdkRgbCmap *rgb_cmap;

unsigned char *rgb_counts8;

void
rgb_init (void)
{
	gint32 colors[256];
	int i;

	printf ("RGB\n");
	gdk_rgb_init();

	gtk_widget_set_default_colormap (gdk_rgb_get_cmap());
	rgb_visual = gdk_rgb_get_visual ();
	gtk_widget_set_default_visual (rgb_visual);

	for (i = 0; i < 256; i++)
		colors[i] = i;
	rgb_cmap = gdk_rgb_cmap_new (colors, 256);
}

void
rgb_resize (int w, int h)
{
	if (rgb_counts8)
		free (rgb_counts8);
	rgb_counts8 = calloc (width * height, sizeof *rgb_counts8);
}

void
rgb_redraw (int counts_changed)
{
	GdkGC *gc;
	int i;
	int r, g, b;
	struct colors *cp;
	int *countp;
	int x, y;
	unsigned char *count8p;

	for (i = 0, cp = colors; i < ncolors; i++, cp++) {
		r = floor (cp->r * 255);
		g = floor (cp->g * 255);
		b = floor (cp->b * 255);

		rgb_cmap->colors[i] = (r << 16) | (g << 8) | b;
	}

	countp = counts;
	count8p = rgb_counts8;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			*count8p++ = *countp++ % ncolors;
		}
	}

	gc = main_area->style->fg_gc[GTK_WIDGET_STATE(main_area)];
	gdk_draw_indexed_image (main_pixmap, gc,
				0, 0, width, height,
				GDK_RGB_DITHER_NONE,
				rgb_counts8, width,
				rgb_cmap);
}

static struct output_handler funcs = {
	rgb_init,
	rgb_resize,
	rgb_redraw,
};
struct output_handler *rgb_output_handler = &funcs;
