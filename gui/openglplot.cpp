#include "openglplot.h"
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QWidget>
#include <QSize>
#include <QPainter>
#include "util.h"

int mouseFuncForward(int x) {
    const int width = 20;
    const float slope = 0.5;
    if (x < width) return 0;
    else if (x < 90/slope + width) return (x - width) * slope;
    else return 90;
}

int mouseFuncInverse(int x) {
    const int width = 20;
    const float slope = 0.5;
    if (x == 0) return 0;
    else if (x == 90) return 2 * width + 90 / slope;
    else return width + x / slope;
}

int mouseFuncForwardLoop(int x) {
    const int width = 20;
    const float slope = 0.5;
    const int radix = 2 * width + 90 / slope;
    QPair<int, int> p = unify(x, radix);
    return p.first * 90 + mouseFuncForward(p.second);
}

int mouseFuncInverseLoop(int x) {
    const int width = 20;
    const float slope = 0.5;
    const int radix = 90;
    QPair<int, int> p = unify(x, radix);
    return p.first * (2 * width + (int)(90 / slope)) + mouseFuncInverse(p.second);
}

OpenGLPlot::OpenGLPlot(std::unique_ptr<Model> &model) {
    QHBoxLayout *l = new QHBoxLayout;
    plot = new OpenGLPlotInternal(model);
    l->addWidget(QWidget::createWindowContainer(plot));
    l->setMargin(0);
    setLayout(l);
}

void OpenGLPlot::setRotation(int x, int y) {
    plot->setRotation(x, y);
}

QSize OpenGLPlot::sizeHint() const { return QSize(200, 200); }
QSize OpenGLPlot::minimumSizeHint() const { return QSize(50, 50); }

OpenGLPlotInternal::OpenGLPlotInternal(std::unique_ptr<Model> &model) {
    std::unique_ptr<Axis> a = std::make_unique<Axis>(5, 5, 5);
    engine = std::make_unique<EngineSimple>(model, a);
    QWidget w;
    engine->setBackGround(Qt::white);
}

void OpenGLPlotInternal::setRotation(int x, int y) {
    engine->setRotation(x, y);
}

void OpenGLPlotInternal::render(QPainter &p) {
    engine->resize(size().width(), size().height());
    engine->render(p);
}

void OpenGLPlotInternal::initialize() { engine->initialize(); }

void OpenGLPlotInternal::mouseMoveEvent(QMouseEvent *event) {
    if ((event->buttons() & Qt::LeftButton) == 0) return;

    QPoint diff = event->pos() - mouse;
    int mx = mouseFuncInverseLoop(rotation.x()) + diff.x();
    int my = mouseFuncInverseLoop(rotation.y()) + diff.y();
    int ry = mouseFuncForwardLoop(my);
    if (ry > 90) ry = 90;
    else if (ry < 0) ry = 0;
    setRotation(unify(mouseFuncForwardLoop(mx), 360).second, ry);
    requestUpdate();
}

void OpenGLPlotInternal::mousePressEvent(QMouseEvent *event) {
    if ((event->buttons() & Qt::LeftButton) == 0) return;
    //setMouseGrabEnabled(true);
    mouse = event->pos();
    rotation = engine->getRotation();
}

void OpenGLPlotInternal::mouseReleaseEvent(QMouseEvent *) {
    setMouseGrabEnabled(false);
}

OpenGLWindow::OpenGLWindow(QWindow *parent)
    : QWindow(parent), m_context(0), m_device(0) {
    setFlags(flags() | Qt::FramelessWindowHint);
    setSurfaceType(QWindow::OpenGLSurface);
}

OpenGLWindow::~OpenGLWindow() {
    delete m_device;
    delete m_context;
}

void OpenGLWindow::initialize() {}

void OpenGLWindow::render(QPainter &) {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

bool OpenGLWindow::event(QEvent *event) {
    switch (event->type()) {
    case QEvent::UpdateRequest:
        doRender();
        return true;
    default:
        return QWindow::event(event);
    }
}

void OpenGLWindow::exposeEvent(QExposeEvent *) { if (isExposed()) doRender(); }

void OpenGLWindow::doRender() {
    if (!isExposed()) return;

    if (!m_context) {
        m_context = new QOpenGLContext(this);
        m_context->create();
        m_context->makeCurrent(this);

        initializeOpenGLFunctions();
        initialize();
        m_device = new QOpenGLPaintDevice;
    } else m_context->makeCurrent(this);
    m_device->setSize(size());

    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    QPainter p(m_device);
    p.setRenderHint(QPainter::Antialiasing, true);
    render(p);
    m_context->swapBuffers(this);
}
