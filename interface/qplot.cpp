#include "interface/qplot.h"
#include <Qt>
#include <QSurfaceFormat>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QMouseEvent>
#include <QSharedPointer>
#include "render/model.h"
#include "render/engine.h"
#include "dataio/fileio.h"


QPlot::QPlot(const SimQuantity&q, QWidget *parent) :
    QOpenGLWidget(parent), QOpenGLFunctions(),
    engine(new EngineSimple) {
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAttribute(Qt::WA_NativeWindow, true);
    //setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_MSWindowsUseDirect3D, true);
    QVector <int> size = q.getSizeData();
    QScopedPointer <Model> model(new ModelInsert(q.getDataAt(0, 0).data(),
            size[0], size[1]));
    engine->setModel(model);
    engine->setBackGround(palette().color(QWidget::backgroundRole()));
}

QPlot::~QPlot() {}

void QPlot::mouseMoveEvent(QMouseEvent *event) {
    rotX = event->x();
    rotY = event->y();
    update();
}

void QPlot::initializeGL() {
    engine->initialize();
}

void QPlot::paintGL() {
    engine->setRotation(rotX, rotY);
    engine->render();
}

void QPlot::resizeGL(int w, int h) {
    engine->resize(w, h);
}
