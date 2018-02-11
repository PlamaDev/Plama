#define Py_LIMITED_API
#include <Python.h>
#include <QApplication>
#include <QSurfaceFormat>
#include <QList>
#include "gui/windowmain.h"


int main(int argc, char *argv[]) {
    Py_Initialize();
    int ret;
    {
        QSurfaceFormat format;

        format.setSamples(6);
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
