#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <memory.h>
#include <time.h>
#include <math.h>

enum lines_enum {
	s_lines,
	s_dup,
	s_run,
};
extern enum lines_enum lines_state;

/* values are 0..1 */
struct colors {
	double r, g, b;
};
extern struct colors *colors;

void make_colors (void);
void set_size (int width, int height);
void next_color (int steps);
void redraw_lines (void);
void redraw_dup (void);

extern unsigned int *counts;

extern int width, height;
extern int maxX, maxY;
extern int midX, midY;

extern int ncolors;

struct output_handler {
	void (*init)(void);
	void (*resize)(int width, int height);
	void (*redraw)(int counts_changed);
};

extern struct output_handler *direct_output_handler; /* defined in direct.c */
extern struct output_handler *true_output_handler; /* defined in true.c */
extern struct output_handler *rgb_output_handler; /* defined in rgb.c */


/* ... */
#include <gtk/gtk.h>
extern GtkWidget *main_area;
extern GdkPixmap *main_pixmap;
extern GtkWidget *main_window;
