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

class SamplerV : public Sampler {
public:
    SamplerV(const vector<double> &data, int step, int width)
        : Sampler(width, data.size() / width, step), data(data) {}
    double getRaw(int x, int y) const override { return data[y * this->sxo + x]; }
    unsigned long repr() const override { return (unsigned long)&data; }

private:
    const std::vector<double> &data;
};

class SamplerVV : public Sampler {
public:
    SamplerVV(const vector<vector<double>> &data, int step, int width)
        : Sampler(width, data.size() / width, step), data(data) {}
    double getRaw(int x, int y) const override { return data[y * this->sxo + x][0]; }
    unsigned long repr() const override { return (unsigned long)&data; }

private:
    const std::vector<std::vector<double>> &data;
};

Sampler::Sampler(int sizeX, int sizeY, int step)
    : step(step), sxo(sizeX), syo(sizeY), sxi(sxo / step), syi(syo / step),
      startX(sizeX % step / 2), startY(sizeY % step / 2), //
      sxf((sxo - 1) / (float)step), syf((syo - 1) / (float)step),
      ofsetX((step - 1) / (2.0 * step) + startX / (float)step),
      ofsetY((step - 1) / (2.0 * step) + startY / (float)step) {}

float Sampler::sizeXF() const { return sxf; }
float Sampler::sizeYF() const { return syf; }
int Sampler::sizeXI() const { return sxi; }
int Sampler::sizeYI() const { return syi; }
int Sampler::sizeXO() const { return sxo; }
int Sampler::sizeYO() const { return syo; }
float Sampler::offsetX() const { return ofsetX; }
float Sampler::offsetY() const { return ofsetY; }

double Sampler::get(int x, int y) const {
    this->conv(x, y);
    y += x / sxi;
    x = x % sxi;
    double total = 0;
    int startX = x * step + this->startX;
    int startY = y * step + this->startY;
    int endX = startX + step;
    int endY = startY + step;

    for (int ix = startX; ix < endX; ix++)
        for (int iy = startY; iy < endY; iy++) //
            total += this->getRaw(ix, iy);

    return total / (step * step);
}

bool Sampler::operator==(const Sampler *a) const {
    return repr() == a->repr() && this->step == a->step;
}

shared_ptr<Sampler> Sampler::gen(const vector<double> &data, int step, int width) {
    return std::make_shared<SamplerV>(data, step, width == -1 ? data.size() : width);
}

shared_ptr<Sampler> Sampler::gen(
    const vector<vector<double>> &data, int step, int width) {
    return std::make_shared<SamplerVV>(data, step, width == -1 ? data.size() : width);
}

void Sampler::conv(int &x, int &y) const {
    if (x < 0) x += sxi;
    if (y < 0) y += syi;
}

QString format(double value) {
    if (value == 0) return "0";
    double v = fabs(value);
    if (v < 0.001)
        return QString::number(value, 'e', 1);
    else if (v < 1)
        return QString::number(value, 'f', 3);
    else if (v < 1000)
        return QString::number(value, 'g', 3);
    else
        return QString::number(value, 'e', 1);
}
