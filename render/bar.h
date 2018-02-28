#ifndef BAR_H
#define BAR_H

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
    const std::vector<QVector3D> &getColorF() const;
    const std::vector<const QColor *> &getColorQ() const;
    const std::vector<GLuint> &getIndex() const;
    const std::vector<QVector3D> &getNumber() const;
    QPair<int, int> getSliceL() const;
    QPair<int, int> getSliceT() const;
    QMatrix4x4 getTransform() const;

private:
    std::vector<QVector3D> point;
    std::vector<GLuint> index;
    std::vector<QVector3D> colorF;
    std::vector<const QColor *> colorQ;
    std::vector<QVector3D> number;
    int steps;
    static const float DIFF;
    static const QColor BLACK_Q;
    static const QVector3D BLACK_F;
};

#endif // BAR_H
