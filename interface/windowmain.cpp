#include "windowmain.h"

#include <functional>
#include <Qt>
#include <QComboBox>
#include <QDebug>
#include <QLineEdit>
#include <QPushButton>
#include <QStringList>
#include <QKeySequence>
#include <QDockWidget>
#include <QListWidget>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QHeaderView>
#include <QTabWidget>
#include <QStyle>
#include <QPushButton>
#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QVariant>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>

#include "dataio/fileadapter.h"
#include "qplot.h"
#include "qsplitview.h"

QTreeWidgetItem *generateTree(const SimTreeNode& n) {
    QTreeWidgetItem *ret = new QTreeWidgetItem(QStringList(n.getName()));

    ret->setData(0, 0x0101, QVariant::fromValue((void *)&n));
    for (const SimTreeNode&i : n.getChildren()) {
        ret->addChild(generateTree(i));
    }
    for (const SimQuantity&i : n.getData()) {
        QTreeWidgetItem *q = new QTreeWidgetItem(QStringList(i.getName()));
        // TODO
        QVariant v = QVariant::fromValue((void *)&i);
        SimQuantity *sq = (SimQuantity *)v.value <void *>();
        q->setData(0, 0x0100, QVariant::fromValue((void *)&i));
        ret->addChild(q);
    }
    return ret;
}

QList <QTreeWidgetItem *> generateTree(const FileAdapter& a) {
    const QList <SimTreeNode>&nodes = a.getTopLevelNodes();

    QList <QTreeWidgetItem *> ret;
    for (const SimTreeNode&i : nodes) {
        ret.append(generateTree(i));
    }
    return ret;
}

WindowMain::WindowMain(QWidget *parent)
    : QMainWindow(parent), data() {
    static FileAdapterManager m;
    QSplitView *s = new QSplitView();

    setCentralWidget(s);
    //setDockOptions(QMainWindow::AllowNestedDocks |
    //    QMainWindow::AllowTabbedDocks);
    QDockWidget *dock = new QDockWidget("Data list", this);
    QTreeWidget *tree = new QTreeWidget(dock);
    dock->setWidget(tree);
    tree->header()->close();
    addDockWidget(Qt::LeftDockWidgetArea, dock);
    connect(tree, &QTreeWidget::itemDoubleClicked,
        [s](QTreeWidgetItem *i) {
        QVariant q = i->data(0, 0x0100);
        if (q.isValid()) {
            QVariant p = i->parent()->data(0, 0x0101);
            SimTreeNode *sp = (SimTreeNode *)p.value <void *>();
            SimQuantity *sq = (SimQuantity *)q.value <void *>();
            s->addWidget(new QPlot(*sq),
                sp->getAbbr() + '>' + sq->getName());
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
            s->clear();
            if (data) tree->addTopLevelItems(generateTree(*data));
        });
    }
}

WindowMain::~WindowMain() {}
