#include "clover.h"

#include <gtk/gtk.h>

GdkVisual *dir_visual;
GdkColormap *dir_cmap;
GdkImage *dir_image;
GdkColor *dir_colors;
gboolean *dir_flags;

void
direct_pick (void)
{
	dir_visual = gdk_visual_get_best_with_type (GDK_VISUAL_DIRECT_COLOR);
	if (dir_visual != NULL && dir_visual->depth > 8)
		return;

	dir_visual = gdk_visual_get_best_with_type (GDK_VISUAL_PSEUDO_COLOR);
	if (dir_visual != NULL)
		return;

	fprintf (stderr, "can't get DIRECTCOLOR visual\n");
	exit (1);
}

void
direct_init (void)
{
	int errcount;
	int i;

	direct_pick ();

	dir_cmap = gdk_colormap_new (dir_visual, TRUE);

	ncolors = dir_visual->colormap_size;
	dir_colors = calloc (ncolors, sizeof *dir_colors);
	dir_flags = calloc (ncolors, sizeof *dir_flags);

	for (i = 0; i < ncolors; i++) {
		dir_colors[i].red = i * 65535 / ncolors;
		dir_colors[i].green = i * 65535 / ncolors;
		dir_colors[i].blue = i * 65535 / ncolors;
	}
	errcount = gdk_colormap_alloc_colors (dir_cmap, dir_colors, ncolors,
					      TRUE, FALSE, dir_flags);

	if (errcount != 0) {
		fprintf (stderr, "error allocating colors\n");
	}

	gtk_widget_set_default_visual (dir_visual);
	gtk_widget_set_default_colormap (dir_cmap);

	printf ("put mouse in window to see the color changes\n");
}

void
direct_resize (int w, int h)
{
	if (dir_image)
		gdk_image_destroy (dir_image);
	dir_image = gdk_image_new (GDK_IMAGE_FASTEST,
				   dir_visual, width, height);

}

void
direct_redraw (int counts_changed)
{
	GdkGC *gc;
	int i;
	unsigned int *countp;
	int color;
	int x, y;
	unsigned char *imagep;
	int pixel;
	struct colors *cp;

	gc = main_area->style->fg_gc[GTK_WIDGET_STATE(main_area)];

	for (i = 0, cp = colors; i < ncolors; i++, cp++) {
		dir_colors[i].red = floor (cp->r * 65535);
		dir_colors[i].green = floor (cp->g * 65535);
		dir_colors[i].blue = floor (cp->b * 65535);

		if (gdk_color_change (dir_cmap, &dir_colors[i]) != TRUE) {
			fprintf (stderr, "color change error\n");
			exit (1);
		}
	}

	if (counts_changed) {
		countp = counts;
		for (y = 0; y < height; y++) {
			imagep = dir_image->mem + y * dir_image->bpl;
			for (x = 0; x < width; x++) {
				color = *countp++ % ncolors;
				
				pixel = dir_colors[color].pixel;
				
				/* XXX depends on byte order */
				*imagep++ = pixel;
				*imagep++ = pixel >> 8;
			}
		}
		
		gdk_draw_image (main_pixmap, gc,
				dir_image, 0, 0, 0, 0, width, height);
	}
}

static struct output_handler funcs = {
	direct_init,
	direct_resize,
	direct_redraw,
};
struct output_handler *direct_output_handler = &funcs;
