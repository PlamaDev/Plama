#include "interface/plotgl.h"
#include <Qt>
#include <QSurfaceFormat>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QMouseEvent>
#include <QSharedPointer>
#include <QGridLayout>
#include <QPainter>
#include <QTimer>
#include "render/model.h"
#include "render/engine.h"
#include "dataio/fileio.h"


PlotGL::PlotGL(const SimQuantity&q, QWidget *parent) :
    QOpenGLWidget(parent), QOpenGLFunctions(),
    engine(new EngineSimple) {
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAttribute(Qt::WA_NativeWindow, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    //setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_MSWindowsUseDirect3D, true);
    QVector <int> size = q.getSizeData();
    QScopedPointer <Model> model(new ModelInsert(q.getDataAt(0, 0).data(),
            size[0], size[1]));
    engine->setModel(model);
    engine->setBackGround(palette().color(QWidget::backgroundRole()));
}

PlotGL::~PlotGL() {}

void PlotGL::mouseMoveEvent(QMouseEvent *event) {
    rotX = event->x();
    rotY = event->y();
    update();
}

void PlotGL::initializeGL() {
    engine->initialize();
}

void PlotGL::paintGL() {
    engine->setRotation(rotX, rotY);
    engine->render();
}

void PlotGL::resizeGL(int w, int h) {
    engine->resize(w, h);
}

void PlotGL::mousePressEvent(QMouseEvent *event) {
    QOpenGLWidget::mousePressEvent(event);
    update();
}

void PlotGL::resizeEvent(QResizeEvent *event) {
    QOpenGLWidget::resizeEvent(event);
    QTimer *t = new QTimer(this);
    connect(t, &QTimer::timeout, [&]() { this->update(); });
    t->start(1000);
}

PlotFrame::PlotFrame(const SimQuantity&q) {
    QGridLayout *l = new QGridLayout();
    PlotGL *p = new PlotGL(q);

    l->addWidget(p, 0, 0, 1, 1);
    //new PlotOverlay(p);
    //new PlotOverlay(this);
    setLayout(l);
}

PlotOverlay::PlotOverlay(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
}

void PlotOverlay::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    painter.drawText(QPointF(20, 30), "Text");
}
