#include <gtk/gtk.h>

size_t cc = 0;

void end_program(GtkWindow* win, gpointer ptr) {
  gtk_main_quit();
}

GtkWidget *txt;
GtkWidget *txt2;

void count_clicked(GtkButton* btn, gpointer ptr) {
  char buffer[40];
  sprintf(buffer, "clicked {%lu}\n", ++cc);
  gtk_label_set_text(GTK_LABEL(ptr), buffer);
}

void copy_text(GtkButton* btn, gpointer ptr) {
  const char *text = gtk_entry_get_text(GTK_ENTRY(txt));
  gtk_label_set_text(GTK_LABEL(ptr), text);
}

void check_state(GtkButton* btn, gpointer ptr) {
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(btn)))
    {
      gtk_label_set_text(GTK_LABEL(ptr), "V");
    } else {
        gtk_label_set_text(GTK_LABEL(ptr), "X");
    }
}

void rdo_state(GtkButton* btn, gpointer ptr) {
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(btn)))
    {
      gtk_label_set_text(GTK_LABEL(ptr), gtk_button_get_label(GTK_BUTTON(btn)));
    }
}

void pick_and_choose(GtkButton* btn, gpointer ptr) {
  char *selected = gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT(btn));
  gtk_label_set_text(GTK_LABEL(ptr), selected);
}

int main(int argc, char *argv[]) {
  gtk_init (&argc, &argv);
  GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  txt = gtk_entry_new();
  txt2 = gtk_entry_new();

  GtkWidget *close_button = gtk_button_new_with_label("close");

  GtkWidget *btn2 = gtk_button_new_with_label("count");
  GtkWidget *copy_button = gtk_button_new_with_label("copy");

  GtkAdjustment *adj_btn = gtk_adjustment_new(0, -10, 10, 1,2,2);
  txt2 = gtk_spin_button_new(adj_btn, 0, 0);

  GtkWidget *chk_btn = gtk_check_button_new_with_label("checks");

  GtkWidget *rdo_btn1 = gtk_radio_button_new_with_label(NULL, "rd1");
  GSList *rdo_list = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rdo_btn1));
  GtkWidget *rdo_btn2 = gtk_radio_button_new_with_label(rdo_list, "rd2");

  GtkWidget *lbl = gtk_label_new("My label");
  GtkWidget *lbl2 = gtk_label_new("My label2");
  GtkWidget *adj_lbl = gtk_label_new("adjustments");
  GtkWidget *checks = gtk_label_new("checks");
  GtkWidget *v_stat_lbl = gtk_label_new("X");
  GtkWidget *rdo_btns_lbl = gtk_label_new("radio_buttons");
  GtkWidget *rdo_btns_out = gtk_label_new("1");
  GtkWidget *opts_lbl = gtk_label_new("options");
  GtkWidget *opts_out = gtk_label_new("b");

  GtkWidget *combo_box = gtk_combo_box_text_new();
  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "a");
  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "b");
  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "c");
  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "d");
  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "e");
  gtk_combo_box_set_active(GTK_COMBO_BOX(combo_box), 1);


  g_signal_connect(close_button, "clicked", G_CALLBACK(end_program), NULL);
  g_signal_connect(btn2, "clicked", G_CALLBACK(count_clicked), lbl);
  g_signal_connect(copy_button, "clicked", G_CALLBACK(copy_text), lbl2);
  g_signal_connect(chk_btn, "clicked", G_CALLBACK(check_state), v_stat_lbl);

  g_signal_connect(rdo_btn1, "clicked", G_CALLBACK(rdo_state), rdo_btns_out); // must be a better way
  g_signal_connect(rdo_btn2, "clicked", G_CALLBACK(rdo_state), rdo_btns_out);

  g_signal_connect(combo_box, "changed", G_CALLBACK(pick_and_choose), opts_out);

  /*
   * and now more complicated way yo have boxes... will try later.
   int pos = 0;
 GtkListStore *ls = gtk_list_store_new (1, G_TYPE_STRING);
 gtk_list_store_insert_with_values (ls, NULL, pos++, 0, "Option 1", -1);
 gtk_list_store_insert_with_values (ls, NULL, pos++, 0, "Option 2", -1);
 gtk_list_store_insert_with_values (ls, NULL, pos++, 0, "Option 3", -1);
 GtkWidget *comb = gtk_combo_box_new_with_model ( GTK_TREE_MODEL (ls));
 GtkCellRenderer *rend = gtk_cell_renderer_text_new ();
 gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (comb), rend, FALSE);
 gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT (comb), rend, "text", 0);

  GtkWidget *comb = gtk_combo_box_new_with_model ( GTK_TREE_MODEL (ls));
  GtkCellRenderer *rend = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (comb), rend, FALSE);
  gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT (comb), rend,  "text", 0);

  GtkTreeModelSort *sorted = GTK_TREE_MODEL_SORT (
 gtk_tree_model_sort_new_with_model (GTK_TREE_MODEL (ls)));
 gtk_tree_sortable_set_sort_column_id (
 GTK_TREE_SORTABLE (sorted), 0, GTK_SORT_ASCENDING);
 GtkWidget *comb = gtk_combo_box_new_with_model (
 GTK_TREE_MODEL (sorted));

   */
  GtkWidget *grid = gtk_grid_new();

  gtk_grid_attach(GTK_GRID(grid), btn2, 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), lbl, 1, 0, 1, 1);

  gtk_grid_attach(GTK_GRID(grid), copy_button, 0, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), txt, 1, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), lbl2, 2, 1, 1, 1);

  gtk_grid_attach(GTK_GRID(grid), adj_lbl, 0, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), txt2, 2, 2, 1, 1);

  gtk_grid_attach(GTK_GRID(grid), checks, 0, 3, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), chk_btn, 1, 3, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), v_stat_lbl, 2, 3, 1, 1);

  gtk_grid_attach(GTK_GRID(grid), rdo_btns_lbl, 0, 4, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), rdo_btn1, 1, 4, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), rdo_btn2, 2, 4, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), rdo_btns_out, 2, 4, 1, 1); // todo how?

  gtk_grid_attach(GTK_GRID(grid), opts_lbl, 0, 5, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), combo_box, 1, 5, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), opts_out, 2, 5, 1, 1);

  gtk_grid_attach(GTK_GRID(grid), close_button, 0, 6, 3, 1);

  gtk_container_add(GTK_CONTAINER(win), grid);
  gtk_widget_show_all(win);
  gtk_main();
  return 0;
}