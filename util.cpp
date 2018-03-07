#include "util.h"
#include <QtMath>
#include <cmath>

#define TABLE_SIZE 256

using namespace std;

float PI = acos(-1);
float PIH = PI / 2;

float *genTable() {
    static float data[TABLE_SIZE];
    for (int i = 0; i < TABLE_SIZE; i++) data[i] = sin(PI * 2 * i / TABLE_SIZE);
    return data;
}

float *SINE_TABLE = genTable();

QPair<int, int> unify(int number, int radix) {
    typedef QPair<int, int> P;
    if (number >= 0) return P(number / radix, number % radix);
    int e = number % radix;
    return e == 0 ? P(number / radix, 0) : P(number / radix - 1, radix + e);
}

QVector3D toV3D(const QColor &c) { return QVector3D(c.redF(), c.greenF(), c.blueF()); }

void toPolar(double x, double y, QVector2D &ret) {
    ret.setX(magnitude(x, y));

    if (x == 0) {
        ret.setY(y > 0 ? PIH : -PIH);
    } else {
        float a = atan(y / x);
        if (a < 0) a += PI;
        ret.setY(y >= 0 ? a : a + PI);
    }
}

void toCatsn(double r, double t, QVector2D &ret) {
    ret.setY(r * fastSin(t));
    ret.setX(r * fastCos(t));
}

// code from qt qmath.h
float fastSin(float x) {
    int si = int(x * (0.5 * TABLE_SIZE / PI));
    float d = x - si * (2.0 * PI / TABLE_SIZE);
    int ci = si + TABLE_SIZE / 4;
    si &= TABLE_SIZE - 1;
    ci &= TABLE_SIZE - 1;
    return SINE_TABLE[si] + (SINE_TABLE[ci] - 0.5 * SINE_TABLE[si] * d) * d;
}

float fastCos(float x) {
    int ci = int(x * (0.5 * TABLE_SIZE / PI));
    float d = x - ci * (2.0 * PI / TABLE_SIZE);
    int si = ci + TABLE_SIZE / 4;
    si &= TABLE_SIZE - 1;
    ci &= TABLE_SIZE - 1;
    return SINE_TABLE[si] - (SINE_TABLE[ci] + 0.5 * SINE_TABLE[si] * d) * d;
}

double magnitude(double x, double y) {
    return x == 0 ? abs(y) : abs(x) * sqrt(1 + pow(y / x, 2));
}

QIcon getIcon(const QString &name) {
    if (QIcon::hasThemeIcon(name)) return QIcon::fromTheme(name);
    else return QIcon(":/icons/" + name + ".svg");
}
