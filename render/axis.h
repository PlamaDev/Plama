#ifndef AXIS_H
#define AXIS_H

#include <QVector>
#include <QVector3D>
#include <QOpenGLFunctions>
#include <QPair>
#include <QMatrix4x4>

class Axis {
public:
    Axis(int sizeX, int sizeY, int sizeZ);
    QVector<QVector3D> &getPoint();
    QVector<QVector3D> &getColor();
    QVector<GLuint> &getIndex();
    QVector<QPair<int, int>> getSlice(int rotX, int rotY);
    QVector<QPair<int, int>> getSlice(bool xEnable, bool yEnable, bool zEnable,
            bool zStrait, bool xyStrait, bool invert);
    QVector<QPair<bool, QVector<QVector3D>>> &getNumber(int dir,
        bool xEnable, bool yEnable, bool zEnable,bool zStrait, bool xyStrait);
    QVector<QPair<bool, QVector<QVector3D>>> &getNumber(int rotX,
        int rotY);
    QMatrix4x4 getTransform(int rotX, int rotY);
    QMatrix4x4 getTransform(int dir, bool flipX);
private:
    QVector<QVector3D> point;
    QVector<QVector3D> color;
    QVector<GLuint> index;
    QVector<QVector<QVector3D>> number;
    int sizeX, sizeY, sizeZ;
    QVector<int> offset;
    static const float dist, extra;
    static const QVector3D black, grey;
    static const int XY_X_B = 0;
    static const int XY_X_G = 1;
    static const int XY_Y_B = 2;
    static const int XY_Y_G = 3;
    static const int XZ_X = 4;
    static const int XZ_Z_B = 5;
    static const int XZ_Z_G = 6;
    static const int YZ_Y = 7;
    static const int YZ_Z = 8;
    static const int X_S = 9;
    static const int X_R = 10;
    static const int Y_S = 11;
    static const int Y_R = 12;
    static const int Z_S = 13;
    static const int Z_R = 14;
    static const int OFFSET = 15;
};

#endif // AXIS_H
