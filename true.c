#include "clover.h"

#include <gtk/gtk.h>

GdkVisual *true_visual;
GdkColormap *true_cmap;
GdkImage *true_image;
int *true_pixels;

void
true_init (void)
{
	printf ("TRUE COLOR visual\n");
	true_visual = gdk_visual_get_best_with_type (GDK_VISUAL_TRUE_COLOR);
	if (true_visual == NULL) {
		fprintf (stderr, "can't find true color visual\n");
		exit (1);
	}

	true_cmap = gdk_colormap_new (true_visual, TRUE);

	gtk_widget_set_default_visual (true_visual);
	gtk_widget_set_default_colormap (true_cmap);

	true_pixels = calloc (ncolors, sizeof *true_pixels);
}

void
true_resize (int w, int h)
{
	if (true_image)
		gdk_image_destroy (true_image);

	true_image = gdk_image_new (GDK_IMAGE_FASTEST,
				    true_visual, width, height);
}

void
true_redraw (int counts_changed)
{
	GdkGC *gc;
	int i;
	int r, g, b;
	struct colors *cp;
	unsigned int *countp;
	int color;
	int x, y;
	unsigned char *imagep;
	int pixel;

	for (i = 0, cp = colors; i < ncolors; i++, cp++) {
		r = floor (cp->r * ((1 << true_visual->red_prec) - 1));
		g = floor (cp->g * ((1 << true_visual->green_prec) - 1));
		b = floor (cp->b * ((1 << true_visual->blue_prec) - 1));

		r <<= true_visual->red_shift;
		g <<= true_visual->green_shift;
		b <<= true_visual->blue_shift;
		
		true_pixels[i] = r | g | b;
	}

	gc = main_area->style->fg_gc[GTK_WIDGET_STATE(main_area)];

	countp = counts;
	for (y = 0; y < height; y++) {
		imagep = true_image->mem + y * true_image->bpl;
		for (x = 0; x < width; x++) {
			color = *countp++ % ncolors;
			pixel = true_pixels[color];
			
			/* XXX depends on byte order */
			*imagep++ = pixel;
			*imagep++ = pixel >> 8;
		}
	}
	gdk_draw_image (main_pixmap, gc,
			true_image, 0, 0, 0, 0, width, height);
}

static struct output_handler funcs = {
	true_init,
	true_resize,
	true_redraw,
};
struct output_handler *true_output_handler = &funcs;
