#ifndef BAR_H
#define BAR_H

#include "common.h"
#include "render/gradient.h"
#include <QColor>
#include <QMatrix4x4>
#include <QOpenGLContext>
#include <QPair>
#include <QVector3D>
#include <vector>

class Bar {
public:
    Bar(const Gradient &gradient, int steps);
    const std::vector<QVector3D> &getPoint() const;
    const std::vector<QVector3D> &getColor() const;
    const std::vector<GLuint> &getIndex() const;
    const std::vector<QVector3D> &getNumber() const;
    const PositionInfo &getLabel() const;
    QPair<int, int> getSliceL() const;
    QPair<int, int> getSliceT() const;

private:
    std::vector<QVector3D> point;
    std::vector<GLuint> index;
    std::vector<QVector3D> color;
    std::vector<QVector3D> number;
    PositionInfo pos;
    int steps;
    static const float DIFF;
    static const QVector3D BLACK;
};

#endif // BAR_H
