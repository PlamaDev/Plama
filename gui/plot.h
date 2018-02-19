#ifndef OPENGLPLOT_H
#define OPENGLPLOT_H

#include <QWidget>
#include <QWindow>
#include <QOpenGLFunctions>
#include <QOpenGLContext>
#include <QOpenGLPaintDevice>
#include <QVector>
#include <memory>
#include <render/engine.h>
#include <data/project.h>

class OpenGLWindow : public QWindow, protected QOpenGLFunctions {
    Q_OBJECT
public:
    explicit OpenGLWindow(QWindow *parent = 0);
    ~OpenGLWindow();
    virtual void render(QPainter &p);
    virtual void initialize();
    void doRender();

protected:
    bool event(QEvent *event) override;
    void exposeEvent(QExposeEvent *event) override;

private:
    QOpenGLContext *m_context;
    QOpenGLPaintDevice *m_device;
};

class PlotInternal : public OpenGLWindow {
    Q_OBJECT
public:
    PlotInternal(std::unique_ptr<Model> &&model, std::unique_ptr<Axis> &&axis,
        const QVector<QVector2D> &size);
    void setRotation(int x, int y);
    void setLabel(float pos);
    void setModel(std::unique_ptr<Model> model);
    void render(QPainter &p) override;
    void initialize() override;

private:
    std::unique_ptr<Engine> engine;
    QPoint mouse;
    QPoint rotation;

protected:
    void mouseMoveEvent(QMouseEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
};

class Plot : public QWidget {
    Q_OBJECT
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
    const QVector<float> *data;
};

#endif // OPENGLPLOT_H
