/* wacomconfig.c */
/* @author : Julius Angres */
/* 09.04.2018 */

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

char *device_name;

static gint delete_Event(GtkWidget *widget, GdkEvent event, gpointer daten) {
  return FALSE;
}

static void end (GtkWidget *widget, gpointer daten) {
  g_print ("terminating program\n");
  gtk_main_quit ();
}

// TODO in dev mode only print the commands to be executed by the system utility [use sprintf to build cmd string]
static void tracking_mode (GtkWidget *widget, gpointer data) {
  gint i = gtk_combo_box_get_active ((GtkComboBox*) widget);
  char cmd[255];
  if (i==1) {
    sprintf (cmd, "xsetwacom --set \"%s stylus\" \"Mode\" \"relative\"", device_name);
    system (cmd);
  } else {
    sprintf (cmd, "xsetwacom --set \"%s stylus\" \"Mode\" \"absolute\"", device_name);
    system (cmd);
  }
}

static void orientation_mode (GtkWidget *widget, gpointer data) {
  char cmd[255];
  if (gtk_switch_get_active ((GtkSwitch*) widget)) {
    sprintf (cmd, "xsetwacom --set \"%s stylus\" \"Rotate\" \"half\"", device_name);
    system (cmd);
  } else {
    sprintf (cmd, "xsetwacom --set \"%s stylus\" \"Rotate\" \"none\"", device_name);
    system (cmd);
  }
}

static void top_btn_action (GtkWidget *widget, gpointer data) {
  g_print ("debug :: ready to set top button action");
  //TODO add button mapping
}

static void lower_btn_action (GtkWidget *widget, gpointer data) {
  g_print ("debug :: ready to set lower button action");
  //TODO add button mapping
}

/* PressureCurve parameter of xsetwacom. 0->soft, 1->middle, 2->firm */
static void eraser_pressure_value (GtkAdjustment *adj, gpointer data) {
  gdouble val = gtk_adjustment_get_value(adj);
  char cmd[255];
  if (val == 0.0) {
    sprintf (cmd, "xsetwacom -set \"%s eraser\" \"PressureCurve\" \"0 75 25 100\"", device_name);
    system (cmd);
  } else if (val == 0.1) {
    sprintf (cmd, "xsetwacom -set \"%s eraser\" \"PressureCurve\" \"0 0 100 100\"", device_name);
    system (cmd);
  } else if (val > 0.1) {
    sprintf (cmd, "xsetwacom -set \"%s eraser\" \"PressureCurve\" \"75 0 100 25\"", device_name);
    system (cmd);
  } else {
    g_print ("invalid value\n");
  }
}

/* PressureCurve parameter of xsetwacom. 0->soft, 1->middle, 2->firm */
static void pen_pressure_value (GtkAdjustment *adj, gpointer data) {
  gdouble val = gtk_adjustment_get_value(adj);
  char cmd[255];
  if (val == 0.0) {
    sprintf (cmd, "xsetwacom -set \"%s stylus\" \"PressureCurve\" \"0 75 25 100\"", device_name);
    system (cmd);
  } else if (val == 0.1) {
    sprintf (cmd, "xsetwacom -set \"%s stylus\" \"PressureCurve\" \"0 0 100 100\"", device_name);
    system (cmd);
  } else if (val > 0.1) {
    sprintf (cmd, "xsetwacom -set \"%s stylus\" \"PressureCurve\" \"75 0 100 25\"", device_name);
    system (cmd);
  } else {
    g_print ("invalid value\n");
  }
}


int main (int argc, char **argv) {
  GtkWindow *win;
  GdkPixbuf *pic;
  GtkGrid *grid, *grid_nodev;
  GtkWidget *combobox, *lefthanded, *img_touchpad, *img_stylus, *top_btn_box, *lower_btn_box, *eraser_scale, *pen_scale;
  GtkLabel *option_label, *nodev_status, *nodev_msg, *orientation, *eraser_pressure, *top_btn, *lower_btn, *tip_pressure, *pen_label, *device_label;
  GtkAdjustment *adj1, *adj2;
  gtk_init (&argc, &argv);
  
  /* use output of xsetwacom to check whether a device was found */
  FILE* fp = popen("xsetwacom --list devices | grep -i pen | cut -d \" \" -f1-4 | uniq","r");
  // for testing use date command as it will always return a non-epmty result
  //FILE *fp = popen("date","r");
  char buf[255];
  fgets (buf, 255, fp);
  device_name = strtok(buf,"\n");
  
  /* load graphic to pixbuf */
  pic = gdk_pixbuf_new_from_file ("/usr/share/icons/wacom-conftool/wacom-icon.png", NULL);
  /* define window properties WIDTH=740, HEIGHT=310 (nodev) HEIGHT=435 (dev) */
  win = g_object_new (GTK_TYPE_WINDOW,
                      "title", "Wacom Tablet",
                      "default-width", 740,
                      "default-height", 310,
                      "resizable", TRUE,
                      "window-position", GTK_WIN_POS_CENTER,
                      "border-width", 10, "icon", pic, NULL);
  /* set up grids for the components */
  grid = (GtkGrid*) gtk_grid_new ();
  gtk_grid_set_row_spacing (grid, 20);
  gtk_grid_set_column_spacing (grid, 10);
  grid_nodev = (GtkGrid*) gtk_grid_new ();
  gtk_grid_set_row_spacing (grid_nodev, 20);
  gtk_grid_set_column_spacing (grid_nodev, 10);
  
  /* create combobox for tracking mode and attach items to it */
  combobox = gtk_combo_box_text_new ();
  gtk_combo_box_text_append_text ((GtkComboBoxText*) combobox, "Touchpad (absolute)");
  gtk_combo_box_text_append_text ((GtkComboBoxText*) combobox, "Touchpad (relative)");
  gtk_combo_box_set_active ((GtkComboBox*) combobox, 0);
  
  /* create comboboxes for button actions */
  top_btn_box = gtk_combo_box_text_new ();
  lower_btn_box = gtk_combo_box_text_new ();
  gtk_combo_box_text_append_text ((GtkComboBoxText*) top_btn_box, "No Action");
  gtk_combo_box_text_append_text ((GtkComboBoxText*) top_btn_box, "Left Mouse Button Click");
  gtk_combo_box_text_append_text ((GtkComboBoxText*) top_btn_box, "Middle Mouse Button Click");
  gtk_combo_box_text_append_text ((GtkComboBoxText*) top_btn_box, "Right Mouse Button Click");
  gtk_combo_box_text_append_text ((GtkComboBoxText*) top_btn_box, "Scroll Up");
  gtk_combo_box_text_append_text ((GtkComboBoxText*) top_btn_box, "Scroll Down");
  gtk_combo_box_text_append_text ((GtkComboBoxText*) top_btn_box, "Scroll Left");
  gtk_combo_box_text_append_text ((GtkComboBoxText*) top_btn_box, "Scroll Right");
  gtk_combo_box_text_append_text ((GtkComboBoxText*) top_btn_box, "Back");
  gtk_combo_box_text_append_text ((GtkComboBoxText*) top_btn_box, "Forward");
  gtk_combo_box_text_append_text ((GtkComboBoxText*) lower_btn_box, "No Action");
  gtk_combo_box_text_append_text ((GtkComboBoxText*) lower_btn_box, "Left Mouse Button Click");
  gtk_combo_box_text_append_text ((GtkComboBoxText*) lower_btn_box, "Middle Mouse Button Click");
  gtk_combo_box_text_append_text ((GtkComboBoxText*) lower_btn_box, "Right Mouse Button Click");
  gtk_combo_box_text_append_text ((GtkComboBoxText*) lower_btn_box, "Scroll Up");
  gtk_combo_box_text_append_text ((GtkComboBoxText*) lower_btn_box, "Scroll Down");
  gtk_combo_box_text_append_text ((GtkComboBoxText*) lower_btn_box, "Scroll Left");
  gtk_combo_box_text_append_text ((GtkComboBoxText*) lower_btn_box, "Scroll Right");
  gtk_combo_box_text_append_text ((GtkComboBoxText*) lower_btn_box, "Back");
  gtk_combo_box_text_append_text ((GtkComboBoxText*) lower_btn_box, "Forward");
  gtk_combo_box_set_active ((GtkComboBox*) top_btn_box, 0);
  gtk_combo_box_set_active ((GtkComboBox*) lower_btn_box, 0);
  
  /* create switch to enable left-handed orientation */
  lefthanded = gtk_switch_new ();
  gtk_switch_set_active ((GtkSwitch*) lefthanded, FALSE);
  
  /* create scales for eraser and pen pressure */
  adj1 = gtk_adjustment_new (0.0, 0.0, 1.2, 0.1, 1.0, 1.0);
  adj2 = gtk_adjustment_new (0.0, 0.0, 1.2, 0.1, 1.0, 1.0);
  eraser_scale = gtk_scale_new (GTK_ORIENTATION_HORIZONTAL, GTK_ADJUSTMENT (adj1));
  pen_scale = gtk_scale_new (GTK_ORIENTATION_HORIZONTAL, GTK_ADJUSTMENT (adj2));
  gtk_scale_add_mark ((GtkScale*)eraser_scale, 0.0, GTK_POS_LEFT, "soft");
  gtk_scale_add_mark ((GtkScale*)eraser_scale, 1.2, GTK_POS_LEFT, "firm");
  gtk_scale_set_draw_value ((GtkScale*)eraser_scale, FALSE);
  gtk_scale_add_mark ((GtkScale*)pen_scale, 0.0, GTK_POS_LEFT, "soft");
  gtk_scale_add_mark ((GtkScale*)pen_scale, 1.2, GTK_POS_LEFT, "firm");
  gtk_scale_set_draw_value ((GtkScale*)pen_scale, FALSE);
  
  /* create images for leftmost column */
  img_touchpad = gtk_image_new_from_file ("/usr/share/icons/wacom-conftool/img_touchpad.png");
  img_stylus = gtk_image_new_from_file ("/usr/share/icons/wacom-conftool/img_general_pen.png");
  
  /* create text labels */
  option_label = g_object_new ( GTK_TYPE_LABEL, "label", "Tracking Mode ", NULL );
  nodev_status = g_object_new (GTK_TYPE_LABEL, "label", "No tablet detected", NULL);
  nodev_msg = g_object_new (GTK_TYPE_LABEL, "label", "Please plug in or turn on your Wacom tablet", NULL);
  orientation = g_object_new (GTK_TYPE_LABEL, "label", "Left-handed orientation ", NULL);
  eraser_pressure = g_object_new (GTK_TYPE_LABEL, "label", "Eraser Pressure Feel ", NULL);
  top_btn = g_object_new (GTK_TYPE_LABEL, "label", "Top Button ", NULL);
  lower_btn = g_object_new (GTK_TYPE_LABEL, "label", "Lower Button", NULL);
  tip_pressure = g_object_new (GTK_TYPE_LABEL, "label", "Tip Pressure Feel ", NULL);
  pen_label = g_object_new (GTK_TYPE_LABEL, "label", "General Pen", NULL);
  device_label = g_object_new (GTK_TYPE_LABEL, "label", device_name, NULL);
  
  /* define signal handler */
  g_signal_connect ( win, "delete-event", G_CALLBACK (delete_Event), NULL);
  g_signal_connect ( win, "destroy", G_CALLBACK (end), NULL );
  g_signal_connect ( combobox, "changed", G_CALLBACK (tracking_mode), NULL);
  g_signal_connect ( lefthanded, "state-set", G_CALLBACK (orientation_mode), NULL);
  g_signal_connect ( top_btn_box, "changed", G_CALLBACK (top_btn_action), NULL);
  g_signal_connect ( lower_btn_box, "changed", G_CALLBACK (lower_btn_action), NULL);
  g_signal_connect ( adj1, "value-changed", G_CALLBACK (eraser_pressure_value), NULL);
  g_signal_connect ( adj2, "value-changed", G_CALLBACK (pen_pressure_value), NULL);
  
  /* pack components */
  // iff no device is detected, length of buffer will be 0
  if (strlen(buf) == 0) {
    gtk_container_add (GTK_CONTAINER (win), GTK_WIDGET (grid_nodev));
    gtk_grid_attach (grid_nodev, GTK_WIDGET (nodev_status), 0, 0, 1, 1);
    gtk_grid_attach (grid_nodev, GTK_WIDGET (nodev_msg), 0, 1, 1, 1);
  } else {
    gtk_window_resize (GTK_WINDOW (win), 740, 435);
    gtk_container_add (GTK_CONTAINER (win), GTK_WIDGET (grid));
    gtk_grid_attach (grid, GTK_WIDGET (device_label), 0, 0, 2, 1);
    gtk_grid_attach (grid, GTK_WIDGET (img_touchpad), 0, 1, 1, 3);
    gtk_grid_attach (grid, GTK_WIDGET (option_label), 1, 1, 1, 1);
    gtk_grid_attach (grid, GTK_WIDGET (combobox), 2, 1, 1, 1);
    gtk_grid_attach (grid, GTK_WIDGET (orientation), 1, 2, 1, 1);
    gtk_grid_attach (grid, GTK_WIDGET (lefthanded), 2, 2, 1, 1);
    gtk_grid_attach (grid, GTK_WIDGET (pen_label), 0, 4, 1, 1);
    gtk_grid_attach (grid, GTK_WIDGET (img_stylus), 0, 5, 1, 4);
    gtk_grid_attach (grid, GTK_WIDGET (eraser_pressure), 1, 5, 1, 1);
    gtk_grid_attach (grid, GTK_WIDGET (eraser_scale), 2, 5, 1, 1);
    gtk_grid_attach (grid, GTK_WIDGET (top_btn), 1, 6, 1, 1);
    gtk_grid_attach (grid, GTK_WIDGET (top_btn_box), 2, 6, 1, 1);
    gtk_grid_attach (grid, GTK_WIDGET (lower_btn), 1, 7, 1, 1);
    gtk_grid_attach (grid, GTK_WIDGET (lower_btn_box), 2, 7, 1, 1);
    gtk_grid_attach (grid, GTK_WIDGET (tip_pressure), 1, 8, 1, 1);
    gtk_grid_attach (grid, GTK_WIDGET (pen_scale), 2, 8, 1, 1);
  }
  /* show window */
  gtk_widget_show_all (GTK_WIDGET (win));
  /* gtk main loop */
  gtk_main ();
  return 0;
}
