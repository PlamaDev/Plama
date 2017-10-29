#ifndef WGLDIAGRAM_H
#define WGLDIAGRAM_H

#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QGuiApplication>
#include <QOpenGLShaderProgram>
#include <QSharedPointer>
#include "render/model.h"
#include "render/engine.h"


class QDiagram : public QOpenGLWidget, protected QOpenGLFunctions {
  Q_OBJECT

protected:

public:
  QDiagram(QWidget *parent);

  ~QDiagram() override;

protected:
  QScopedPointer<Engine> engine;
  int rotX;
  int rotY;

  void mouseMoveEvent(QMouseEvent * event) override;
  void paintGL() override;
  void initializeGL() override;
  void resizeGL(int w, int h) override;
};

#endif // WGLDIAGRAM_H
