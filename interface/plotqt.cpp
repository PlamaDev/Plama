#include "plotqt.h"
#include <QPainter>
#include <QVector>
#include <Qt>
#include <QMatrix4x4>
#include <QVector3D>

PlotQt::PlotQt() {}

void PlotQt::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    //painter.setRenderHint(painter.Antialiasing, true);
    QVector3D view(1.5, 0, 1.5);
    QMatrix4x4 matTrans;
    QMatrix4x4 matSize = QMatrix4x4((float)event->rect().width() / 2, 0, 0, 0,
            0, (float)event->rect().height() / 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
    QMatrix4x4 matScreen = QMatrix4x4(-1, 0, 0, 0,
            0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
    QPoint ptCenter = QPoint(event->rect().center());

    //matTrans.perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    matTrans.lookAt(view, QVector3D(0, 0, 0), QVector3D(0, 0, 1));

    //matTrans.rotate(rotY, 0, 1, 0);
    //matTrans.rotate(rotX, 0, 0, 1);

    painter.setBrush(Qt::green);
    QVector <QVector3D> v3ds;
    v3ds << QVector3D(0, -1, 0);
    v3ds << QVector3D(0, 1, 0);
    v3ds << QVector3D(0, 0, 1);

    QVector <QPoint> pts = QVector <QPoint>(v3ds.size());
    for (int i = 0; i < v3ds.size(); i++) {
        pts[i] = (matScreen * matSize * matTrans * v3ds[i]).toPoint() +
            ptCenter;
    }

    for (int i = 0; i < 500; i++) {
        painter.drawPolygon(pts.constData(), 3);
    }
}
