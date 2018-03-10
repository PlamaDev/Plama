#ifndef PLOT_H
#define PLOT_H

#include "data/project.h"
#include "render/engine.h"
#include <QImage>
#include <QOpenGLWindow>
#include <QWidget>
#include <memory>
#include <vector>

class PlotInternal : public QOpenGLWindow {
public:
    PlotInternal(std::unique_ptr<Axis> &&axis, std::unique_ptr<Bar> &&bar,
        std::unique_ptr<std::vector<VectorD2D>> &&size);
    void setRotation(int x, int y, bool update = true);
    void setLabel(float pos, bool update = true);
    void setShader(bool en, bool update = true);
    void setEnBar(bool en, bool update = true);
    void setEnLabel(bool en, bool update = true);
    bool setQuantity(SimQuantity &sq, float time, int step, bool update = true);
    void renderTo(QPaintDevice &d);

private:
    std::shared_ptr<Model> model;
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
    Plot(SimQuantity &quantity);
    void setRotation(int x, int y, bool update = true);
    void setTime(double t, bool update = true);
    void setPartition(float p, bool update = true);
    void setStep(int s, bool update = true);
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

private:
    PlotInternal *plot;
    SimQuantity *quantity;
    double time;
    int step;

    void renderVideo(QString dir, int sizeX, int sizeY, int len, int fps);
};

#endif // PLOT_H
