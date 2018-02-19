#include "windowmain.h"
#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QDockWidget>
#include <QTreeWidget>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QWidget>
#include <QSizePolicy>
#include <QToolBar>
#include <QSlider>
#include <QSet>
#include "data/project.h"
#include "gui/plot.h"


QTreeWidgetItem *generateTree(const SimTreeNode &n) {
    QTreeWidgetItem *ret = new QTreeWidgetItem(QStringList(n.getName()));

    ret->setData(0, 0x0101, QVariant::fromValue((void *)&n));
    for (const SimTreeNode &i : n.getChildren())
        ret->addChild(generateTree(i));
    for (const SimQuantity &i : n.getData()) {
        QTreeWidgetItem *q = new QTreeWidgetItem(QStringList(i.getName()));
        q->setData(0, 0x0100, QVariant::fromValue((void *)&i));
        ret->addChild(q);
    }
    return ret;
}

QList <QTreeWidgetItem *> generateTree(const Project &a) {
    const QList <SimTreeNode> &nodes = a.getTopLevelNodes();

    QList <QTreeWidgetItem *> ret;
    for (const SimTreeNode &i : nodes)
        ret.append(generateTree(i));
    return ret;
}

WindowMain::WindowMain(QWidget *parent)
    : QMainWindow(parent), data() {
    static ProjectLoader m;
    static QSet<Plot *> activePlots;

    //QWidget *w = new QWidget();
    //QHBoxLayout *l = new QHBoxLayout();
    //w->setLayout(l);
    QToolBar *toolbar = new QToolBar("Hello");
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setMaximum(10000);
    slider->setMinimum(0);
    toolbar->addWidget(slider);
    addToolBar(toolbar);

    setCentralWidget(nullptr);

    setDockOptions(QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks);
    QDockWidget *dock = new QDockWidget("Data list", this);
    QTreeWidget *tree = new QTreeWidget(dock);
    dock->setWidget(tree);
    tree->header()->close();
    addDockWidget(Qt::LeftDockWidgetArea, dock);
    connect(tree, &QTreeWidget::itemDoubleClicked,
    [=](QTreeWidgetItem *i) {
        QVariant q = i->data(0, 0x0100);
        if (q.isValid()) {
            QVariant p = i->parent()->data(0, 0x0101);
            SimTreeNode *sp = (SimTreeNode *)p.value <void *>();
            SimQuantity *sq = (SimQuantity *)q.value <void *>();
            QVector<int> size = sq->getSizeData();
            //l->addWidget(new OpenGLPlot(m));

            QDockWidget *d = new QDockWidget(sp->getAbbr() + '>' + sq->getName());
            Plot *plot = new Plot(*sq, 0);
            plot->setRotation(90, 90);
            d->setWidget(plot);
            plot->setPartition(slider->value() / (float) 10000);

            addDockWidget(Qt::RightDockWidgetArea, d);

            connect(d, &QDockWidget::visibilityChanged, [=](bool v) {
                if (v) activePlots.insert(plot);
                else activePlots.remove(plot);
            });
        }
    });

    connect(slider, &QSlider::sliderMoved, [](int i) {
        for(auto p : activePlots)
            p->setPartition(i / (float) 10000);
    });

    QMenu *mFile = menuBar()->addMenu("File");
    QMenu *mFOpen = mFile->addMenu("Open");
    for (QString i : m.plugins()) {
        QAction *a = new QAction(i, this);
        mFOpen->addAction(a);
        connect(a, &QAction::triggered, [ = ]() {
            data = m.load(i, QFileDialog::getOpenFileNames(
                        this, "Select Data Files", "", "Any Files(*.*)"));
            tree->clear();
            //s->clear();
            if (data) tree->addTopLevelItems(generateTree(*data));
        });
    }
}
