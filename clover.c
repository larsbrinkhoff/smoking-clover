#include "clover.h"

#include <gtk/gtk.h>

GtkWidget *main_area;
GdkPixmap *main_pixmap;
GtkWidget *main_window;

struct output_handler *output_handler;
struct colors *colors;
unsigned int *counts;
int width, height;
int maxX, maxY;
int midX, midY;
int ncolors;

enum lines_enum lines_state;

void make_movie_init (void);

void write_frame (void);

int need_redraw;
void redraw (void);
void force_redraw (void);

void redo_lines ();

gint main_configure (GtkWidget *area, GdkEventConfigure *ev);
gint main_expose (GtkWidget *area, GdkEventExpose *ev);
gint main_key_press (GtkWidget *area, GdkEventKey *ev);

void 
done ()
{
	gtk_main_quit ();
	exit (0);
}

gint
timer_handler (gpointer data)
{
	force_redraw ();
	return (TRUE);
}

void
usage ()
{
	fprintf (stderr, "usage: pclover\n");
	fprintf (stderr, "   -m      make movie\n");
	exit (1);
}

int make_movie;
int frame_limit;
int dflag;
int rflag;

int
main (int argc, char **argv)
{
	int w, h;
	int c;
	int i;

	for (i = 1; i < argc; i++) {
		if (strcmp (argv[i], "-L") == 0) {
			putenv ("DISPLAY=h55:0");
			break;
		}
	}

	frame_limit = 5 * 60 * 30;

	gtk_init (&argc, &argv);

	while ((c = getopt (argc, argv, "ml:drL")) != EOF) {
		switch (c) {
		case 'L':
			break;
		case 'r':
			rflag = 1;
			break;
		case 'd':
			dflag = 1;
			break;
		case 'm':
			make_movie = 1;
			break;
		case 'l':
			frame_limit = atoi (optarg);
			break;
		default:
			usage ();
		}
	}

	if (rflag) {
		ncolors = 256;
		output_handler = rgb_output_handler;
	} else if (dflag) {
		ncolors = 256;
		output_handler = direct_output_handler;
	} else {
		ncolors = 256;
		output_handler = true_output_handler;
	}

	if (output_handler == NULL) {
		fprintf (stderr, "no output handler\n");
		exit (1);
	}

	(*output_handler->init)();

	main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

	make_colors ();

	w = gdk_screen_width () / 4;
	h = gdk_screen_height () / 4;

#if 0
	if (make_movie) {
		make_movie_init ();
		w = 352;
		h = 240;
	}
#endif

	gtk_window_set_default_size (GTK_WINDOW (main_window), w, h);

	main_area = gtk_drawing_area_new ();
	gtk_widget_show (main_area);

	gtk_window_set_title (GTK_WINDOW (main_window), "pclover");
	gtk_quit_add_destroy (1, GTK_OBJECT (main_window));
	gtk_signal_connect (GTK_OBJECT (main_window), "delete_event",
			    GTK_SIGNAL_FUNC (done), NULL);
	gtk_signal_connect (GTK_OBJECT (main_window), "destroy",
			    GTK_SIGNAL_FUNC (done), NULL);
	gtk_signal_connect (GTK_OBJECT (main_area), "expose_event",
			    GTK_SIGNAL_FUNC (main_expose), NULL);
	gtk_signal_connect (GTK_OBJECT (main_area), "configure_event",
			    GTK_SIGNAL_FUNC (main_configure), NULL);
	gtk_signal_connect (GTK_OBJECT(main_window), "key_press_event",
			    GTK_SIGNAL_FUNC(main_key_press), NULL);

	gtk_container_add (GTK_CONTAINER (main_window), main_area);

	gtk_widget_set_events(GTK_WIDGET(main_area),
			      GDK_EXPOSURE_MASK|
			      GDK_KEY_PRESS_MASK);

	gtk_widget_show (main_window);

	gtk_timeout_add (1/24.0 * 1000, timer_handler, NULL);

	gtk_main ();

	return (0);
}

gint
main_configure (GtkWidget *area, GdkEventConfigure *ev)
{
	set_size (area->allocation.width, area->allocation.height);

	if (main_pixmap)
		gdk_pixmap_unref (main_pixmap);
	main_pixmap = gdk_pixmap_new (area->window, width, height, -1);

	(*output_handler->resize)(width, height);

	need_redraw = 1;

	return (TRUE);
}

gint
main_expose (GtkWidget *area, GdkEventExpose *ev)
{
	if (need_redraw) {
		need_redraw = 0;
		redraw ();
	}

	gdk_draw_pixmap (area->window,
			 area->style->fg_gc[GTK_WIDGET_STATE (area)],
			 main_pixmap,
			 ev->area.x, ev->area.y,
			 ev->area.x, ev->area.y,
			 ev->area.width, ev->area.height);

	return (FALSE);
}

gint
main_key_press (GtkWidget *area, GdkEventKey *ev)
{
/*	app_key_event (ev->state, ev->keyval, ev->length, ev->string); */
	return (TRUE);
}

void
force_redraw (void)
{
 	GdkRectangle area;

	need_redraw = 1;

	area.x = 0;
	area.y = 0;
	area.width = width;
	area.height = height;
	gtk_widget_draw(main_area, &area);
}

void
redraw_run (void)
{
	next_color (1);
}

void
redraw (void)
{
	int counts_changed;

	counts_changed = 0;
	switch (lines_state) {
	case s_lines:
		counts_changed = 1;
		redraw_lines ();
		break;

	case s_dup: 
		counts_changed = 1;
		redraw_dup ();
		break;

	case s_run:
		redraw_run ();
		break;
	}

	(*output_handler->redraw)(counts_changed);
}
