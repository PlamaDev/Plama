#include "render/qdiagram.h"
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

QDiagram::QDiagram(QWidget *parent) : QOpenGLWidget(parent), QOpenGLFunctions(), engine(new EngineSimple) {
  setAttribute(Qt::WA_OpaquePaintEvent, true);
  setAttribute(Qt::WA_NativeWindow, true);
  setAttribute(Qt::WA_NoSystemBackground, true);
  setAttribute(Qt::WA_MSWindowsUseDirect3D, true);

  // antialiasing
  QSurfaceFormat format;
  format.setSamples(6);
  setFormat(format);


  //float data[] = {0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.0f};
  //QScopedPointer<Model> model(new ModelInsert(data, 3, 3));
  Fileio f;
  QScopedPointer<Model> model(new ModelSmooth(f.getData(), f.getSizeX(), f.getSizeY()));
  engine->setModel(model);
  engine->setBackGround(palette().color(QWidget::backgroundRole()));
}

QDiagram::~QDiagram() {

}

void QDiagram::mouseMoveEvent(QMouseEvent * event) {
  rotX = event->x();
  rotY = event->y();
  update();
}

void QDiagram::initializeGL() {
  engine->initialize();
}

void QDiagram::paintGL() {
  engine->setRotation(rotX, rotY);
  engine->render();

}

void QDiagram::resizeGL(int w, int h) {
  engine->resize(w, h);
}
