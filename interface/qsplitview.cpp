#include "qsplitview.h"
#include "qplot.h"
#include <QDrag>
#include <QFrame>
#include <QGridLayout>
#include <QMimeData>
#include <QMouseEvent>
#include <QObject>
#include <QPushButton>
#include <QRubberBand>
#include <QSize>
#include <QSplitter>
#include <QTabBar>
#include <QWidgetItem>
#include <QtDebug>

QSplitViewHelper::QSplitViewHelper(QSVInternalITab *parent) {
    parentItem = parent;

    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);
    QPushButton *btnUp = new QPushButton("Up");
    QPushButton *btnDown = new QPushButton("Down");
    QPushButton *btnLeft = new QPushButton("Left");
    QPushButton *btnRight = new QPushButton("Right");
    QPushButton *btnNew = new QPushButton("New");
    btnUp->setMinimumWidth(10);
    btnDown->setMinimumWidth(10);
    btnLeft->setMinimumWidth(10);
    btnRight->setMinimumWidth(10);
    btnNew->setMinimumWidth(10);
    layout->addWidget(btnUp, 0, 1);
    layout->addWidget(btnLeft, 1, 0);
    layout->addWidget(btnRight, 1, 2);
    layout->addWidget(btnDown, 2, 1);
    layout->addWidget(btnNew, 1, 1);
    connect(btnRight, &QPushButton::clicked, [&](bool) {
        parentItem->split(new QSVInternalITab(), RIGHT);
    });
    connect(btnLeft, &QPushButton::clicked, [&](bool) {
        parentItem->split(new QSVInternalITab(), LEFT);
    });
    connect(btnUp, &QPushButton::clicked, [&](bool) {
        parentItem->split(new QSVInternalITab(), UP);
    });
    connect(btnDown, &QPushButton::clicked, [&](bool) {
        parentItem->split(new QSVInternalITab(), DOWN);
    });
    //connect(btnNew, &QPushButton::clicked, [&](bool) {
    //    parentItem->addWidget(new QPlot(), "Data");
    //});
}

void QSVInternalItem::split(QSVInternalItem *extra, enumDirection dir) {
    parentItem->split(this, extra, dir);
}

void QSVInternalItem::setParentItem(QSVInternalISection *parent) {
    parentItem = parent;
}

QSVInternalITab::QSVInternalITab() {
    this->layout = new QGridLayout();

    layout->setMargin(0);
    QTabWidget *tab = new QSVInternalWTab(this);
    layout->addWidget(tab, 0, 0, 3, 3);

    setLayout(layout);
    connect(tab, &QTabWidget::tabCloseRequested, [&](int index) {
        this->tab->removeTab(index);
        if (this->tab->count() == 0) {
            parentItem->removeWidget(this);
        }
    });

    this->tab = tab;
    tab->setTabsClosable(true);
    setAcceptDrops(true);
}

void QSVInternalITab::addWidget(QWidget *widget, QString name) {
    tab->addTab(widget, name);
}

void QSVInternalITab::checkEmpty() {
    if (tab->count() == 0)
        parentItem->removeWidget(this);
}

QSVInternalISection::QSVInternalISection(Qt::Orientation orientation) {
    QGridLayout *layout = new QGridLayout();
    QSplitter *splitter = new QSplitter(orientation);

    splitter->setChildrenCollapsible(false);
    layout->addWidget(splitter);
    layout->setMargin(0);
    setLayout(layout);
    this->splitter = splitter;
}

void QSVInternalISection::addWidget(QSVInternalItem *widget) {
    widget->setParentItem(this);
    splitter->addWidget(widget);
}

void QSVInternalISection::insertWidget(QSVInternalItem *widget, int index) {
    widget->setParentItem(this);
    splitter->insertWidget(index, widget);
}

void QSVInternalISection::removeWidget(QSVInternalItem *widget) {
    delete widget;
    if (splitter->count() == 0 && parentItem != nullptr) {
        parentItem->removeWidget(this);
    }
}

void QSVInternalISection::split(QSVInternalItem *source,
    QSVInternalItem *extra, enumDirection dir) {
    if ((splitter->orientation() == Qt::Horizontal &&
         (dir == LEFT || dir == RIGHT)) ||
        (splitter->orientation() == Qt::Vertical &&
         (dir == UP || dir == DOWN))) {
        insertWidget(extra, splitter->indexOf(
                source) + (dir == RIGHT || dir == DOWN ? 1 : 0));
    } else {
        QSVInternalISection *section = new QSVInternalISection(
                dir == LEFT || dir == RIGHT ? Qt::Horizontal : Qt::Vertical);
        QList <int> sizes = splitter->sizes();
        insertWidget(section, splitter->indexOf(source));
        if (dir == LEFT || dir == UP) {
            section->addWidget(extra);
            section->addWidget(source);
        } else {
            section->addWidget(source);
            section->addWidget(extra);
        }
        int size = dir == LEFT || dir == RIGHT ?
            section->width() : section->height();
        size /= 2;
        section->splitter->setSizes(QList <int> { size, size });
        splitter->setSizes(sizes);
    }
}

void QSVInternalISection::clear() {
    int len = splitter->count();

    for (int i = 0; i < len; i++) {
        delete splitter->widget(0);
    }
}

QSplitView::QSplitView() {
    QGridLayout *layout = new QGridLayout();

    root = new QSVInternalISection(Qt::Horizontal);
    layout->addWidget(root);
    layout->setMargin(0);
    setLayout(layout);
}

void QSplitView::addWidget(QWidget *w, QString text) {
    QSVInternalITab *tab = new QSVInternalITab();

    tab->addWidget(w, text);
    root->addWidget(tab);
}

void QSplitView::clear() {
    root->clear();
}

QSVInternalBar::QSVInternalBar(QSVInternalWTab *parent) {
    setAcceptDrops(true);
    parentTab = parent;
}

QSVInternalWTab *QSVInternalBar::getParentTab() {
    return parentTab;
}

void QSVInternalBar::mouseMoveEvent(QMouseEvent *event) {
    if (index >= 0) {
        QMimeData *mimeData = new QMimeData;
        mimeData->setProperty("index", index);
        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        if (drag->exec(Qt::MoveAction) == Qt::MoveAction) {
            parentTab->getItemTab()->checkEmpty();
        }
        event->accept();
    } else {
        QTabBar::mouseMoveEvent(event);
    }
}

void QSVInternalBar::mousePressEvent(QMouseEvent *event) {
    index = tabAt(event->pos());
    QTabBar::mousePressEvent(event);
}

void QSVInternalBar::dragEnterEvent(QDragEnterEvent *event) {
    QSVInternalBar *source = qobject_cast <QSVInternalBar *>(event->source());

    if (source) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void QSVInternalBar::dragMoveEvent(QDragMoveEvent *event) {
    QSVInternalBar *source = qobject_cast <QSVInternalBar *>(event->source());

    if (source) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void QSVInternalBar::dropEvent(QDropEvent *event) {
    QSVInternalBar *source = qobject_cast <QSVInternalBar *>(event->source());

    if (source) {
        int indexLocal = tabAt(event->pos());

        QSVInternalWTab *tab = source->getParentTab();
        int indexRemote = event->mimeData()->property("index").toInt();

        parentTab->insertTab(
            indexLocal, tab->widget(indexRemote), tab->tabText(indexRemote));
        parentTab->setCurrentIndex(indexLocal);
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

QSVInternalWTab::QSVInternalWTab(QSVInternalITab *tab)
    : parentTab(tab) {
    setTabBar(new QSVInternalBar(this));
}

QSVInternalITab *
QSVInternalWTab::getItemTab() {
    return parentTab;
}

void QSVInternalITab::dragEnterEvent(QDragEnterEvent *event) {
    QSVInternalBar *source = qobject_cast <QSVInternalBar *>(event->source());

    if (source) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    rect = new QRubberBand(QRubberBand::Rectangle, this);
}

void QSVInternalITab::dragMoveEvent(QDragMoveEvent *event) {
    QSVInternalBar *source = qobject_cast <QSVInternalBar *>(event->source());

    if (source) {
        if (event->pos().y() > tab->tabBar()->height()) {
            QPoint pos = event->pos();
            int h = height();
            int w = width();
            int hh = h / 2;
            int hw = w / 2;
            int y = pos.y();
            int x = pos.x();
            float r = (float)h / w;
            bool ul = (h - y) / (float)x > r;
            bool ur = y / (float)x < r;

            rect->setGeometry((!ul && ur) ? hw : 0, ul || ur ? 0 : hh,
                (ul ^ ur) ? hw : w, (ul ^ ur) ? h : hh);
            rect->show();
        } else {
            rect->hide();
        }
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void QSVInternalITab::dropEvent(QDropEvent *event) {
    QSVInternalBar *source = qobject_cast <QSVInternalBar *>(event->source());

    if (source) {
        int index = event->mimeData()->property("index").toInt();
        event->setDropAction(Qt::MoveAction);
        event->accept();
        if (event->pos().y() > tab->tabBar()->height()) {
            QPoint pos = event->pos();
            int h = height();
            int w = width();
            int y = pos.y();
            int x = pos.x();
            float r = (float)h / w;
            bool ul = (h - y) / (float)x > r;
            bool ur = y / (float)x < r;

            QSVInternalITab *t = new QSVInternalITab();

            split(t, ul ? (ur ? UP : LEFT) : (ur ? RIGHT : DOWN));
            t->addWidget(source->getParentTab()->widget(index),
                source->tabText(index));
        } else {
            tab->addTab(source->getParentTab()->widget(index),
                source->tabText(index));
        }
    }
    delete rect;
    rect = Q_NULLPTR;
}

void QSVInternalITab::dragLeaveEvent(QDragLeaveEvent *event) {
    delete rect;
    rect = Q_NULLPTR;
    QWidget::dragLeaveEvent(event);
}
