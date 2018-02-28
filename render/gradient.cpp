#include "gradient.h"

using namespace std;

Gradient Gradient::HEIGHT_MAP(
    {//
        {Qt::darkMagenta, 0.0}, {Qt::blue, 0.2}, {Qt::cyan, 0.4f}, {Qt::green, 0.6f},
        {Qt::yellow, 0.8f}, {Qt::red, 1.0f}},
    200);

Gradient::Gradient(vector<QPair<QColor, float>> data, int steps)
    : cacheQ(steps), cacheF(steps), step(1.0f / steps) {
    QColor c1 = data[0].first;
    QColor c2 = data[0].first;
    float p1 = 0;
    float p2 = 0;
    int count = 0;
    for (auto i : data) {
        c1 = c2;
        p1 = p2;
        c2 = i.first;
        p2 = i.second;
        for (float pos = count * step; pos < p2; count++, pos = count * step) {
            float diff1 = pos - p1;
            float diff2 = p2 - pos;
            float diffT = p2 - p1;
            int r = (c1.red() * diff2 + c2.red() * diff1) / diffT;
            int g = (c1.green() * diff2 + c2.green() * diff1) / diffT;
            int b = (c1.blue() * diff2 + c2.blue() * diff1) / diffT;
            cacheQ[count] = QColor(r, g, b);
        }
    }

    for (auto i = 0; i < steps; i++) {
        const QColor c = cacheQ[i];
        cacheF[i] = QVector3D(c.redF(), c.greenF(), c.blueF());
    }
}

const QColor &Gradient::getColorQ(float pos) const {
    int index = pos / step;
    int size = cacheQ.size();
    return cacheQ[index >= size ? size - 1 : index];
}

const QVector3D &Gradient::getColorF(float pos) const {
    int index = pos / step;
    int size = cacheQ.size();
    return cacheF[index >= size ? size - 1 : index];
}
