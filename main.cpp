#include "interface/windowmain.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  WindowMain w(nullptr);
  w.show();

  return a.exec();
}
