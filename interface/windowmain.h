#ifndef WINDOWMAIN_H
#define WINDOWMAIN_H

#include <QMainWindow>

namespace Ui {
class Main;
}

class WindowMain : public QMainWindow
{
  Q_OBJECT

public:
  explicit WindowMain(QWidget *parent = 0);
  ~WindowMain();

private:
  Ui::Main *ui;
};

#endif // WINDOWMAIN_H
