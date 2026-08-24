#include <QApplication>
#include "window.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  create_window_main();
  window_main.window->show();
  return app.exec();
}
