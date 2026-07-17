#include <gtk/gtk.h>

void end_program(GtkWindow* win, gpointer ptr) {
  gtk_main_quit();
}

int main(int argc, char *argv[]) {
  gtk_init (&argc, &argv);
  GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  GtkWidget *btn = gtk_button_new_with_label("close");

  g_signal_connect(btn, "clicked", G_CALLBACK(end_program), NULL);

  gtk_container_add(GTK_CONTAINER(win), btn);
  gtk_widget_show_all(win);
  gtk_main();
  return 0;
}