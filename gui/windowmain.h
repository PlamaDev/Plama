#ifndef WINDOWMAIN_H
#define WINDOWMAIN_H

#include <QMainWindow>
#include <memory>
#include "data/fileadapter.h"

class WindowMain : public QMainWindow {
    Q_OBJECT

public:
    explicit WindowMain(QWidget *parent = 0);
    ~WindowMain() = default;

private:
    std::unique_ptr<FileAdapter> data;
};

#endif // WINDOWMAIN_H
