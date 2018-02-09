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
#include "data/fileadapter.h"
#include "gui/openglplot.h"


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

QList <QTreeWidgetItem *> generateTree(const FileAdapter &a) {
    const QList <SimTreeNode> &nodes = a.getTopLevelNodes();

    QList <QTreeWidgetItem *> ret;
    for (const SimTreeNode &i : nodes)
        ret.append(generateTree(i));
    return ret;
}

WindowMain::WindowMain(QWidget *parent)
    : QMainWindow(parent), data() {
    static FileAdapterManager m;

    QWidget *w = new QWidget();
    QHBoxLayout *l = new QHBoxLayout();
    w->setLayout(l);
    setCentralWidget(w);

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

            std::unique_ptr<Model> m = std::make_unique<ModelDivided>(
                    sq->getDataAt(0).constData(), sq->getSizeData()[0],
                    sq->getSizeData()[1], QVector3D(1, 1, 1));
            l->addWidget(new OpenGLPlot(m));
            //QDockWidget *d = new QDockWidget(sp->getAbbr() + '>' + sq->getName());
            //d->setWidget(new OpenGLPlot(m));
            //addDockWidget(Qt::RightDockWidgetArea, d);
        }
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
