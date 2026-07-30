#include <gtk/gtk.h>

size_t cc = 0;

void end_program(GtkWindow* win, gpointer ptr) {
  gtk_main_quit();
}

GtkWidget *txt;

void count_clicked(GtkButton* btn, gpointer ptr) {
  char buffer[40];
  sprintf(buffer, "clicked {%lu}\n", ++cc);
  gtk_label_set_text(GTK_LABEL(ptr), buffer);
}

void copy_text(GtkButton* btn, gpointer ptr) {
  const char *text = gtk_entry_get_text(GTK_ENTRY(txt));
  gtk_label_set_text(GTK_LABEL(ptr), text);
}

int main(int argc, char *argv[]) {
  gtk_init (&argc, &argv);
  GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  txt = gtk_entry_new ();

  GtkWidget *close_button = gtk_button_new_with_label("close");

  GtkWidget *btn2 = gtk_button_new_with_label("count");
  GtkWidget *lbl = gtk_label_new("My label");

  GtkWidget *copy_button = gtk_button_new_with_label("copy");
  GtkWidget *lbl2 = gtk_label_new("My label2");


  g_signal_connect(close_button, "clicked", G_CALLBACK(end_program), NULL);
  g_signal_connect(btn2, "clicked", G_CALLBACK(count_clicked), lbl);
  g_signal_connect(copy_button, "clicked", G_CALLBACK(copy_text), lbl2);

  GtkWidget *grid = gtk_grid_new();

  gtk_grid_attach(GTK_GRID(grid), btn2, 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), lbl, 1, 0, 1, 1);

  gtk_grid_attach(GTK_GRID(grid), copy_button, 0, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), txt, 1, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), lbl2, 2, 1, 1, 1);


  gtk_grid_attach(GTK_GRID(grid), close_button, 0, 2, 3, 1);

  gtk_container_add(GTK_CONTAINER(win), grid);
  gtk_widget_show_all(win);
  gtk_main();
  return 0;
}