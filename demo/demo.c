/*
 * GooCanvas Demo. Copyright (C) 2005 Damon Chaplin.
 * Released under the GNU LGPL license. See COPYING for details.
 *
 * main.c - demo app.
 */
#include <config.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <cairo.h>
#if CAIRO_HAS_PDF_SURFACE
#include <cairo-pdf.h>
#endif
#include <goocanvas.h>
#include "demo-item.h"

static GooCanvasItem *ellipse2, *textitem;

static gboolean dragging = FALSE;
static double drag_x, drag_y;

static void setup_canvas (GooCanvas *canvas);
GtkWidget *create_canvas_fifteen (void);
GtkWidget *create_canvas_features (void);
GtkWidget *create_canvas_arrowhead (void);
GtkWidget *create_canvas_scalability (void);
GtkWidget *create_grabs_page (void);
GtkWidget *create_events_page (void);
GtkWidget *create_paths_page (void);
GtkWidget *create_focus_page (void);
GtkWidget *create_animation_page (void);
GtkWidget *create_clipping_page (void);
GtkWidget *create_table_page (void);
GtkWidget *create_large_items_page (void);

#if CAIRO_HAS_PDF_SURFACE
static void
write_pdf_clicked (GtkWidget *button, GooCanvas *canvas)
{
  cairo_surface_t *surface;
  GooCanvasBounds bounds G_GNUC_UNUSED;
  cairo_t *cr;

  g_print ("In write_pdf_clicked\n");

  surface = cairo_pdf_surface_create ("demo.pdf", 9 * 72, 10 * 72);
  cr = cairo_create (surface);

  /* Place it in the middle of our 9x10 page. */
  cairo_translate (cr, 20, 130);

  bounds.x1 = 100;
  bounds.y1 = 100;
  bounds.x2 = 300;
  bounds.y2 = 300;

#if 1
  goo_canvas_render (canvas, cr, NULL, 1.0);
#else
  goo_canvas_render (canvas, cr, &bounds, 1.0);
#endif

  cairo_show_page (cr);

  cairo_surface_destroy (surface);
  cairo_destroy (cr);
}
#endif

static void
zoom_changed (GtkAdjustment *adj, GooCanvas *canvas)
{
  goo_canvas_set_scale (canvas, gtk_adjustment_get_value (adj));
}


static void
zoom_x_changed (GtkAdjustment *adj, GooCanvas *canvas)
{
  g_object_set (G_OBJECT (canvas),
		"scale-x", gtk_adjustment_get_value (adj),
		NULL);
}


static void
zoom_y_changed (GtkAdjustment *adj, GooCanvas *canvas)
{
  g_object_set (G_OBJECT (canvas),
		"scale-y", gtk_adjustment_get_value (adj),
		NULL);
}


static void
center_toggled (GtkToggleButton *button, gpointer data)
{
  /*foo_canvas_set_center_scroll_region (data,
                                   gtk_toggle_button_get_active (button));*/
}


static void
anchor_toggled (GtkWidget *button, GooCanvas *canvas)
{
  GooCanvasAnchorType anchor;

  anchor = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (button), "anchor"));

  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)))
    g_object_set (canvas, "anchor", anchor, NULL);
}


static void
scroll_to_50_50_clicked (GtkWidget *button, GooCanvas *canvas)
{
  goo_canvas_scroll_to (canvas, 50, 50);
}


static void
scroll_to_500_500_clicked (GtkWidget *button, GooCanvas *canvas)
{
  goo_canvas_scroll_to (canvas, 500, 500);
}


static void
scroll_to_250_250_clicked (GtkWidget *button, GooCanvas *canvas)
{
  goo_canvas_scroll_to (canvas, 250, 250);
}


static void
animate_ellipse_clicked (GtkWidget *button, GooCanvas *canvas)
{
#if 1
  goo_canvas_item_animate (ellipse2, 100, 100, 1, 90, TRUE, 1000, 40,
			   GOO_CANVAS_ANIMATE_BOUNCE);
#endif
#if 0
  goo_canvas_item_animate (textitem, -300, -200, 1, 90, TRUE, 1000, 40,
			   GOO_CANVAS_ANIMATE_BOUNCE);
#endif
}


static void
stop_animation_clicked (GtkWidget *button, GooCanvas *canvas)
{
  goo_canvas_item_stop_animation (ellipse2);
}


static void
change_bounds_clicked (GtkWidget *button, GooCanvas *canvas)
{
  static gdouble bounds[4][4] = {
    { -100, -100, 200, 200 },
    { 100, 100, 200, 200 },
    { 0, 0, 300, 300 },
    { 0, 0, 604, 454 }
  };
  static gint bounds_num = 0;

  gboolean automatic_bounds = TRUE;
  gboolean bounds_from_origin = FALSE;
  gdouble bounds_padding = 50.0;

  if (bounds_num < 4)
    {
      automatic_bounds = FALSE;
      goo_canvas_set_bounds (canvas,
			     bounds[bounds_num][0], bounds[bounds_num][1],
			     bounds[bounds_num][2], bounds[bounds_num][3]);
    }
  else if (bounds_num == 4)
    {
      bounds_from_origin = TRUE;
      bounds_padding = 50.0;
    }

  g_object_set (G_OBJECT (canvas),
		"automatic-bounds", automatic_bounds,
		"bounds-from-origin", bounds_from_origin,
		"bounds-padding", bounds_padding,
		NULL);

  bounds_num = (bounds_num + 1) % 6;
}


static void
move_ellipse_clicked (GtkWidget *button, GooCanvas *canvas)
{
  static int last_state = 0;

#if 0
  g_print ("Moving ellipse\n");
#endif

  goo_canvas_item_set_transform (ellipse2, NULL);

  if (last_state == 0)
    {
      g_object_set (ellipse2,
		    "center-x", 300.0,
		    "center-y", 70.0,
		    "radius-x", 45.0,
		    "radius-y", 30.0,
		    "fill-color", "red",
		    "stroke-color", "midnightblue",
		    "line-width", 4.0,
		    "title", "A red ellipse",
		    NULL);
      last_state = 1;
    }
  else if (last_state == 1)
    {
      g_object_set (ellipse2,
		    "center-x", 390.0,
		    "center-y", 150.0,
		    "radius-x", 45.0,
		    "radius-y", 40.0,
		    "fill-pattern", NULL,
		    "stroke-color", "midnightblue",
		    "line-width", 4.0,
		    "title", "A brown ellipse",
		    NULL);
      last_state = 2;
    }
  else if (last_state == 2)
    {
      g_object_set (ellipse2,
		    "center-x", 0.0,
		    "center-y", 0.0,
		    "radius-y", 30.0,
		    NULL);
      goo_canvas_item_set_simple_transform (ellipse2, 100, 100, 1, 0);
      last_state = 3;
    }
  else if (last_state == 3)
    {
      goo_canvas_item_set_simple_transform (ellipse2, 200, 100, 2, 0);
      last_state = 4;
    }
  else if (last_state == 4)
    {
      goo_canvas_item_set_simple_transform (ellipse2, 200, 200, 1, 45);
      last_state = 5;
    }
  else if (last_state == 5)
    {
      goo_canvas_item_set_simple_transform (ellipse2, 50, 50, 0.2, 225);
      last_state = 6;
    }
  else
    {
      g_object_set (ellipse2,
		    "center-x", 335.0,
		    "center-y", 70.0,
		    "radius-x", 45.0,
		    "radius-y", 30.0,
		    "fill-color", "purple",
		    "stroke-color", "midnightblue",
		    "line-width", 4.0,
		    "title", "A purple ellipse",
		    NULL);
      last_state = 0;
    }
}


static gboolean
on_motion_notify (GooCanvasItem *item,
		  GooCanvasItem *target,
		  GdkEventMotion *event,
		  gpointer data)
{
#if 0
  g_print ("received 'motion-notify' signal at %g, %g\n",
	   event->x, event->y);
#endif

  if (dragging && (event->state & GDK_BUTTON1_MASK))
    {
      double new_x = event->x;
      double new_y = event->y;

      goo_canvas_item_translate (item, new_x - drag_x, new_y - drag_y);
    }

  return TRUE;
}


static void
output_items_in_area (GooCanvas *canvas,
		      gdouble x,
		      gdouble y)
{
  static gdouble last_x = 0.0, last_y = 0.0;
  GooCanvasBounds area;
  GList *items, *elem;

  /* Print out the items from the last point to this one. */
  area.x1 = MIN (x, last_x);
  area.x2 = MAX (x, last_x);
  area.y1 = MIN (y, last_y);
  area.y2 = MAX (y, last_y);
  items = goo_canvas_get_items_in_area (canvas, &area, TRUE, FALSE, FALSE);
  for (elem = items; elem; elem = elem->next)
    {
      g_print ("  found items in area (%g, %g - %g, %g): %p\n",
	       area.x1, area.y1, area.x2, area.y2, elem->data);
    }
  g_list_free (items);
  last_x = x;
  last_y = y;
}


static gboolean
on_button_press (GooCanvasItem *item,
		 GooCanvasItem *target,
		 GdkEventButton *event,
		 gpointer data)
{
  GooCanvas *canvas;
  GdkDisplay *display;
  GdkCursor *fleur;
  GList *items, *elem;

  g_print ("%p received 'button-press' signal at %g, %g (root: %g, %g)\n",
	   item, event->x, event->y, event->x_root, event->y_root);

  canvas = goo_canvas_item_get_canvas (item);
  items = goo_canvas_get_items_at (canvas, event->x_root, event->y_root,
				   TRUE);
  for (elem = items; elem; elem = elem->next)
    g_print ("  found items: %p\n", elem->data);
  g_list_free (items);

  output_items_in_area (canvas, event->x_root, event->y_root);

  switch (event->button)
    {
    case 1:
      if (event->state & GDK_SHIFT_MASK)
	{
	  goo_canvas_item_remove (item);
	}
      else
	{
	  drag_x = event->x;
	  drag_y = event->y;

	  display = gtk_widget_get_display (GTK_WIDGET (canvas));
	  fleur = gdk_cursor_new_for_display (display, GDK_FLEUR);
	  goo_canvas_pointer_grab (canvas, item,
				   GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_RELEASE_MASK,
				   fleur,
				   event->time);
	  g_object_unref (fleur);
	  dragging = TRUE;
	}
      break;

    case 2:
      goo_canvas_item_lower (item, NULL);
      break;

    case 3:
      goo_canvas_item_raise (item, NULL);
      break;

    default:
      break;
    }

  return TRUE;
}


static gboolean
on_scroll (GooCanvasItem *item,
	   GooCanvasItem *target,
	   GdkEventScroll *event,
	   gpointer data)
{
#if 0
  g_print ("received 'scroll-event' signal\n");
#endif

  if (event->direction == GDK_SCROLL_UP)
    goo_canvas_item_scale (item, 1.1, 1.1);
  else if (event->direction == GDK_SCROLL_DOWN)
    goo_canvas_item_scale (item, 0.909, 0.909);
  else return FALSE;

  return TRUE;
}

static gboolean
on_button_release (GooCanvasItem *item,
		   GooCanvasItem *target,
		   GdkEventButton *event,
		   gpointer data)
{
  GooCanvas *canvas;

#if 0
  g_print ("received 'button-release' signal\n");
#endif

  canvas = goo_canvas_item_get_canvas (item);
  goo_canvas_pointer_ungrab (canvas, item, event->time);
  dragging = FALSE;

  return TRUE;
}

static gboolean
on_background_button_press (GooCanvasItem *item,
			    GooCanvasItem *target,
			    GdkEventButton *event,
			    gpointer data)
{
  GooCanvas *canvas;
  GList *items, *elem;

#if 1
  g_print ("background received 'button-press' signal\n");
#endif

  canvas = goo_canvas_item_get_canvas (item);
  output_items_in_area (canvas, event->x_root, event->y_root);

  items = goo_canvas_get_items_at (canvas, event->x_root, event->y_root,
				   FALSE);
  for (elem = items; elem; elem = elem->next)
    g_print ("  clicked items: %p\n", elem->data);
  g_list_free (items);

  return TRUE;
}


static void
setup_item_signals (GooCanvasItem *item)
{
  g_signal_connect (item, "motion_notify_event",
		    G_CALLBACK (on_motion_notify), NULL);
  g_signal_connect (item, "button_press_event",
		    G_CALLBACK (on_button_press), NULL);
  g_signal_connect (item, "button_release_event",
		    G_CALLBACK (on_button_release), NULL);
}


GtkWidget *
create_canvas_primitives ()
{
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *w;
	GtkWidget *scrolled_win, *canvas;
	GtkAdjustment *adj;
	GSList *group = NULL;

	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);
	gtk_container_set_border_width (GTK_CONTAINER (vbox), 4);
	gtk_widget_show (vbox);

	w = gtk_label_new ("Drag an item with button 1.  Click button 2 on an item to lower it, or button 3 to raise it.");
	gtk_box_pack_start (GTK_BOX (vbox), w, FALSE, FALSE, 0);
	gtk_widget_show (w);

	hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show (hbox);

	/* Create the canvas */

	canvas = goo_canvas_new ();
	g_object_set (G_OBJECT (canvas),
		      "automatic-bounds", TRUE,
		      "bounds-from-origin", FALSE,
		      "bounds-padding", 4.0,
		      "background-color-rgb", 0xC3C3FF,
 		      "has-tooltip", TRUE,
#if 0
		      "redraw-when-scrolled", TRUE,
#endif
		      NULL);
	goo_canvas_set_bounds (GOO_CANVAS (canvas), 0, 0, 604, 454);

	/* Scale */

	w = gtk_label_new ("Scale:");
	gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
	gtk_widget_show (w);

	adj = GTK_ADJUSTMENT (gtk_adjustment_new (1.00, 0.05, 50.00, 0.05, 0.50, 0.50));
	w = gtk_spin_button_new (adj, 0.0, 2);
	g_signal_connect (adj, "value_changed",
			  G_CALLBACK (zoom_changed),
			  canvas);
	gtk_widget_set_size_request (w, 50, -1);
	gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
	gtk_widget_show (w);


	w = gtk_label_new ("Scale X:");
	gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
	gtk_widget_show (w);

	adj = GTK_ADJUSTMENT (gtk_adjustment_new (1.00, 0.05, 50.00, 0.05, 0.50, 0.50));
	w = gtk_spin_button_new (adj, 0.0, 2);
	g_signal_connect (adj, "value_changed",
			  G_CALLBACK (zoom_x_changed),
			  canvas);
	gtk_widget_set_size_request (w, 50, -1);
	gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
	gtk_widget_show (w);


	w = gtk_label_new ("Scale Y:");
	gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
	gtk_widget_show (w);

	adj = GTK_ADJUSTMENT (gtk_adjustment_new (1.00, 0.05, 50.00, 0.05, 0.50, 0.50));
	w = gtk_spin_button_new (adj, 0.0, 2);
	g_signal_connect (adj, "value_changed",
			  G_CALLBACK (zoom_y_changed),
			  canvas);
	gtk_widget_set_size_request (w, 50, -1);
	gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
	gtk_widget_show (w);


	hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show (hbox);

	/* Center: */
	
	w = gtk_check_button_new_with_label("Center scroll region");
	gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
	/*gtk_widget_show (w);*/
	g_signal_connect (w, "toggled",
			  G_CALLBACK (center_toggled),
			  canvas);

	/* Move Ellipse */

	w = gtk_button_new_with_label("Move Ellipse");
	gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
	gtk_widget_show (w);
	g_signal_connect (w, "clicked",
			  G_CALLBACK (move_ellipse_clicked),
			  canvas);

	/* Animate Ellipse */

	w = gtk_button_new_with_label("Animate Ellipse");
	gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
	gtk_widget_show (w);
	g_signal_connect (w, "clicked",
			  G_CALLBACK (animate_ellipse_clicked),
			  canvas);
	
	/* Stop Animation */

	w = gtk_button_new_with_label("Stop Animation");
	gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
	gtk_widget_show (w);
	g_signal_connect (w, "clicked",
			  G_CALLBACK (stop_animation_clicked),
			  canvas);
	

#if CAIRO_HAS_PDF_SURFACE
	/* Create PDF */

	w = gtk_button_new_with_label("Write PDF");
	gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
	gtk_widget_show (w);
	g_signal_connect (w, "clicked",
			  G_CALLBACK (write_pdf_clicked),
			  canvas);
#endif

	w = gtk_button_new_with_label("Change Bounds");
	gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
	gtk_widget_show (w);
	g_signal_connect (w, "clicked",
			  G_CALLBACK (change_bounds_clicked),
			  canvas);

	hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show (hbox);

	/* Scroll to */

	w = gtk_label_new ("Scroll To:");
	gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
	gtk_widget_show (w);

	w = gtk_button_new_with_label("50,50");
	gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
	gtk_widget_show (w);
	g_signal_connect (w, "clicked",
			  G_CALLBACK (scroll_to_50_50_clicked),
			  canvas);

	w = gtk_button_new_with_label("250,250");
	gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
	gtk_widget_show (w);
	g_signal_connect (w, "clicked",
			  G_CALLBACK (scroll_to_250_250_clicked),
			  canvas);

	w = gtk_button_new_with_label("500,500");
	gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
	gtk_widget_show (w);
	g_signal_connect (w, "clicked",
			  G_CALLBACK (scroll_to_500_500_clicked),
			  canvas);

	/* Scroll anchor */

	w = gtk_label_new ("Anchor:");
	gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
	gtk_widget_show (w);

	w = gtk_radio_button_new_with_label (group, "NW");
	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (w));
	gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
	gtk_widget_show (w);
	g_signal_connect (w, "toggled",
			  G_CALLBACK (anchor_toggled),
			  canvas);
	g_object_set_data (G_OBJECT (w), "anchor",
			   GINT_TO_POINTER (GOO_CANVAS_ANCHOR_NW));

	w = gtk_radio_button_new_with_label (group, "N");
	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (w));
	gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
	gtk_widget_show (w);
	g_signal_connect (w, "toggled",
			  G_CALLBACK (anchor_toggled),
			  canvas);
	g_object_set_data (G_OBJECT (w), "anchor",
			   GINT_TO_POINTER (GOO_CANVAS_ANCHOR_N));

	w = gtk_radio_button_new_with_label (group, "NE");
	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (w));
	gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
	gtk_widget_show (w);
	g_signal_connect (w, "toggled",
			  G_CALLBACK (anchor_toggled),
			  canvas);
	g_object_set_data (G_OBJECT (w), "anchor",
			   GINT_TO_POINTER (GOO_CANVAS_ANCHOR_NE));

	w = gtk_radio_button_new_with_label (group, "W");
	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (w));
	gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
	gtk_widget_show (w);
	g_signal_connect (w, "toggled",
			  G_CALLBACK (anchor_toggled),
			  canvas);
	g_object_set_data (G_OBJECT (w), "anchor",
			   GINT_TO_POINTER (GOO_CANVAS_ANCHOR_W));

	w = gtk_radio_button_new_with_label (group, "C");
	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (w));
	gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
	gtk_widget_show (w);
	g_signal_connect (w, "toggled",
			  G_CALLBACK (anchor_toggled),
			  canvas);
	g_object_set_data (G_OBJECT (w), "anchor",
			   GINT_TO_POINTER (GOO_CANVAS_ANCHOR_CENTER));

	w = gtk_radio_button_new_with_label (group, "E");
	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (w));
	gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
	gtk_widget_show (w);
	g_signal_connect (w, "toggled",
			  G_CALLBACK (anchor_toggled),
			  canvas);
	g_object_set_data (G_OBJECT (w), "anchor",
			   GINT_TO_POINTER (GOO_CANVAS_ANCHOR_E));

	w = gtk_radio_button_new_with_label (group, "SW");
	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (w));
	gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
	gtk_widget_show (w);
	g_signal_connect (w, "toggled",
			  G_CALLBACK (anchor_toggled),
			  canvas);
	g_object_set_data (G_OBJECT (w), "anchor",
			   GINT_TO_POINTER (GOO_CANVAS_ANCHOR_SW));

	w = gtk_radio_button_new_with_label (group, "S");
	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (w));
	gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
	gtk_widget_show (w);
	g_signal_connect (w, "toggled",
			  G_CALLBACK (anchor_toggled),
			  canvas);
	g_object_set_data (G_OBJECT (w), "anchor",
			   GINT_TO_POINTER (GOO_CANVAS_ANCHOR_S));

	w = gtk_radio_button_new_with_label (group, "SE");
	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (w));
	gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
	gtk_widget_show (w);
	g_signal_connect (w, "toggled",
			  G_CALLBACK (anchor_toggled),
			  canvas);
	g_object_set_data (G_OBJECT (w), "anchor",
			   GINT_TO_POINTER (GOO_CANVAS_ANCHOR_SE));

	
	/* Layout the stuff */

	scrolled_win = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolled_win);
	gtk_box_pack_start (GTK_BOX (vbox), scrolled_win, TRUE, TRUE, 0);

	gtk_widget_show (canvas);
	gtk_container_add (GTK_CONTAINER (scrolled_win), canvas);

	/* Add all the canvas items. */
	setup_canvas (GOO_CANVAS (canvas));

#if 0
	g_signal_connect_after (canvas, "key_press_event",
				G_CALLBACK (key_press),
				NULL);

	GTK_WIDGET_SET_FLAGS (canvas, GTK_CAN_FOCUS);
	gtk_widget_grab_focus (canvas);
#endif

	return vbox;
}


static void
setup_heading (GooCanvasItem *root, char *text, int pos)
{
  double x = (pos % 3) * 200 + 100;
  double y = (pos / 3) * 150 + 5;
  GooCanvasItem *item;
  PangoRectangle ink_rect, logical_rect;

  item = goo_canvas_text_new (root, text, x, y, -1, GOO_CANVAS_ANCHOR_N,
			      "font", "Sans 12",
			      NULL);
  goo_canvas_item_skew_y (item, 30, x, y);
  /*goo_canvas_item_rotate (item, 30, x, y);*/

  goo_canvas_text_get_natural_extents (GOO_CANVAS_TEXT (item),
				       &ink_rect, &logical_rect);
  g_print ("Ink Extents: %i,%i %ix%i  Logical: %i,%i %ix%i\n",
	   ink_rect.x, ink_rect.y, ink_rect.width, ink_rect.height,
	   logical_rect.x, logical_rect.y,
	   logical_rect.width, logical_rect.height);
}


static void
setup_divisions (GooCanvasItem *root)
{
  GooCanvasItem *group, *item G_GNUC_UNUSED;

  group = goo_canvas_group_new (root, NULL);
  goo_canvas_item_translate (group, 2, 2);

  item = goo_canvas_rect_new (group, 0, 0, 600, 450,
			      "line-width", 4.0,
			      NULL);

  item = goo_canvas_polyline_new_line (group, 0, 150, 600, 150,
			      "line-width", 4.0,
			      NULL);

  item = goo_canvas_polyline_new_line (group, 0, 300, 600, 300,
			      "line-width", 4.0,
			      NULL);

  item = goo_canvas_polyline_new_line (group, 200, 0, 200, 450,
			      "line-width", 4.0,
			      NULL);

  item = goo_canvas_polyline_new_line (group, 400, 0, 400, 450,
			      "line-width", 4.0,
			      NULL);

#if 1
  setup_heading (group, "Rectangles", 0);
  setup_heading (group, "Ellipses", 1);
  setup_heading (group, "Texts", 2);
  setup_heading (group, "Images", 3);
  setup_heading (group, "Lines", 4);
  /*setup_heading (group, "No Curves", 5);*/
  /*setup_heading (group, "Arcs", 6);*/
  setup_heading (group, "Polygons", 7);
  /*setup_heading (group, "Widgets", 8);*/
#endif
}


/* FIXME: I think we may need to check byte order. */
static cairo_pattern_t*
create_stipple (const char *color_name, guchar stipple_data[16])
{
  cairo_surface_t *surface;
  cairo_pattern_t *pattern;
  GdkRGBA color;

  gdk_rgba_parse (&color, color_name);
  stipple_data[2] = stipple_data[14] = color.red * 255;
  stipple_data[1] = stipple_data[13] = color.green * 255;
  stipple_data[0] = stipple_data[12] = color.blue * 255;
  surface = cairo_image_surface_create_for_data (stipple_data,
						 CAIRO_FORMAT_ARGB32,
						 2, 2, 8);
  pattern = cairo_pattern_create_for_surface (surface);
  cairo_surface_destroy (surface);
  cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);

  return pattern;
}

static void
setup_rectangles (GooCanvasItem *root)
{
  GooCanvasItem *item;
  cairo_pattern_t *pattern;
  static guchar stipple_data[16] = {
    0, 0, 0, 255,   0, 0, 0, 0,   0, 0, 0, 0,     0, 0, 0, 255
  };
  GooCanvasLineDash *dash;

  item = goo_canvas_rect_new (root, 20, 30, 50, 30,
			      "stroke-color", "red",
			      "line-width", 8.0,
			      "tooltip", "Red stroked rectangle",
			      NULL);
  setup_item_signals (item);

  pattern = create_stipple ("mediumseagreen", stipple_data);
  item = goo_canvas_rect_new (root, 90, 40, 90, 60,
			      "fill-pattern", pattern,
			      "stroke-color", "black",
			      "line-width", 4.0,
			      "tooltip", "Medium Sea Green stippled rectangle",
			      NULL);
  cairo_pattern_destroy (pattern);
  setup_item_signals (item);

  item = goo_canvas_rect_new (root, 10, 80, 70, 60,
			      "fill-color", "steelblue",
			      /*"fill-pattern", NULL,*/
			      "tooltip", "Steel Blue rectangle",
			      NULL);
  setup_item_signals (item);

  dash = goo_canvas_line_dash_new (2, 5.0, 2.0);
  goo_canvas_line_dash_set_offset (dash, 2.0);
  item = goo_canvas_rect_new (root, 20, 90, 70, 60,
			      "fill-color-rgba", 0x3cb37180,
			      "line-dash", dash,
			      "stroke-color", "blue",
			      "line-width", 2.0,
			      "tooltip", "Partially transparent rectangle",
			      NULL);
  goo_canvas_line_dash_unref (dash);
  setup_item_signals (item);

  item = goo_canvas_rect_new (root, 110, 80, 50, 30,
			      "radius-x", 20.0,
			      "radius-y", 10.0,
			      "stroke-color", "yellow",
			      "fill-color-rgba", 0x3cb3f180,
			      "tooltip", "Rectangle with rounded corners",
			      NULL);
  setup_item_signals (item);

  item = goo_demo_item_new (root, 30, 20, 50, 30,
			    "fill-color", "yellow",
			    "tooltip", "Yellow demo item",
			    NULL);
  setup_item_signals (item);
}


static void
setup_ellipses (GooCanvasItem *root)
{
  GooCanvasItem *ellipse1, *ellipse3;
  cairo_pattern_t *pattern;
  static guchar stipple_data[16] = {
    0, 0, 0, 255,   0, 0, 0, 0,
    0, 0, 0, 0,     0, 0, 0, 255
  };

#if 1
  ellipse1 = goo_canvas_ellipse_new (root, 245, 45, 25, 15,
				     "stroke-color", "goldenrod",
				     "line-width", 8.0,
				     NULL);
  setup_item_signals (ellipse1);
#endif

  ellipse2 = goo_canvas_ellipse_new (root, 335, 70, 45, 30,
				     "fill-color", "wheat",
				     "stroke-color", "midnightblue",
				     "line-width", 4.0,
				     "title", "An ellipse",
				     NULL);
#if 0
  goo_canvas_item_rotate (ellipse2, 30, 0, 0);
#endif
  setup_item_signals (ellipse2);

#if 1
  pattern = create_stipple ("cadetblue", stipple_data);
  ellipse3 = goo_canvas_ellipse_new (root, 245, 110, 35, 30,
				     "fill-pattern", pattern,
				     "stroke-color", "black",
				     "line-width", 1.0,
				     NULL);
  cairo_pattern_destroy (pattern);
  setup_item_signals (ellipse3);
#endif
}


#define VERTICES 10
#define RADIUS 60.0

static void
polish_diamond (GooCanvasItem *root)
{
  GooCanvasItem *group, *item G_GNUC_UNUSED;
  int i, j;
  double a, x1, y1, x2, y2;

  group = goo_canvas_group_new (root,
				"line-width", 1.0,
				"line-cap", CAIRO_LINE_CAP_ROUND,
				NULL);
  goo_canvas_item_translate (group, 270, 230);
  setup_item_signals (group);

  for (i = 0; i < VERTICES; i++) {
    a = 2.0 * M_PI * i / VERTICES;
    x1 = RADIUS * cos (a);
    y1 = RADIUS * sin (a);

    for (j = i + 1; j < VERTICES; j++) {
      a = 2.0 * M_PI * j / VERTICES;
      x2 = RADIUS * cos (a);
      y2 = RADIUS * sin (a);
      item = goo_canvas_polyline_new_line (group, x1, y1, x2, y2, NULL);
    }
  }
}


#define SCALE 7.0

static void
make_hilbert (GooCanvasItem *root)
{
  char hilbert[] = "urdrrulurulldluuruluurdrurddldrrruluurdrurddldrddlulldrdldrrurd";
  char *c;
  double *pp, *p;
  GooCanvasItem *item;
  GooCanvasPoints *points;
  cairo_pattern_t *pattern;
  static guchar stipple_data[16] = {
    0, 0, 0, 255,   0, 0, 0, 0,   0, 0, 0, 0,     0, 0, 0, 255
  };

  points = goo_canvas_points_new (strlen (hilbert) + 1);
  points->coords[0] = 340.0;
  points->coords[1] = 290.0;

  pp = points->coords;
  for (c = hilbert, p = points->coords + 2; *c; c++, p += 2, pp += 2)
    switch (*c) {
    case 'u':
      p[0] = pp[0];
      p[1] = pp[1] - SCALE;
      break;

    case 'd':
      p[0] = pp[0];
      p[1] = pp[1] + SCALE;
      break;

    case 'l':
      p[0] = pp[0] - SCALE;
      p[1] = pp[1];
      break;

    case 'r':
      p[0] = pp[0] + SCALE;
      p[1] = pp[1];
      break;
    }

  pattern = create_stipple ("red", stipple_data);
  item = goo_canvas_polyline_new (root, FALSE, 0,
				  "points", points,
				  "line-width", 4.0,
				  "stroke-pattern", pattern,
				  "line-cap", CAIRO_LINE_CAP_SQUARE,
				  "line-join", CAIRO_LINE_JOIN_MITER,
				  NULL);
  cairo_pattern_destroy (pattern);
  setup_item_signals (item);

  goo_canvas_points_unref (points);
}

static void
setup_lines (GooCanvasItem *root)
{
  GooCanvasItem *polyline1, *polyline2, *polyline3, *polyline4, *polyline5;

  polish_diamond (root);
  make_hilbert (root);

  /* Arrow tests */

  polyline1 = goo_canvas_polyline_new (root, FALSE, 4,
				       340.0, 170.0,
				       340.0, 230.0,
				       390.0, 230.0,
				       390.0, 170.0,
				       "stroke-color", "midnightblue",
				       "line-width", 3.0,
				       "start-arrow", TRUE,
				       "end-arrow", TRUE,
				       "arrow-tip-length", 3.0,
				       "arrow-length", 4.0,
				       "arrow-width", 3.5,
				       NULL);
  setup_item_signals (polyline1);

  polyline2 = goo_canvas_polyline_new (root, FALSE, 2,
				       356.0, 180.0,
				       374.0, 220.0,
				       "stroke-color", "blue",
				       "line-width", 1.0,
				       "start-arrow", TRUE,
				       "end-arrow", TRUE,
				       "arrow-tip-length", 5.0,
				       "arrow-length", 6.0,
				       "arrow-width", 6.0,
				       NULL);
  setup_item_signals (polyline2);

  polyline3 = goo_canvas_polyline_new (root, FALSE, 2,
				       356.0, 220.0,
				       374.0, 180.0,
				       "stroke-color", "blue",
				       "line-width", 1.0,
				       "start-arrow", TRUE,
				       "end-arrow", TRUE,
				       "arrow-tip-length", 5.0,
				       "arrow-length", 6.0,
				       "arrow-width", 6.0,
				       NULL);
  setup_item_signals (polyline3);

  /* Test polyline without any coords. */
  polyline4 = goo_canvas_polyline_new (root, FALSE, 0,
				       NULL);
  setup_item_signals (polyline4);

  /* Test polyline with 1 coord and arrows. */
  polyline5 = goo_canvas_polyline_new (root, FALSE, 1,
				       356.0, 220.0,
				       "start-arrow", TRUE,
				       "end-arrow", TRUE,
				       NULL);
  setup_item_signals (polyline5);
}


static void
setup_polygons (GooCanvasItem *root)
{
  GooCanvasItem *polyline1, *polyline2;
  GooCanvasPoints *points;
  cairo_pattern_t *pattern;
  static guchar stipple_data[16] = {
    0, 0, 0, 255,   0, 0, 0, 0,   0, 0, 0, 0,     0, 0, 0, 255
  };

  points = goo_canvas_points_new (3);
  points->coords[0] = 210.0;
  points->coords[1] = 320.0;
  points->coords[2] = 210.0;
  points->coords[3] = 380.0;
  points->coords[4] = 260.0;
  points->coords[5] = 350.0;
  pattern = create_stipple ("blue", stipple_data);
  polyline1 = goo_canvas_polyline_new (root, TRUE, 0,
				       "line-width", 1.0,
				       "points", points,
				       "fill_pattern", pattern,
				       "stroke_color", "black",
				       NULL);
  cairo_pattern_destroy (pattern);
  goo_canvas_points_unref (points);
  setup_item_signals (polyline1);
  

  points = goo_canvas_points_new (14);
  points->coords[0] = 270.0;
  points->coords[1] = 330.0;
  points->coords[2] = 270.0;
  points->coords[3] = 430.0;
  points->coords[4] = 390.0;
  points->coords[5] = 430.0;
  points->coords[6] = 390.0;
  points->coords[7] = 330.0;
  points->coords[8] = 310.0;
  points->coords[9] = 330.0;
  points->coords[10] = 310.0;
  points->coords[11] = 390.0;
  points->coords[12] = 350.0;
  points->coords[13] = 390.0;
  points->coords[14] = 350.0;
  points->coords[15] = 370.0;
  points->coords[16] = 330.0;
  points->coords[17] = 370.0;
  points->coords[18] = 330.0;
  points->coords[19] = 350.0;
  points->coords[20] = 370.0;
  points->coords[21] = 350.0;
  points->coords[22] = 370.0;
  points->coords[23] = 410.0;
  points->coords[24] = 290.0;
  points->coords[25] = 410.0;
  points->coords[26] = 290.0;
  points->coords[27] = 330.0;
  polyline2 = goo_canvas_polyline_new (root, TRUE, 0,
				       "points", points,
				       "fill_color", "tan",
				       "stroke_color", "black",
				       "line-width", 3.0,
				       NULL);
  goo_canvas_points_unref (points);
  setup_item_signals (polyline2);
}


static GooCanvasItem *
make_anchor (GooCanvasItem *root, double x, double y)
{
  GooCanvasItem *group, *item;
  cairo_matrix_t transform = { 0.8, 0.2, -0.3, 0.5, x, y };

  group = goo_canvas_group_new (root, NULL);
  goo_canvas_item_translate (group, x, y);
#if 1
  g_object_set (group,
		"transform", &transform,
		NULL);
#endif

  item = goo_canvas_rect_new (group, -2.5, -2.5, 4, 4,
			      "line-width", 1.0,
			      NULL);
  setup_item_signals (item);

  g_signal_connect (group, "scroll_event",
		    G_CALLBACK (on_scroll), NULL);

  return group;
}

static void 
setup_texts (GooCanvasItem *root)
{
  GooCanvasItem *item;
  cairo_pattern_t *pattern;
  static guchar stipple_data[16] = {
    0, 0, 0, 255,   0, 0, 0, 0,   0, 0, 0, 0,     0, 0, 0, 255
  };

#if 1
  pattern = create_stipple ("blue", stipple_data);
  item = goo_canvas_text_new (make_anchor (root, 420, 20),
			      "Anchor NW", 0, 0, -1, GOO_CANVAS_ANCHOR_NW,
			      "font", "Sans Bold 24",
			      "fill_pattern", pattern,
			      NULL);
  cairo_pattern_destroy (pattern);
  setup_item_signals (item);

  item = goo_canvas_text_new (make_anchor (root, 470, 75),
			      "Anchor center\nJustify center\nMultiline text\nb8bit text ÅÄÖåäö",
			      0, 0, -1, GOO_CANVAS_ANCHOR_CENTER,
			      "font", "monospace bold 14",
			      "alignment", PANGO_ALIGN_CENTER,
			      "fill_color", "firebrick",
			      NULL);
  setup_item_signals (item);
#endif

#if 0
  item = goo_canvas_text_new (make_anchor (root, 590, 140),
			      "Clipped text\nClipped text\nClipped text\nClipped text\nClipped text\nClipped text",
			      0, 0, -1, GOO_CANVAS_ANCHOR_SE,
			      "font", "Sans 12",
			      /*"clip", TRUE,*/
			      /*"clip_width", 50.0,*/
			      /*"clip_height", 55.0,*/
			      /*"x_offset", 10.0,*/
			      "fill_color", "darkgreen",
			      NULL);
  setup_item_signals (item);
#endif

#if 1
  textitem = goo_canvas_text_new (make_anchor (root, 420, 240),
				  "This is a very long paragraph that will need to be wrapped over several lines so we can see what happens to line-breaking as the view is zoomed in and out.",
				  0, 0, 180, GOO_CANVAS_ANCHOR_W,
				  "font", "Sans 12",
				  "fill_color", "goldenrod",
				  NULL);
  setup_item_signals (textitem);
#endif

#if 1
  textitem = goo_canvas_text_new (root,
				  "Ellipsized text.",
				  20, 420, 115, GOO_CANVAS_ANCHOR_W,
				  "font", "Sans 12",
				  "fill_color", "blue",
				  "ellipsize", PANGO_ELLIPSIZE_END,
				  NULL);
  setup_item_signals (textitem);
#endif
}


static void
setup_invisible_texts (GooCanvasItem *root)
{
  goo_canvas_text_new (root, "Visible above 0.8x", 500, 330, -1,
		       GOO_CANVAS_ANCHOR_CENTER,
		       "visibility", GOO_CANVAS_ITEM_VISIBLE_ABOVE_THRESHOLD,
		       "visibility-threshold", 0.8,
		       NULL);
  goo_canvas_rect_new (root, 410.5, 322.5, 180, 15,
		       "line-width", 1.0,
		       "visibility", GOO_CANVAS_ITEM_VISIBLE_ABOVE_THRESHOLD,
		       "visibility-threshold", 0.8,
		       NULL);

  goo_canvas_text_new (root, "Visible above 1.5x", 500, 350, -1,
		       GOO_CANVAS_ANCHOR_CENTER,
		       "visibility", GOO_CANVAS_ITEM_VISIBLE_ABOVE_THRESHOLD,
		       "visibility-threshold", 1.5,
		       NULL);
  goo_canvas_rect_new (root, 410.5, 342.5, 180, 15,
		       "line-width", 1.0,
		       "visibility", GOO_CANVAS_ITEM_VISIBLE_ABOVE_THRESHOLD,
		       "visibility-threshold", 1.5,
		       NULL);

  goo_canvas_text_new (root, "Visible above 3.0x", 500, 370, -1,
		       GOO_CANVAS_ANCHOR_CENTER,
		       "visibility", GOO_CANVAS_ITEM_VISIBLE_ABOVE_THRESHOLD,
		       "visibility-threshold", 3.0,
		       NULL);
  goo_canvas_rect_new (root, 410.5, 362.5, 180, 15,
		       "line-width", 1.0,
		       "visibility", GOO_CANVAS_ITEM_VISIBLE_ABOVE_THRESHOLD,
		       "visibility-threshold", 3.0,
		       NULL);

  /* This should never be seen. */
  goo_canvas_text_new (root, "Always Invisible", 500, 390, -1,
		       GOO_CANVAS_ANCHOR_CENTER,
		       "visibility", GOO_CANVAS_ITEM_INVISIBLE,
		       NULL);
  goo_canvas_rect_new (root, 410.5, 350.5, 180, 15,
		       "line-width", 1.0,
		       "visibility", GOO_CANVAS_ITEM_INVISIBLE,
		       NULL);
}


static void
plant_flower (GooCanvasItem *root, double x, double y, GooCanvasAnchorType anchor, gdouble opacity)
{
  cairo_pattern_t *pattern;
  cairo_surface_t *surface;
  GooCanvasItem *image;
  double w, h;

  surface = cairo_image_surface_create_from_png ("flower.png");
  w = cairo_image_surface_get_width (surface);
  h = cairo_image_surface_get_height (surface);

  pattern = cairo_pattern_create_for_surface (surface);
  cairo_surface_destroy (surface);

  image = goo_canvas_image_new (root, NULL, x, y,
			       "pattern", pattern,
			       "width", w * 1.5,
			       "height", h * 2,
				"scale-to-fit", TRUE,
				"alpha", opacity,
			       NULL);
  cairo_pattern_destroy (pattern);
  setup_item_signals (image);
}

static void
setup_images (GooCanvasItem *root)
{
  GdkPixbuf *im;
  GooCanvasItem *image;

  im = gdk_pixbuf_new_from_file ("toroid.png", NULL);

  if (im)
    {
      double w = gdk_pixbuf_get_width (im);
      double h = gdk_pixbuf_get_height (im);
      image = goo_canvas_image_new (root, im, 100.0 - w / 2, 225.0 - h / 2,
				   "width", w,
				   "height", h,
				   /* "anchor", GOO_CANVAS_ANCHOR_CENTER, */
				   NULL);
      g_object_unref(im);
      setup_item_signals (image);
    }
  else
    g_warning ("Could not find the toroid.png sample file");

  plant_flower (root,  20.0, 170.0, GOO_CANVAS_ANCHOR_NW, 0.3);
  plant_flower (root, 180.0, 170.0, GOO_CANVAS_ANCHOR_NE, 0.5);
  plant_flower (root,  20.0, 280.0, GOO_CANVAS_ANCHOR_SW, 0.7);
  plant_flower (root, 180.0, 280.0, GOO_CANVAS_ANCHOR_SE, 1.0);
}


static void
setup_static_items (GooCanvas *canvas)
{
  GooCanvasItem *static_root, *group, *item;

  static_root = goo_canvas_get_static_root_item (canvas);

  /* All static items in one place in the canvas should be placed in the same
     group. */
  group = goo_canvas_group_new (static_root, NULL);

  item = goo_canvas_polyline_new_line (group,
				       40.0, 410.0,
				       40.0, 330.0,
				       "stroke-color", "midnightblue",
				       "line-width", 3.0,
				       "end-arrow", TRUE,
				       "arrow-tip-length", 3.0,
				       "arrow-length", 4.0,
				       "arrow-width", 3.5,
				       NULL);
  setup_item_signals (item);

  item = goo_canvas_polyline_new_line (group,
				       32.0, 370.0,
				       48.0, 370.0,
				       "stroke-color", "midnightblue",
				       "line-width", 3.0,
				       NULL);
  setup_item_signals (item);

  item = goo_canvas_text_new (group, "N", 40, 320, -1, GOO_CANVAS_ANCHOR_S,
			      "font", "Sans 12",
			      NULL);
  setup_item_signals (item);
}


/* This checks that the rgba color properties work properly, i.e. the value
   written to them is the same when read back out. */
static void
test_color_properties (GooCanvasItem *root)
{
#if 0
  GooCanvasItem *item;
  guint red, green, blue, alpha, rgba_in, rgba_out;

  item = goo_canvas_rect_new (root, 20, 30, 50, 30,
			      NULL);

  for (red = 0; red < 256; red += 15)
    {
      for (green = 0; green < 256; green += 15)
	{     
	  for (blue = 0; blue < 256; blue ++)
	    {     
	      for (alpha = 0; alpha < 256; alpha += 15)
		{     
		  rgba_in = (red << 24) + (green << 16) + (blue << 8) + alpha;
		  g_object_set (item, "fill_color_rgba", rgba_in, NULL);
		  rgba_out = 0;
		  g_object_get (item, "fill_color_rgba", &rgba_out, NULL);

		  if (rgba_in != rgba_out)
		    g_print ("ERROR: rgba in: %.8X out: %.8X\n", rgba_in, rgba_out);
		}
	    }
	}
    }
#endif
}

static void
test_simple_transforms (GooCanvasItem *root)
{
#if 0
  GooCanvasItem *item;
  int rotation, x, y;
  double scale, x_out, y_out, scale_out, rotation_out, max_error = 0.0000001;

  item = goo_canvas_rect_new (root, 20, 30, 50, 30,
			      NULL);

  for (rotation = 0; rotation < 360; rotation += 20)
    {
      scale = 0.1;
      while (scale < 10)
	{
	  g_print ("Rotation: %i Scale: %g\n", rotation, scale);

	  for (x = -100; x < 100; x += 10)
	    {
	      for (y = -100; y < 100; y += 10)
		{
		  goo_canvas_item_set_simple_transform (item, x, y, scale,
							rotation);

		  goo_canvas_item_get_simple_transform (item, &x_out, &y_out,
							&scale_out,
							&rotation_out);

		  if (fabs (x_out - x) > max_error
		      || fabs (y_out - y) > max_error
		      || fabs (scale_out - scale) > max_error
		      || fabs (rotation_out - rotation) > max_error)
		    {
		      g_print ("IN X:%i Y:%i Scale:%g Rotation:%i -> %g, %g, %g, %g\n", x, y, scale, rotation, x_out, y_out, scale_out, rotation_out);
		    }
		}
	    }
	  scale += 0.1;
	}
    }
#endif
}


static void
setup_grids (GooCanvasItem *root)
{
  GooCanvasItem *item G_GNUC_UNUSED;

  item = goo_canvas_grid_new (root, 80, 310, 90, 90, 10, 10, 5, 5,
			      "stroke-color", "yellow",
			      "fill-color", "pink",
			      "border-width", 2.0,
			      "border-color", "red",
			      "vert-grid-line-color", "lightblue",
			      "horz-grid-line-width", 1.0,
			      "vert-grid-line-width", 1.0,
			      "vert-grid-lines-on-top", TRUE,
			      NULL);
}


static void
setup_canvas (GooCanvas *canvas)
{
  GooCanvasItem *root;

  root = goo_canvas_get_root_item (canvas);
  g_signal_connect (root, "button_press_event",
		    G_CALLBACK (on_background_button_press), NULL);

  /* Setup canvas items */
#if 1
  setup_divisions (root);
  setup_rectangles (root);
  setup_ellipses (root);
  setup_lines (root);
  setup_polygons (root);
  setup_texts (root); 
  setup_images (root);
  setup_invisible_texts (root);
  setup_static_items (canvas);
  setup_grids (root);
#endif

  test_color_properties (root);
  test_simple_transforms (root);
}


static gboolean
on_delete_event (GtkWidget *window,
		 GdkEvent *event,
		 gpointer unused_data)
{
  gtk_main_quit ();
  return FALSE;
}


static GtkWidget*
create_window ()
{
  GtkWidget *window, *notebook;

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size (GTK_WINDOW (window), 640, 600);
  gtk_widget_show (window);
  g_signal_connect (window, "delete_event", G_CALLBACK (on_delete_event),
		    NULL);

  notebook = gtk_notebook_new ();
  gtk_widget_show (notebook);
  gtk_container_add (GTK_CONTAINER (window), notebook);

#if 1
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook),
			    create_canvas_primitives (),
			    gtk_label_new ("Primitives"));
#endif
#if 1
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook),
			    create_canvas_arrowhead (),
			    gtk_label_new ("Arrowhead"));
#endif
#if 1
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook),
			    create_canvas_fifteen (),
			    gtk_label_new ("Fifteen"));
#endif
#if 1
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook),
			    create_canvas_features (),
			    gtk_label_new ("Reparent"));
#endif
#if 1
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook),
			    create_canvas_scalability (),
			    gtk_label_new ("Scalability"));
#endif
#if 1
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook),
			    create_grabs_page (),
			    gtk_label_new ("Grabs"));
#endif
#if 1
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook),
			    create_events_page (),
			    gtk_label_new ("Events"));
#endif
#if 1
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook),
			    create_paths_page (),
			    gtk_label_new ("Paths"));
#endif
#if 1
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook),
			    create_focus_page (),
			    gtk_label_new ("Focus"));
#endif
#if 1
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook),
			    create_animation_page (),
			    gtk_label_new ("Animation"));
#endif
#if 1
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook),
			    create_clipping_page (),
			    gtk_label_new ("Clipping"));
#endif
#if 1
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook),
			    create_table_page (),
			    gtk_label_new ("Table"));
#endif
#if 1
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook),
			    create_large_items_page (),
			    gtk_label_new ("Large Items"));
#endif

  return window;
}


int
main (int argc, char *argv[])
{
  GtkWidget *window G_GNUC_UNUSED;

  gtk_init (&argc, &argv);

  window = create_window ();

  gtk_main ();
  return 0;
}
