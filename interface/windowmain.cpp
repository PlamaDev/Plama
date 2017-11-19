#include "ui_windowmain.h"
#include "windowmain.h"

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
#include <QTabWidget>
#include <QStyle>
#include <QPushButton>
#include <QWidget>
#include <QGridLayout>
#include <QLabel>

#include "qplot.h"
#include "qsplitview.h"

WindowMain::WindowMain(QWidget *parent)
    : QMainWindow(parent) {
    setCentralWidget(nullptr);
    setDockOptions(QMainWindow::AllowNestedDocks |
                   QMainWindow::AllowTabbedDocks);
    QDockWidget *dock = new QDockWidget("Data list", this);
    QTreeWidget *tree = new QTreeWidget(dock);
    QTreeWidgetItem *particles =
        new QTreeWidgetItem(tree, QStringList("Particles"));
    particles->addChild(new QTreeWidgetItem(particles, QStringList("e")));
    particles->addChild(new QTreeWidgetItem(particles, QStringList("He+")));
    particles->addChild(new QTreeWidgetItem(particles, QStringList("He2+")));
    particles->addChild(new QTreeWidgetItem(particles, QStringList("He*")));
    particles->addChild(new QTreeWidgetItem(particles, QStringList("e*")));
    particles->addChild(new QTreeWidgetItem(particles, QStringList("He")));
    QTreeWidgetItem *reactions = new QTreeWidgetItem(tree,
                                                     QStringList() <<
                                                     "Reactions");

    particles->addChild(new QTreeWidgetItem(reactions,
                                            QStringList("e + He <=> e + He*")));
    particles->addChild(new QTreeWidgetItem(reactions,
                                            QStringList(
                                                "e + He* <=> e + e + He+")));
    particles->addChild(new QTreeWidgetItem(reactions,
                                            QStringList(
                                                "e + He <=> e + e + He+")));
    particles->addChild(new QTreeWidgetItem(reactions,
                                            QStringList(
                                                "He+ + He + He <=> He2+ + He")));
    particles->addChild(new QTreeWidgetItem(reactions,
                                            QStringList(
                                                "He* + He* <=> He+ + He + e*")));
    particles->addChild(new QTreeWidgetItem(reactions,
                                            QStringList(
                                                "He* + He* <=> He2+ + e*")));
    particles->addChild(new QTreeWidgetItem(reactions,
                                            QStringList(
                                                "He2+ + He <=> He+ + He + He")));
    dock->setWidget(tree);
    tree->header()->close();

    QWidget *w = new QWidget();
    w->setAutoFillBackground(true);
    QGridLayout *l = new QGridLayout();
    w->setLayout(l);
    QPlot *d = new QPlot();
    //QDiagram *d1 = new QDiagram();
    //QLabel *la = new QLabel();
    //la->setAutoFillBackground(true);
    //l->addWidget(la, 10, 10, 2, 2);
    l->addWidget(d, 0, 0, 10, 10);
    //l->addWidget(d1, 0, 0, 10, 10);

    //la->setFixedSize(10, 10);

    QSplitView *s = new QSplitView();
    s->addWidget(new QPlot(), "Plot");
    setCentralWidget(s);

    //setCentralWidget(w);


    //QDockWidget *dock1 = new QDockWidget("asd1", this);
    //QDockWidget *dock2 = new QDockWidget("asd2", this);
//    QDockWidget *dock3 = new QDockWidget("asd3", this);
//    QDockWidget *dock4 = new QDockWidget("asd4", this);
//    QDiagram *dia1 = new QDiagram(dock1);
    //QPlot *dia2 = new QPlot(dock2);
//    QDiagram *dia3 = new QDiagram(dock1);
//    QDiagram *dia4 = new QDiagram(dock1);
    //dock1->setWidget(w);
    //dock2->setWidget(dia2);
//    dock3->setWidget(dia3);
//    dock4->setWidget(dia4);



//    QTabWidget* tab = new QTabWidget(this);
//    tab->addTab(new QDiagram(this), "page1");
//    tab->addTab(new QDiagram(this), "page2");
//    tab->setTabsClosable(true);
//    setCentralWidget(tab);

    //QSplitView* split = new QSplitView();
    //split->addWidget(new QSplitViewItemTab());
    //setCentralWidget(split);

    addDockWidget(Qt::LeftDockWidgetArea, dock);
    //addDockWidget(Qt::RightDockWidgetArea, dock1);
    //addDockWidget(Qt::LeftDockWidgetArea, dock2);
//    addDockWidget(Qt::RightDockWidgetArea, dock3);
//    addDockWidget(Qt::LeftDockWidgetArea, dock4);
}

WindowMain::~WindowMain() {}
