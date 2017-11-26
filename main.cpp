#define Py_LIMITED_API
#include <Python.h>
#include "interface/windowmain.h"
#include <QApplication>
#include <QSurfaceFormat>
#include <QList>


int main(int argc, char *argv[]) {
    Py_Initialize();
    int ret;
    {
        QSurfaceFormat format;

        format.setSamples(6);
        QSurfaceFormat::setDefaultFormat(format);

        QApplication a(argc, argv);
        WindowMain w(nullptr);

        w.show();
        ret = a.exec();
    }
    Py_Finalize();
    return ret;
}
