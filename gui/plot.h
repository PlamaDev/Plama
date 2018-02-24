#ifndef PLOT_H
#define PLOT_H

#include "data/project.h"
#include "render/engine.h"
#include <QImage>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLPaintDevice>
#include <QOpenGLWindow>
#include <QWidget>
#include <QWindow>
#include <memory>
#include <vector>

class PlotInternal : public QOpenGLWindow {
public:
    PlotInternal(std::unique_ptr<Model> &&model, std::unique_ptr<Axis> &&axis,
        std::unique_ptr<std::vector<QVector2D>> &&size);
    void setRotation(int x, int y);
    void setLabel(float pos);
    void setModel(std::unique_ptr<Model> model);
    void renderTo(QPaintDevice &d);

private:
    std::unique_ptr<EngineGL> engineGL;
    std::unique_ptr<EngineQt> engineQt;
    QPoint mouse;
    QPoint rotation;

protected:
    void mouseMoveEvent(QMouseEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
};

class Plot : public QWidget {
public:
    Plot(SimQuantity &quantity, int dim);
    void setRotation(int x, int y);
    void setTime(float t);
    void setPartition(float p);
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

private:
    PlotInternal *plot;
    SimQuantity *quantity;
    const std::vector<float> *data;
};

#endif // PLOT_H
