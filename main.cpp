#include <QApplication>
#include "druxlord.h"
#include "window.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  GameState game_state;
  create_window_main(game_state);
  window_main.window->show();
  return app.exec();
}
