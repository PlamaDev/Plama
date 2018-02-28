#ifndef GRADIENT_H
#define GRADIENT_H

#include <QColor>
#include <QPair>
#include <QVector3D>
#include <vector>

class Gradient {
public:
    Gradient(std::vector<QPair<QColor, float>> data, int steps = 50);
    const QColor &getColorQ(float pos) const;
    const QVector3D &getColorF(float pos) const;

    static Gradient HEIGHT_MAP;

private:
    std::vector<QColor> cacheQ;
    std::vector<QVector3D> cacheF;
    float step;
};

#endif // GRADIENT_H
