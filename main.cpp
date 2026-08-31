#include <QApplication>

#include "druxlord.h"
#include "window.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  app.setOrganizationName("Mizux");
  app.setApplicationName("DruxLord");

  MainWindows main;
  main.show();
  return app.exec();
}
