#ifndef QSPLITVIEW_H
#define QSPLITVIEW_H
#pragma clang diagnostic ignored "-Wunknown-warning-option"

#include <QWidget>
#include <QTabWidget>
#include <QSplitter>
#include <QTabBar>
#include <QFrame>
#include <QPushButton>
#include <QGridLayout>
#include <QRubberBand>

enum enumDirection { LEFT, RIGHT, UP, DOWN, NONE };

class QSVInternalISection;

class QSVInternalItem : public QWidget {
    Q_OBJECT
public:
    void split(QSVInternalItem *extra, enumDirection dir);
    void setParentItem(QSVInternalISection *parent);

protected:
    QSVInternalISection *parentItem;
    QGridLayout *layout;
};

class QSVInternalITab : public QSVInternalItem {
    Q_OBJECT
public:
    QSVInternalITab();
    void addWidget(QWidget *widget, QString name);
    void checkEmpty();

private:
    QTabWidget *tab;
    QRubberBand *rect;
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
};

class QSVInternalWTab : public QTabWidget {
    Q_OBJECT
public:
    QSVInternalWTab(QSVInternalITab *parentTab);
    QSVInternalITab *getItemTab();

private:
    QSVInternalITab *parentTab;
};

class QSVInternalBar : public QTabBar {
    Q_OBJECT
public:
    QSVInternalBar(QSVInternalWTab *parent);
    QSVInternalWTab *getParentTab();

protected:
    int index;
    QSVInternalWTab *parentTab;
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
};

class QSVInternalISection : public QSVInternalItem {
    Q_OBJECT
public:
    QSVInternalISection(Qt::Orientation orientation);
    void addWidget(QSVInternalItem *widget);
    void insertWidget(QSVInternalItem *widget, int index);
    void removeWidget(QSVInternalItem *widget);
    void split(QSVInternalItem *source, QSVInternalItem *extra,
        enumDirection dir);
    void clear();

private:
    QSplitter *splitter;
};


class QSplitViewHelper : public QWidget {
    Q_OBJECT
public:
    QSplitViewHelper(QSVInternalITab *parent);
private:
    QSVInternalITab *parentItem;
};


class QSplitView : public QWidget {
    Q_OBJECT
public:
    QSplitView();
    void addWidget(QWidget *w, QString text);
    void clear();

private:
    QSVInternalISection *root;
};


#endif // QSPLITVIEW_H
