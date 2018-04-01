#ifndef COMMON_H
#define COMMON_H
#include <QVector3D>

enum EnumPosition { LEFT, RIGHT, CENTER, PARALLEL, DISABLED };

struct PositionInfo {
    EnumPosition align;
    QVector3D base;
    QVector3D line;
    QVector3D expand;
};

#endif // COMMON_H
