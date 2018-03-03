#include "gradient.h"
#include "util.h"
#include <cmath>

using namespace std;

Gradient Gradient::HEIGHT_MAP(
    {//
        {Qt::blue, 0.0}, {Qt::cyan, 0.2f}, {Qt::green, 0.4f}, {Qt::yellow, 0.6f},
        {Qt::red, 0.8f}, {Qt::darkRed, 1.0f}},
    200);

Gradient::Gradient(vector<QPair<QColor, float>> data, int steps)
    : cache(steps), step(1.0f / steps) {
    QVector3D c1 = toV3D(data[0].first);
    QVector3D c2 = c1;
    float p1 = 0;
    float p2 = 0;
    int cnt = 0;
    for (auto i : data) {
        c1 = c2;
        p1 = p2;
        c2 = toV3D(i.first);
        p2 = i.second;
        for (float pos = cnt * step; pos < p2; cnt++, pos = cnt * step)
            cache[cnt] = (c1 * (p2 - pos) + c2 * (pos - p1)) / (p2 - p1);
    }
}

const QVector3D &Gradient::getColor(float pos) const {
    int index = pos / step;
    int size = cache.size();
    return cache[index >= size ? size - 1 : index];
}
