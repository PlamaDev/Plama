#include "util.h"
#include <cmath>

#define TABLE_SIZE 256

using namespace std;

float PI = acos(-1);

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

void toPolar(double x, double y, double &r, double &t) {
    static float PI_1O2 = PI / 2;
    static float PI_3O2 = PI * 3 / 2;

    r = magnitude(x, y);

    if (x == 0)
        t = y > 0 ? PI_1O2 : PI_3O2;
    else if (y == 0)
        t = x > 0 ? 0 : PI;
    else {
        float a = atan(y / x);
        if (a < 0) a += PI;
        t = y >= 0 ? a : a + PI;
    }
}

void toPolar(double x, double y, VectorD2D &ret) {
    double r = 0, t = 0;
    toPolar(x, y, r, t);
    ret.first = r;
    ret.second = t;
}

void toCatsn(double r, double t, VectorD2D &ret) {
    ret.first = r * fastCos(t);
    ret.second = r * fastSin(t);
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
    if (QIcon::hasThemeIcon(name))
        return QIcon::fromTheme(name);
    else
        return QIcon(":/icons/" + name + ".svg");
}

void toPolar(double x, double y, QVector2D &ret) {
    double r = 0, t = 0;
    toPolar(x, y, r, t);
    ret.setX(r);
    ret.setY(t);
}

void toCatsn(double r, double t, QVector2D &ret) {
    ret.setX(r * fastCos(t));
    ret.setY(r * fastSin(t));
}

class AccessorV : public Sampler {
public:
    AccessorV(const Vec<double> &data, int step, int width)
        : Sampler(width, data.size() / width, step), data(data) {}
    double getRaw(int x, int y) const override { return data[y * this->sXActual + x]; }
    unsigned long repr() const override { return (unsigned long)&data; }

private:
    const std::vector<double> &data;
};

class AccessorVV : public Sampler {
public:
    AccessorVV(const Vec<Vec<double>> &data, int step, int width)
        : Sampler(width, data.size() / width, step), data(data) {}
    double getRaw(int x, int y) const override { return data[y * this->sXActual + x][0]; }
    unsigned long repr() const override { return (unsigned long)&data; }

private:
    const std::vector<std::vector<double>> &data;
};

double Sampler::get(int x, int y) {
    this->conv(x, y);
    y += x / this->sXImgI;
    x = x % this->sXImgI;
    int cnt = 0;
    double total = 0;
    int startX = x * step;
    int startY = y * step;
    int endX = startX + step;
    int endY = startY + step;

    for (int ix = startX; ix < sXActual && ix < endX; ix++) {
        for (int iy = startY; iy < sYActual && iy < endY; iy++) {
            total += this->getRaw(ix, iy);
            cnt++;
        }
    }
    return total / cnt;
}

bool Sampler::operator==(const Sampler *a) {
    return repr() == a->repr() && this->step == a->step;
}

PtrS<Sampler> Sampler::gen(const Vec<double> &data, int step, int width) {
    return std::make_shared<AccessorV>(data, step, width == -1 ? data.size() : width);
}

PtrS<Sampler> Sampler::gen(const Vec<Vec<double>> &data, int step, int width) {
    return std::make_shared<AccessorVV>(data, step, width == -1 ? data.size() : width);
}

void Sampler::conv(int &x, int &y) {
    if (x < 0) x += sXImgI;
    if (y < 0) y += sYImgI;
}
