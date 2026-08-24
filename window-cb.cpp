#include "window-cb.h"
#include "window.h"
#include "druxlord.h"

extern "C" {

gboolean window_main_window_close_request_cb(GtkWindow *window, gpointer data) {
  return FALSE;
}

void window_main_button_buy_clicked_cb(GtkWidget *widget, gpointer data) {
  create_window_input("input", "message", "how many?");
  gtk_window_present(GTK_WINDOW(window_input.window));
}

void window_main_button_stayhere_clicked_cb(GtkWidget *widget, gpointer data) {
  generate_drug();
  insert_treeview_drug(GTK_TREE_VIEW(window_main.treeview_market));
}

void menuitem_places_finances_activate_cb(GtkWidget *widget, gpointer data) {
  create_window_finance();
  gtk_window_present(GTK_WINDOW(window_finance.window));
}

void menuitem_places_shopping_activate_cb(GtkWidget *widget, gpointer data) {
  create_window_shopping();
  gtk_window_present(GTK_WINDOW(window_shopping.window));
}

void menuitem_places_hospital_activate_cb(GtkWidget *widget, gpointer data) {
  create_window_hospital();
  gtk_window_present(GTK_WINDOW(window_hospital.window));
}

void menuitem_places_vault_activate_cb(GtkWidget *widget, gpointer data) {
  create_window_vault();
  gtk_window_present(GTK_WINDOW(window_vault.window));
}

void menuitem_places_shipping_activate_cb(GtkWidget *widget, gpointer data) {
}

void menuitem_info_vaults_activate_cb(GtkWidget *widget, gpointer data) {
}

void menuitem_info_drug_prices_activate_cb(GtkWidget *widget, gpointer data) {
}

void menuitem_info_world_cities_activate_cb(GtkWidget *widget, gpointer data) {
}

void menuitem_info_shipment_status_activate_cb(GtkWidget *widget, gpointer data) {
}

void menuitem_info_history_activate_cb(GtkWidget *widget, gpointer data) {
}

}  // extern "C"
