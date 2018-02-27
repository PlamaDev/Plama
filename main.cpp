#define Py_LIMITED_API
#include "gui/windowmain.h"
#include "render/model.h"
#include <Python.h>
#include <QApplication>
#include <QDebug>
#include <QList>
#include <QProgressBar>
#include <QSurfaceFormat>
#include <QVector2D>
#include <cstdlib>
#include <memory>
#include <vector>

using namespace std;

int main(int argc, char *argv[]) {
    //    srand(time(0));
    //    vector<float> data(10000);

    //    for (int i = 0; i < 1000; i++) data[i] = rand();
    //    for (int i = 0; i < 1000; i++) data[i] *= 2.0 / RAND_MAX;
    //    QVector2D extreme(0, 2);
    //    unique_ptr<Model> m = Model::fromData(data, 100, 100, extreme);

    //    for (int i = 0; i < 10000; i++) {
    //        m->changeData(data, 100, 100, extreme);
    //        qDebug() << i;
    //    }
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
