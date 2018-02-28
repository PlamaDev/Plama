#ifndef AXIS_H
#define AXIS_H

#include <QMatrix4x4>
#include <QOpenGLContext>
#include <QPair>
#include <QVector3D>
#include <QVector>
#include <vector>

class Axis {
public:
    Axis(int sizeX, int sizeY, int sizeZ);
    const std::vector<QVector3D> &getPoint() const;
    const std::vector<QVector3D> &getColorF() const;
    const std::vector<const QColor *> &getColorQ() const;
    const std::vector<GLuint> &getIndex() const;
    std::vector<QPair<int, int>> getSlice(int rotX, int rotY);
    std::vector<QPair<int, int>> getSlice(bool xEnable, bool yEnable, bool zEnable,
        bool zStrait, bool xyStrait, bool invert);
    const std::vector<QPair<bool, std::vector<QVector3D>>> &getNumber(int dir,
        bool xEnable, bool yEnable, bool zEnable, bool zStrait, bool xyStrait) const;
    const std::vector<QPair<bool, std::vector<QVector3D>>> &getNumber(
        int rotX, int rotY) const;
    QMatrix4x4 getTransform(int rotX, int rotY) const;
    QMatrix4x4 getTransform(int dir, bool flipX) const;

private:
    std::vector<QVector3D> point;
    std::vector<QVector3D> colorF;
    std::vector<const QColor *> colorQ;
    std::vector<GLuint> index;
    std::vector<std::vector<QVector3D>> number;
    int sizeX, sizeY, sizeZ;
    std::vector<int> offset;
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
