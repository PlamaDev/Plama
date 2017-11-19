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


class QPlot : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

protected:

public:
    QPlot(const QPlot& p);

    QPlot(QWidget *parent = nullptr);


    ~QPlot() override;

protected:
    QScopedPointer <Engine> engine;
    int rotX;
    int rotY;

    void mouseMoveEvent(QMouseEvent *event) override;
    void paintGL() override;
    void initializeGL() override;
    void resizeGL(int w, int h) override;
};

#endif // WGLDIAGRAM_H
