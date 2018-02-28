#include "util.h"

QPair<int, int> unify(int number, int radix) {
    typedef QPair<int, int> P;
    if (number >= 0) return P(number / radix, number % radix);
    int e = number % radix;
    return e == 0 ? P(number / radix, 0) : P(number / radix - 1, radix + e);
}

QVector3D toV3D(const QColor &c) { return QVector3D(c.redF(), c.greenF(), c.blueF()); }
