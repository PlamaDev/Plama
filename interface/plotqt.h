#ifndef PLOTQT_H
#define PLOTQT_H

#include <QWidget>
#include <QVector>
#include "dataio/fileadapter.h"

class PlotQt : public QWidget {
public:
    PlotQt();
protected:
    void paintEvent(QPaintEvent *event);
private:
    int rotY = 0;
    int rotX = 0;
};

#endif // PLOTQT_H
