#ifndef OPENGLPLOT_H
#define OPENGLPLOT_H

#include <QWidget>
#include <QWindow>
#include <QOpenGLFunctions>
#include <QOpenGLContext>
#include <QOpenGLPaintDevice>
#include <memory>
#include <render/engine.h>

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

class OpenGLPlotInternal : public OpenGLWindow {
public:
    OpenGLPlotInternal(std::unique_ptr<Model> &model);
    void setRotation(int x, int y);
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

class OpenGLPlot : public QWidget {
public:
    OpenGLPlot(std::unique_ptr<Model> &model);
    void setRotation(int x, int y);
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

private:
    OpenGLPlotInternal *plot;
};

#endif // OPENGLPLOT_H
