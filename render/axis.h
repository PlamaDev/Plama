#ifndef AXIS_H
#define AXIS_H

#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QPair>
#include <QVector3D>
#include <QVector>

class Axis {
public:
    Axis(int sizeX, int sizeY, int sizeZ);
    QVector<QVector3D> &getPoint();
    QVector<QVector3D> &getColorF();
    QVector<const QColor *> &getColorQ();
    QVector<GLuint> &getIndex();
    QVector<QPair<int, int>> getSlice(int rotX, int rotY);
    QVector<QPair<int, int>> getSlice(bool xEnable, bool yEnable, bool zEnable,
        bool zStrait, bool xyStrait, bool invert);
    QVector<QPair<bool, QVector<QVector3D>>> &getNumber(
        int dir, bool xEnable, bool yEnable, bool zEnable, bool zStrait, bool xyStrait);
    QVector<QPair<bool, QVector<QVector3D>>> &getNumber(int rotX, int rotY);
    QMatrix4x4 getTransform(int rotX, int rotY);
    QMatrix4x4 getTransform(int dir, bool flipX);

private:
    QVector<QVector3D> point;
    QVector<QVector3D> colorF;
    QVector<const QColor *> colorQ;
    QVector<GLuint> index;
    QVector<QVector<QVector3D>> number;
    int sizeX, sizeY, sizeZ;
    QVector<int> offset;
    static const float DIST, EXTRA;
    static const QVector3D BLACK_F, GREY_F;
    static const QColor BLACK_Q, GREY_Q;
    static const int XY_X_B = 0;
    static const int XY_X_G = 1;
    static const int XY_Y_B = 2;
    static const int XY_Y_G = 3;
    static const int XZ = 4;
    static const int YZ = 5;
    static const int X_S = 6;
    static const int X_R = 7;
    static const int Y_S = 8;
    static const int Y_R = 9;
    static const int Z_S = 10;
    static const int Z_R = 11;
    static const int OFFSET = 12;
};

#endif // AXIS_H
