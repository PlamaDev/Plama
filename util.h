#ifndef UTIL_H
#define UTIL_H

#include <QColor>
#include <QPair>
#include <QString>
#include <QVector2D>
#include <QVector3D>
#include <QtDebug>
#include <QIcon>
#include <cmath>

extern float PI;
extern float PIH;

QPair<int, int> unify(int number, int radix);
QVector3D toV3D(const QColor &c);
void toPolar(double x, double y, QVector2D &ret);
void toCatsn(double r, double t, QVector2D &ret);
double magnitude(double x, double y);
float fastSin(float x);
float fastCos(float x);
QIcon getIcon(const QString &name);

template<typename A, typename B, typename C> class Trio {
public:
    A a;
    B b;
    C c;
    Trio(const A &a, const B &b, const C &c);
};

template<typename A, typename B, typename C>
Trio<A, B, C>::Trio(const A &a, const B &b, const C &c) : a(a), b(b), c(c) {}

#endif // UTIL_H
