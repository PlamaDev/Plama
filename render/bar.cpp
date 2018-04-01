#include "bar.h"
#include "util.h"

const float Bar::DIFF = -0.00001;
const QVector3D Bar::BLACK = QVector3D(0.4, 0.4, 0.4);

Bar::Bar(const Gradient &gradient, int steps)
    : point(104 + 2 * steps), index(310 + 2 * steps), color(104 + 2 * steps),
      number(steps + 1), pos{PARALLEL, {1.3, 0.5, 0}, {0, 1, 0}, {1, 0, 0}},
      steps(steps) {
    int count = 0;
    float step = 1.0 / 50;
    for (int j = 0; j < 2; j++) {
        for (int i = 0; i < 51; i++) {
            point[count] = QVector3D(j, i * step, 0);
            color[count] = gradient.getColor(step / 2 + i * step);
            count++;
        }
    }
    for (int j = 0; j < 2; j++) {
        for (int i = 0; i < 2; i++) {
            point[count] = QVector3D(j, i, DIFF);
            color[count++] = BLACK;
        }
    }
    step = 1.0 / steps;
    for (int i = 1; i < steps; i++) {
        for (int j = 0; j < 2; j++) {
            point[count] = QVector3D(0.8 + 0.2 * j, i * step, DIFF);
            color[count++] = BLACK;
        }
    }
    count = 0;
    for (int i = 0; i < 50; i++) {
        index[count++] = 52 + i;
        index[count++] = 1 + i;
        index[count++] = i;
        index[count++] = 52 + i;
        index[count++] = i;
        index[count++] = 51 + i;
    }
    index[count++] = 102;
    index[count++] = 103;
    index[count++] = 103;
    index[count++] = 105;
    index[count++] = 105;
    index[count++] = 104;
    index[count++] = 104;
    index[count++] = 102;
    for (int i = 0; i < steps - 1; i++) {
        index[count++] = 106 + 2 * i;
        index[count++] = 107 + 2 * i;
    }

    for (int i = 0; i < steps + 1; i++) {
        number[i] = QVector3D(1.3, i * step, 0);
    }
}

const std::vector<QVector3D> &Bar::getPoint() const { return point; }
const std::vector<QVector3D> &Bar::getColor() const { return color; }
const std::vector<GLuint> &Bar::getIndex() const { return index; }
const std::vector<QVector3D> &Bar::getNumber() const { return number; }
const PositionInfo &Bar::getLabel() const { return pos; }
QPair<int, int> Bar::getSliceL() const { return QPair<int, int>(300, steps + 3); }
QPair<int, int> Bar::getSliceT() const { return QPair<int, int>(0, 100); }
