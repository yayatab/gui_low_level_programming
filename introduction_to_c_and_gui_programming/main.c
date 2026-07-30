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

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  GtkWidget *box2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  gtk_box_pack_start(GTK_BOX(box2), btn2, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(box2), lbl, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(box), box2, TRUE, TRUE, 1);
  gtk_box_pack_start(GTK_BOX(box), btn, FALSE, FALSE, 0);

  gtk_container_add(GTK_CONTAINER(win), box);
  gtk_widget_show_all(win);
  gtk_main();
  return 0;
}