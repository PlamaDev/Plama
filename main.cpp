#define Py_LIMITED_API
#include "gui/windowmain.h"
#include <Python.h>
#include <QApplication>
#include <QDebug>
#include <QList>
#include <QProgressBar>
#include <QSurfaceFormat>
#include <vector>

int main(int argc, char *argv[]) {
    Py_Initialize();

    int ret;
    {
        QSurfaceFormat format;

        format.setSamples(2);
        format.setSwapInterval(0);
        format.setDepthBufferSize(16);
        format.setRenderableType(QSurfaceFormat::OpenGL);
        QSurfaceFormat::setDefaultFormat(format);

        QApplication a(argc, argv);
        WindowMain w(nullptr);

        w.show();
        ret = a.exec();
    }
    Py_Finalize();
    return ret;
}
