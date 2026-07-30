#include <gtk/gtk.h>

size_t cc = 0;

void end_program(GtkWindow* win, gpointer ptr) {
  gtk_main_quit();
}

void count_clicked(GtkButton* btn, gpointer ptr) {
  char buffer[40];
  sprintf(buffer, "clicked {%lu}\n", ++cc);
  gtk_label_set_text(GTK_LABEL(ptr), buffer);
}

int main(int argc, char *argv[]) {
  gtk_init (&argc, &argv);
  GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  GtkWidget *btn = gtk_button_new_with_label("close");
  GtkWidget *btn2 = gtk_button_new_with_label("count");
  GtkWidget *lbl = gtk_label_new ("My label");

  g_signal_connect(btn, "clicked", G_CALLBACK(end_program), NULL);
  g_signal_connect(btn2, "clicked", G_CALLBACK(count_clicked), lbl);

  GtkWidget *grid = gtk_grid_new();

  gtk_grid_attach(GTK_GRID(grid), btn2, 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), lbl, 1, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), btn, 1, 1, 1, 1);

  gtk_container_add(GTK_CONTAINER(win), grid);
  gtk_widget_show_all(win);
  gtk_main();
  return 0;
}