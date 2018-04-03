#ifndef WINDOWMAIN_H
#define WINDOWMAIN_H

#include "data/project.h"
#include <QMainWindow>
#include <memory>

class WindowMain : public QMainWindow {
    Q_OBJECT

public:
    explicit WindowMain(QWidget *parent = 0);
    ~WindowMain() = default;

private:
    std::unique_ptr<Project> data;
};

#endif // WINDOWMAIN_H
