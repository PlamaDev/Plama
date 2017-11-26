#ifndef WINDOWMAIN_H
#define WINDOWMAIN_H
#pragma clang diagnostic ignored "-Wunknown-warning-option"

#include <QMainWindow>
#include <QSharedPointer>
#include <dataio/fileadapter.h>

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
    QSharedPointer <FileAdapter> data;
};

#endif // WINDOWMAIN_H
