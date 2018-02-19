#include "plot.h"
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QWidget>
#include <QSize>
#include <QPainter>
#include "util.h"
#include "render/model.h"

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

QVector<QVector2D> genRange(SimQuantity &quantity) {
    if (quantity.getSizeModel().size() == 0) {
        return  {
            {quantity.getTimes()[0], quantity.getTimes()[quantity.getTimes().size() - 1]},
            quantity.getExtreme(), {0, 1}
        };
    } else {
        return {
            quantity.getSizeModel()[0], quantity.getSizeModel()[1], quantity.getExtreme()
        };
    }
}

Plot::Plot(SimQuantity &quantity, int dim) {
    QVector<QVector2D> range;
    if (quantity.getSizeModel().size() == 0) {
        range = {
            {quantity.getTimes()[0], quantity.getTimes()[quantity.getTimes().size() - 1]},
            quantity.getExtreme(), {0, 1}
        };
    } else {
        range = {
            quantity.getSizeModel()[0], quantity.getSizeModel()[1], quantity.getExtreme()
        };
    }

    QHBoxLayout *l = new QHBoxLayout;
    plot = new PlotInternal(quantity.getSizeData().size() == 0 ?
        Model::fromQuantity(quantity, dim)
        : Model::fromQuantity(quantity, quantity.getTimes()[0], dim),
        std::make_unique<Axis>(5, 5, 5), range);
    l->addWidget(QWidget::createWindowContainer(plot));
    l->setMargin(0);
    setLayout(l);
    this->quantity = &quantity;
}

void Plot::setRotation(int x, int y) { plot->setRotation(x, y);}

void Plot::setTime(float t) {
    if (quantity->getSizeData().size() == 0) {
        const QVector<float> &times = quantity->getTimes();
        float t1 = times[0];
        float t2 = times[times.size() - 1];
        float td = t2 - t1;
        plot->setLabel((t - t1) / td);
    } else if (quantity->getSizeData().size() == 2) {
        const QVector<float> &d = quantity->getDataAt(t, 0);
        if (&d != data) {
            plot->setModel(Model::fromQuantity(*quantity, t, 0));
            data = &d;
        }
    }
}

void Plot::setPartition(float p) {
    const QVector<float> &times = quantity->getTimes();
    float t1 = times[0];
    float t2 = times[times.size() - 1];
    float td = t2 - t1;
    setTime(t1 + td * p);
}

QSize Plot::sizeHint() const { return QSize(500, 500); }
QSize Plot::minimumSizeHint() const { return QSize(100, 100); }

PlotInternal::PlotInternal(
    std::unique_ptr<Model> &&model, std::unique_ptr<Axis> &&axis,
    const QVector<QVector2D> &size) {
    engine = std::make_unique<EngineQt>(
            std::move(model), std::move(axis), size);
    engine->setBackGround(Qt::white);
}

void PlotInternal::setRotation(int x, int y) { engine->setRotation(x, y); }
void PlotInternal::setLabel(float pos) {
    engine->setLabel(pos);
    requestUpdate();
}

void PlotInternal::setModel(std::unique_ptr<Model> model) {
    engine->setModel(std::move(model));
    requestUpdate();
}

void PlotInternal::render(QPainter &p) {
    engine->resize(size().width(), size().height());
    engine->render(p);
}

void PlotInternal::initialize() { engine->initialize(); }

void PlotInternal::mouseMoveEvent(QMouseEvent *event) {
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

void PlotInternal::mousePressEvent(QMouseEvent *event) {
    if ((event->buttons() & Qt::LeftButton) == 0) return;
    mouse = event->pos();
    rotation = engine->getRotation();
}

void PlotInternal::mouseReleaseEvent(QMouseEvent *) {
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
