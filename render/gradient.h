#ifndef GRADIENT_H
#define GRADIENT_H

#include <QColor>
#include <QPair>
#include <QVector3D>
#include <vector>

class Gradient {
public:
    Gradient(std::vector<QPair<QColor, float>> data, int steps = 50);
    const QVector3D &getColor(float pos) const;

    static Gradient HEIGHT_MAP;

private:
    std::vector<QVector3D> cache;
    float step;
};

#endif // GRADIENT_H
