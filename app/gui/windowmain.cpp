#include "windowmain.h"
#include "data/project.h"
#include "gui/plot.h"
#include <QAction>
#include <QDockWidget>
#include <QFileDialog>
#include <QHeaderView>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QToolBar>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QWidget>
#include <set>

using namespace std;

QTreeWidgetItem *generateTree(const SimTreeNode &n) {
    QTreeWidgetItem *ret = new QTreeWidgetItem(QStringList(n.getName()));

    ret->setData(0, 0x0101, QVariant::fromValue((void *)&n));
    for (const SimTreeNode &i : n.getChildren()) ret->addChild(generateTree(i));
    for (const SimQuantity &i : n.getData()) {
        QTreeWidgetItem *q = new QTreeWidgetItem(QStringList(i.getName()));
        q->setData(0, 0x0100, QVariant::fromValue((void *)&i));
        ret->addChild(q);
    }
    return ret;
}

QList<QTreeWidgetItem *> generateTree(const Project &a) {
    const vector<SimTreeNode> &nodes = a.getTopLevelNodes();

    QList<QTreeWidgetItem *> ret;
    for (auto &i : nodes) ret.append(generateTree(i));
    return ret;
}

WindowMain::WindowMain(QWidget *parent) : QMainWindow(parent), data() {
    static ProjectLoader m;
    static set<Plot *> activePlots;
    static set<QDockWidget *> activeDocks;

    QToolBar *toolbar = new QToolBar("Hello");
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setMaximum(10000);
    slider->setMinimum(0);
    toolbar->addWidget(slider);

    addToolBar(toolbar);
    QWidget *empty = new QWidget(this);
    setCentralWidget(empty);
    resize(700, 500);

    setDockOptions(QMainWindow::AnimatedDocks | QMainWindow::AllowNestedDocks |
        QMainWindow::AllowTabbedDocks);
    QDockWidget *dock = new QDockWidget("Data list", this);
    QTreeWidget *tree = new QTreeWidget(dock);
    dock->setWidget(tree);
    tree->header()->close();
    dock->setAllowedAreas(Qt::LeftDockWidgetArea);
    dock->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    addDockWidget(Qt::LeftDockWidgetArea, dock);

    connect(tree, &QTreeWidget::itemDoubleClicked, [=](QTreeWidgetItem *i) {
        QVariant q = i->data(0, 0x0100);
        if (q.isValid()) {
            QVariant p = i->parent()->data(0, 0x0101);
            SimTreeNode *sp = (SimTreeNode *)p.value<void *>();
            SimQuantity *sq = (SimQuantity *)q.value<void *>();
            vector<int> size = sq->getSizeData();

            if (sq->getError().isEmpty()) {
                QDockWidget *d =
                    new QDockWidget(sp->getAbbr() + '>' + sq->getName(), this);
                d->setAttribute(Qt::WA_DeleteOnClose);
                Plot *plot = new Plot(*sq);
                plot->setRotation(90, 90);
                plot->setPartition(slider->value() / (float)10000);
                d->setWidget(plot);

                d->setAllowedAreas(Qt::LeftDockWidgetArea);
                addDockWidget(Qt::LeftDockWidgetArea, d);
                if (activePlots.size() == 0)
                    splitDockWidget(dock, d, Qt::Horizontal);
                else {
                    QDockWidget *dst = *activeDocks.begin();
                    QSize s = dst->size();
                    splitDockWidget(
                        dst, d, s.width() > s.height() ? Qt::Horizontal : Qt::Vertical);
                }

                connect(d, &QDockWidget::visibilityChanged, [=](bool v) {
                    if (v) {
                        activeDocks.insert(d);
                        activePlots.insert(plot);
                    } else {
                        activeDocks.erase(d);
                        activePlots.erase(plot);
                        if (activePlots.size() == 0) {
                            QWidget *empty = new QWidget(this);
                            setCentralWidget(empty);
                        }
                    }
                });

            } else
                QMessageBox::warning(this, "Data Reading Error", sq->getError());
        }
    });

    connect(slider, &QSlider::sliderMoved, [](int i) {
        for (auto p : activePlots) p->setPartition(i / (float)10000);
    });

    QMenu *mFile = menuBar()->addMenu("File");
    QMenu *mFOpen = mFile->addMenu("Open");
    for (QString i : m.plugins()) {
        QAction *a = new QAction(i, this);
        mFOpen->addAction(a);
        connect(a, &QAction::triggered, [=]() {
            unique_ptr<Project> tmp = m.load(i);
            if (tmp->getError().isEmpty()) {
                data = std::move(tmp);
                tree->clear();
                tree->addTopLevelItems(generateTree(*data));
                auto set = activeDocks;
                for (auto i : set) i->close();
            } else
                QMessageBox::warning(this, "File Loading Error", tmp->getError());
        });
    }
}
