#include "plot.h"
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
    static vector<Trio<QString, vector<QString>, function<void(QString, Plot &)>>> types =
        {
            {"JPEG image (*.jpg *.jpeg *.jpe)", {"jpg", "jpeg", "jpe"},
                [](QString s, Plot &p) {
                    QImage image(4000, 4000, QImage::Format_ARGB32);
                    p.plot->renderTo(image);
                    QImage scaled = image.scaled(
                        2000, 2000, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                    scaled.save(s, "JPG", 100);
                }},
            {"SVG image (*.svg)", {"svg"},
                [](QString s, Plot &p) {
                    QSvgGenerator generator;
                    generator.setFileName(s);
                    generator.setSize(QSize(800, 800));
                    generator.setViewBox(QRect(0, 0, 800, 800));
                    p.plot->renderTo(generator);
                }},
            {"PDF image (*.pdf)", {"pdf"},
                [](QString s, Plot &p) {
                    QPdfWriter writer(s);
                    QSizeF size(100, 100);
                    writer.setPageSizeMM(size);
                    writer.setResolution(300);
                    p.plot->renderTo(writer);
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
    aShader->setCheckable(true);
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
    bar->addAction(aExport);
    bar->addAction(aShader);
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
            plot->setModel(Model::fromData(d, quantity->getSizeData()[0],
                quantity->getSizeData()[1], quantity->getExtreme()));
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

void Plot::renderVideo(QString dir, int sizeX, int sizeY, int len, int fps) {
    QProgressDialog *progress = new QProgressDialog(this);
    QProgressBar *bar = new QProgressBar(progress);
    bar->setRange(0, 0);
    progress->setLabelText("Processing...");
    progress->setBar(bar);
    progress->show();

    QTemporaryDir *tmp = new QTemporaryDir();
    int total = len * fps;
    float lenData = quantity->getTimes()[quantity->getTimes().size() - 1];
    float lenStep = lenData / total;
    const vector<float> *dataTmp = nullptr;
    int sizeName = QString::number(total).length();
    QString format = "%1.bmp";
    QString f1, f2;
    f2 = format.arg(int(0), sizeName, 10, QLatin1Char('0'));
    for (int i = 0; i < total; i++) {
        f1 = f2;
        f2 = format.arg(int(i), sizeName, 10, QLatin1Char('0'));

        const vector<float> *dataNew = &quantity->getDataAt(i * lenStep, 0);
        if (dataNew == dataTmp)
            QFile::copy(tmp->filePath(f1), tmp->filePath(f2));
        else {
            QImage image(sizeX * 2, sizeY * 2, QImage::Format_ARGB32);
            QString filePath = tmp->filePath(f2);
            plot->setModel(Model::fromData(*dataNew, quantity->getSizeData()[0],
                               quantity->getSizeData()[1], quantity->getExtreme()),
                false);
            plot->renderTo(image);
            image.save(filePath, Q_NULLPTR, 100);
            dataTmp = dataNew;
        }
        QApplication::processEvents();
    }

    QProcess *process = new QProcess(this);
    process->setWorkingDirectory(tmp->path());
    connect(progress, &QProgressDialog::canceled, [=]() { process->kill(); });
    connect(process, QOverload<int>::of(&QProcess::finished), [=]() {
        delete tmp;
        progress->close();
    });
    QString cmd = "ffmpeg -y -r 10 -i %0" + QString::number(sizeName) +
        "d.bmp -c:v libx264 -crf 12 -s " + QString::number(sizeX) + "x" +
        QString::number(sizeY) + " " + dir;
    process->start(cmd);
}

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

void PlotInternal::setShader(bool en) {
    engineGL->setShader(en);
    engineQt->setShader(en);
    requestUpdate();
}

void PlotInternal::setModel(unique_ptr<Model> model, bool update) {
    shared_ptr<Model> pm = move(model);
    engineGL->setModel(pm);
    engineQt->setModel(pm);
    if (update) requestUpdate();
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
