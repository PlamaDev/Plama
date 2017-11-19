#include "interface/windowmain.h"
#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char *argv[]) {
    QSurfaceFormat format;

    format.setSamples(6);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication a(argc, argv);
    WindowMain w(nullptr);

    w.show();

    return(a.exec());
}
