#include "plot.h"
#include "render/model.h"
#include "util.h"
#include <QFileDialog>
#include <QIcon>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QSize>
#include <QToolBar>
#include <QVBoxLayout>
#include <QVector2D>
#include <QWidget>
#include <vector>

using namespace std;

int mouseFuncForward(int x) {
    const int width = 20;
    const float slope = 0.5;
    if (x < width)
        return 0;
    else if (x < 90 / slope + width)
        return (x - width) * slope;
    else
        return 90;
}

int mouseFuncInverse(int x) {
    const int width = 20;
    const float slope = 0.5;
    if (x == 0)
        return 0;
    else if (x == 90)
        return 2 * width + 90 / slope;
    else
        return width + x / slope;
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

vector<QVector2D> genRange(SimQuantity &quantity) {
    if (quantity.getSizeModel().size() == 0) {
        return {
            {quantity.getTimes()[0], quantity.getTimes()[quantity.getTimes().size() - 1]},
            quantity.getExtreme(), {0, 1}};
    } else {
        return {quantity.getSizeModel()[0], quantity.getSizeModel()[1],
            quantity.getExtreme()};
    }
}

Plot::Plot(SimQuantity &quantity, int dim) {
    auto range = make_unique<vector<QVector2D>>();
    if (quantity.getSizeModel().size() == 0) {
        *range = {
            {quantity.getTimes()[0], quantity.getTimes()[quantity.getTimes().size() - 1]},
            quantity.getExtreme(), {0, 1}};
    } else {
        *range = {quantity.getSizeModel()[0], quantity.getSizeModel()[1],
            quantity.getExtreme()};
    }

    QVBoxLayout *l = new QVBoxLayout;
    l->setMargin(0);
    QToolBar *bar = new QToolBar;
    QAction *aExport = new QAction(QIcon::fromTheme("document-export"), "Export");
    connect(aExport, &QAction::triggered, [=]() {
        QString name = QFileDialog::getSaveFileName(
            this, "Export file", "", "JPEG image (*.jpg, *.jpeg, *.jpe)");
        qDebug() << name;
    });
    bar->addAction(aExport);
    l->addWidget(bar);

    plot = new PlotInternal(quantity.getSizeData().size() == 0
            ? Model::fromQuantity(quantity, dim)
            : Model::fromQuantity(quantity, quantity.getTimes()[0], dim),
        make_unique<Axis>(5, 5, 5), std::move(range));
    l->addWidget(QWidget::createWindowContainer(plot));
    l->setMargin(0);
    setLayout(l);
    this->quantity = &quantity;
    data = &quantity.getDataAt(quantity.getTimes()[0], dim);
}

void Plot::setRotation(int x, int y) { plot->setRotation(x, y); }

void Plot::setTime(float t) {
    if (quantity->getSizeData().size() == 0) {
        const vector<float> &times = quantity->getTimes();
        float t1 = times[0];
        float t2 = times[times.size() - 1];
        float td = t2 - t1;
        plot->setLabel((t - t1) / td);
    } else if (quantity->getSizeData().size() == 2) {
        const vector<float> &d = quantity->getDataAt(t, 0);
        if (&d != data) {
            plot->setModel(Model::fromQuantity(*quantity, t, 0));
            data = &d;
        }
    }
}

void Plot::setPartition(float p) {
    const vector<float> &times = quantity->getTimes();
    float t1 = times[0];
    float t2 = times[times.size() - 1];
    float td = t2 - t1;
    setTime(t1 + td * p);
}

QSize Plot::sizeHint() const { return QSize(500, 500); }
QSize Plot::minimumSizeHint() const { return QSize(100, 100); }

PlotInternal::PlotInternal(unique_ptr<Model> &&model, unique_ptr<Axis> &&axis,
    unique_ptr<vector<QVector2D>> &&size) {
    shared_ptr<Model> pm = move(model);
    shared_ptr<Axis> pa = move(axis);
    shared_ptr<vector<QVector2D>> ps = move(size);
    engineGL = make_unique<EngineGL>(pm, pa, ps);
    engineQt = make_unique<EngineQt>(pm, pa, ps);
    engineGL->setBackGround(Qt::white);
    engineQt->setBackGround(Qt::transparent);
}

void PlotInternal::setRotation(int x, int y) {
    engineGL->setRotation(x, y);
    engineQt->setRotation(x, y);
}

void PlotInternal::setLabel(float pos) {
    engineGL->setLabel(pos);
    engineQt->setLabel(pos);
    requestUpdate();
}

void PlotInternal::setModel(unique_ptr<Model> model) {
    shared_ptr<Model> pm = move(model);
    engineGL->setModel(pm);
    engineQt->setModel(pm);
    requestUpdate();
}

void PlotInternal::renderTo(QPaintDevice &d) {
    QPainter p(&d);
    engineQt->render(p);
}

void PlotInternal::mouseMoveEvent(QMouseEvent *event) {
    if ((event->buttons() & Qt::LeftButton) == 0) return;

    QPoint diff = event->pos() - mouse;
    int mx = mouseFuncInverseLoop(rotation.x()) + diff.x();
    int my = mouseFuncInverseLoop(rotation.y()) + diff.y();
    int ry = mouseFuncForwardLoop(my);
    if (ry > 90)
        ry = 90;
    else if (ry < 0)
        ry = 0;
    setRotation(unify(mouseFuncForwardLoop(mx), 360).second, ry);
    requestUpdate();
}

void PlotInternal::mousePressEvent(QMouseEvent *event) {
    if ((event->buttons() & Qt::LeftButton) == 0) return;
    mouse = event->pos();
    rotation = engineGL->getRotation();
}

void PlotInternal::mouseReleaseEvent(QMouseEvent *) { setMouseGrabEnabled(false); }

void PlotInternal::initializeGL() { engineGL->initialize(); }

void PlotInternal::resizeGL(int w, int h) { engineGL->resize(w, h); }

void PlotInternal::paintGL() {
    QPainter p(this);
    engineGL->render(p);
}
