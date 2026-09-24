#include <QApplication>
#include <QString>

#include "window.h"
#include "config.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  app.setOrganizationName(QString::fromUtf8(std::string{kProgramAuthorName}));
  app.setApplicationName(QString::fromUtf8(std::string{kProgramName}));

  MainWindow main;
  main.show();
  return app.exec();
}
