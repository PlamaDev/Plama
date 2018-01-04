#ifndef WGLDIAGRAM_H
#define WGLDIAGRAM_H
#pragma clang diagnostic ignored "-Wunknown-warning-option"

#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QGuiApplication>
#include <QOpenGLShaderProgram>
#include <QSharedPointer>
#include "render/model.h"
#include "render/engine.h"
#include "dataio/fileadapter.h"

class PlotOverlay : public QWidget {
public:
    PlotOverlay(QWidget *parent);
protected:
    void paintEvent(QPaintEvent *event);
};

class PlotFrame : public QWidget {
public:
    PlotFrame(const SimQuantity& q);
};

class PlotGL : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

protected:

public:
    PlotGL(const SimQuantity& q, QWidget *parent = nullptr);
    ~PlotGL() override;

protected:
    QScopedPointer <Engine> engine;
    int rotX;
    int rotY;

    void mouseMoveEvent(QMouseEvent *event) override;
    void paintGL() override;
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event);
};

#endif // WGLDIAGRAM_H
