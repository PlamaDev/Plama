#include "plot.h"
#include "render/bar.h"
#include "render/model.h"
#include "util.h"
#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QIcon>
#include <QLatin1Char>
#include <QMouseEvent>
#include <QPainter>
#include <QPdfWriter>
#include <QProcess>
#include <QProgressBar>
#include <QPushButton>
#include <QSize>
#include <QSizeF>
#include <QSvgGenerator>
#include <QTemporaryDir>
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

void Plot::setTime(float t, bool update) {
    const vector<float> &times = quantity->getTimes();
    float t1 = times[0];
    float t2 = times[times.size() - 1];
    if (plot->setQuantity(*quantity, t, update))
        plot->setLabel((t - t1) / (t2 - t1), update);
    time = t;
}

Plot::Plot(SimQuantity &quantity, int dim) : time(quantity.getTimes()[0]) {
    static vector<Trio<QString, vector<QString>, function<void(QString, Plot &)>>> types =
        {
            {"JPEG image (*.jpg *.jpeg *.jpe)", {"jpg", "jpeg", "jpe"},
                [](QString s, Plot &p) {
                    QImage image(3000, 3000, QImage::Format_ARGB32);
                    p.plot->setEnLabel(false, false);
                    p.plot->renderTo(image);
                    p.plot->setEnLabel(true, false);
                    QImage scaled = image.scaled(
                        1500, 1500, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                    scaled.save(s, "JPG", 100);
                }},
            {"PNG image (*.png)", {"png"},
                [](QString s, Plot &p) {
                    QImage image(3000, 3000, QImage::Format_ARGB32);
                    p.plot->setEnLabel(false, false);
                    p.plot->renderTo(image);
                    p.plot->setEnLabel(true, false);
                    QImage scaled = image.scaled(
                        1500, 1500, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                    scaled.save(s, "PNG", 100);
                }},
            {"SVG image (*.svg)", {"svg"},
                [](QString s, Plot &p) {
                    QSvgGenerator generator;
                    generator.setFileName(s);
                    generator.setSize(QSize(800, 800));
                    generator.setViewBox(QRect(0, 0, 800, 800));
                    p.plot->setEnLabel(false, false);
                    p.plot->renderTo(generator);
                    p.plot->setEnLabel(true, false);
                }},
            {"PDF image (*.pdf)", {"pdf"},
                [](QString s, Plot &p) {
                    QPdfWriter writer(s);
                    QSizeF size(100, 100);
                    writer.setPageSizeMM(size);
                    writer.setResolution(300);
                    p.plot->setEnLabel(false, false);
                    p.plot->renderTo(writer);
                    p.plot->setEnLabel(true, false);
                }},
            {"AVI video (*.avi)", {"avi"},
                [](QString s, Plot &p) { p.renderVideo(s, 800, 800, 10, 20); }},
        };

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
    QAction *aShader = new QAction(QIcon::fromTheme("draw-cuboid"), "Enable shader");
    QAction *aBar =
        new QAction(QIcon::fromTheme("paint-gradient-linear"), "Enable color bar");
    aShader->setCheckable(true);
    aBar->setCheckable(true);
    connect(aExport, &QAction::triggered, [&]() {
        QString selected;
        QStringList filters;
        for (auto &i : types) filters << i.a;
        QString filter = filters.join(";;");
        QString name =
            QFileDialog::getSaveFileName(this, "Export file", "", filter, &selected);
        if (name.isEmpty()) return;

        for (auto &i : types) {
            if (i.a == selected) {
                if (QFileInfo::exists(name))
                    i.c(name, *this);
                else {
                    bool ext = false;
                    for (auto &j : i.b)
                        if (name.endsWith(j)) ext = true;
                    if (!ext) name += "." + i.b[0];
                    i.c(name, *this);
                }
            }
        }
    });
    connect(aShader, &QAction::toggled, [=](bool b) { plot->setShader(b); });
    connect(aBar, &QAction::toggled, [=](bool b) { plot->setEnBar(b); });
    bar->addAction(aExport);
    bar->addAction(aShader);
    bar->addAction(aBar);
    l->addWidget(bar);

    plot = new PlotInternal(make_unique<Axis>(5, 5, 5),
        make_unique<Bar>(Gradient::HEIGHT_MAP, 5), std::move(range));
    plot->setQuantity(quantity, quantity.getTimes()[0]);
    l->addWidget(QWidget::createWindowContainer(plot));
    l->setMargin(0);
    setLayout(l);
    this->quantity = &quantity;
    data = &quantity.getDataAt(quantity.getTimes()[0], dim);
}

void Plot::setRotation(int x, int y, bool update) { plot->setRotation(x, y, update); }

void Plot::setPartition(float p, bool update) {
    const vector<float> &times = quantity->getTimes();
    float t1 = times[0];
    float t2 = times[times.size() - 1];
    float td = t2 - t1;
    setTime(t1 + td * p, update);
}

QSize Plot::sizeHint() const { return QSize(500, 500); }
QSize Plot::minimumSizeHint() const { return QSize(100, 100); }

void Plot::renderVideo(QString dir, int sizeX, int sizeY, int len, int fps) {
    QProgressDialog *progress = new QProgressDialog(this);
    QProgressBar *bar = new QProgressBar(progress);
    bar->setRange(0, 0);
    progress->setLabelText("Processing...");
    progress->setBar(bar);
    progress->show();

    QTemporaryDir *tmp = new QTemporaryDir();
    int total = len * fps;
    float lenStep = 1.0 / total;
    int sizeName = QString::number(total).length();
    QString format = "%1.jpg";
    QString f1, f2;
    f2 = format.arg(int(0), sizeName, 10, QLatin1Char('0'));
    float t = time;
    for (int i = 0; i < total; i++) {
        f1 = f2;
        f2 = format.arg(int(i), sizeName, 10, QLatin1Char('0'));
        setPartition(i * lenStep, false);
        QImage image(sizeX * 2, sizeY * 2, QImage::Format_ARGB32);
        QString filePath = tmp->filePath(f2);
        plot->renderTo(image);
        image.save(filePath, Q_NULLPTR, 100);
        QApplication::processEvents();
    }
    setTime(t, false);

    QProcess *process = new QProcess(this);
    process->setWorkingDirectory(tmp->path());
    connect(progress, &QProgressDialog::canceled, [=]() { process->kill(); });
    connect(process, QOverload<int>::of(&QProcess::finished), [=]() {
        delete tmp;
        progress->close();
    });
    QString cmd = "ffmpeg -y -r 10 -i %0" + QString::number(sizeName) +
        "d.jpg -c:v libx264 -crf 12 -s " + QString::number(sizeX) + "x" +
        QString::number(sizeY) + " " + dir;
    process->start(cmd);
}

PlotInternal::PlotInternal(
    unique_ptr<Axis> &&axis, unique_ptr<Bar> &&bar, unique_ptr<vector<QVector2D>> &&size)
    : model(new Model()) {
    shared_ptr<Axis> pa = move(axis);
    shared_ptr<vector<QVector2D>> ps = move(size);
    shared_ptr<Bar> pb = move(bar);

    engineGL = make_unique<EngineGL>(model, pa, pb, ps);
    engineQt = make_unique<EngineQt>(model, pa, pb, ps);
    engineGL->setBackGround(Qt::white);
    engineQt->setBackGround(Qt::transparent);
}

void PlotInternal::setRotation(int x, int y, bool update) {
    engineGL->setRotation(x, y);
    engineQt->setRotation(x, y);
    if (update) requestUpdate();
}

void PlotInternal::setLabel(float pos, bool update) {
    engineGL->setLabel(pos);
    engineQt->setLabel(pos);
    if (update) requestUpdate();
}

void PlotInternal::setShader(bool en, bool update) {
    engineGL->setShader(en);
    engineQt->setShader(en);
    if (update) requestUpdate();
}

void PlotInternal::setEnBar(bool en, bool update) {
    engineGL->setEnBar(en);
    engineQt->setEnBar(en);
    if (update) requestUpdate();
}

void PlotInternal::setEnLabel(bool en, bool update) {
    engineGL->setEnLabel(en);
    engineQt->setEnLabel(en);
    if (update) requestUpdate();
}

bool PlotInternal::setQuantity(SimQuantity &sq, float time, bool update) {
    bool ret = model->setQuantity(sq, time);
    if (update) requestUpdate();
    return ret;
}

void PlotInternal::renderTo(QPaintDevice &d) {
    QPainter p(&d);
    engineQt->resize(d.width(), d.height());
    engineQt->render(p);
    p.end();
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
}

void PlotInternal::mousePressEvent(QMouseEvent *event) {
    if ((event->buttons() & Qt::LeftButton) == 0) return;
    mouse = event->pos();
    rotation = engineGL->getRotation();
}

void PlotInternal::mouseReleaseEvent(QMouseEvent *) { setMouseGrabEnabled(false); }

void PlotInternal::initializeGL() { engineGL->initialize(); }

void PlotInternal::resizeGL(int w, int h) {
    engineGL->resize(w, h);
    engineQt->resize(w, h);
}

void PlotInternal::paintGL() {
    QPainter p(this);
    engineGL->render(p);
}
