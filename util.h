#ifndef UTIL_H
#define UTIL_H

#include <QColor>
#include <QIcon>
#include <QPair>
#include <QString>
#include <QVector2D>
#include <QVector3D>
#include <QtDebug>
#include <cmath>
#include <memory>
#include <vector>

extern float PI;

typedef QPair<double, double> VectorD2D;
QPair<int, int> unify(int number, int radix);
QVector3D toV3D(const QColor &c);
void toPolar(double x, double y, VectorD2D &ret);
void toCatsn(double r, double t, VectorD2D &ret);
void toPolar(double x, double y, QVector2D &ret);
void toCatsn(double r, double t, QVector2D &ret);
double magnitude(double x, double y);
float fastSin(float x);
float fastCos(float x);
QIcon getIcon(const QString &name);

template <typename A, typename B, typename C> class Trio {
public:
    A a;
    B b;
    C c;
    Trio(const A &a, const B &b, const C &c) : a(a), b(b), c(c) {}
};

class Sampler {
public:
    Sampler(int sizeX, int sizeY, int step);
    virtual ~Sampler() = default;
    virtual unsigned long repr() const = 0;
    float sizeXF() const;
    float sizeYF() const;
    int sizeXI() const;
    int sizeYI() const;
    int sizeXO() const;
    int sizeYO() const;
    float offsetX() const;
    float offsetY() const;
    double get(int x, int y = 0) const;
    bool operator==(const Sampler *a) const;

    static std::shared_ptr<Sampler> gen(
        const std::vector<double> &data, int step, int width = -1);
    static std::shared_ptr<Sampler> gen(
        const std::vector<std::vector<double>> &data, int step, int width = -1);

protected:
    int step, sxo, syo, sxi, syi, startX, startY;
    float sxf, syf, ofsetX, ofsetY;

    virtual double getRaw(int x, int y = 0) const = 0;
    void conv(int &x, int &y) const;
};

#endif // UTIL_H
