#include <gtk/gtk.h>
#include "window.h"

static void on_activate(GtkApplication *app, gpointer user_data) {
  create_window_main(app);
  gtk_window_present(GTK_WINDOW(window_main.window));
}

int main(int argc, char **argv) {
  GtkApplication *app = gtk_application_new("org.druxlord.game", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(on_activate), nullptr);
  int status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  return status;
}
