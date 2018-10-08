#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <memory.h>
#include <time.h>
#include <math.h>

enum {
	s_lines,
	s_dup,
	s_run,
} lines_state;

/* values are 0..1 */
struct colors {
	double r, g, b;
};
struct colors *colors;

void make_colors (void);
void set_size (int width, int height);
void next_color (int steps);
void redraw_lines (void);
void redraw_dup (void);

unsigned int *counts;

int width, height;
int maxX, maxY;
int midX, midY;

int ncolors;

struct output_handler {
	void (*init)(void);
	void (*resize)(int width, int height);
	void (*redraw)(int counts_changed);
};

struct output_handler *direct_output_handler;
struct output_handler *true_output_handler;
struct output_handler *rgb_output_handler;


/* ... */
#include <gtk/gtk.h>
GtkWidget *main_area;
GdkPixmap *main_pixmap;
GtkWidget *main_window;
