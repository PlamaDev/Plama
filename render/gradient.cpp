#include "gradient.h"

using namespace std;

Gradient Gradient::HEIGHT_MAP(
    {//
        {Qt::blue, 0.0}, {Qt::cyan, 0.2f}, {Qt::green, 0.4f}, {Qt::yellow, 0.6f},
        {Qt::red, 0.8f}, {Qt::darkRed, 1.0f}},
    200);

Gradient::Gradient(vector<QPair<QColor, float>> data, int steps)
    : cache(steps), step(1.0f / steps) {
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
            cache[count] = QVector3D(r / 255.0, g / 255.0, b / 255.0);
        }
    }
}

const QVector3D &Gradient::getColor(float pos) const {
    int index = pos / step;
    int size = cache.size();
    return cache[index >= size ? size - 1 : index];
}
