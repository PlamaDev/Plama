#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class Main;
}

class Main : public QMainWindow
{
    Q_OBJECT

public:
    explicit Main(QWidget *parent = 0);
    ~Main();

private:
    Ui::Main *ui;
};

#endif // MAINWINDOW_H
