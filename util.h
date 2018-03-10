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

template<typename T> using Vec = std::vector<T>;
template<typename T> using PtrU = std::unique_ptr<T>;
template<typename T> using PtrS = std::shared_ptr<T>;
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
    Trio(const A &a, const B &b, const C &c) : a(a), b(b), c(c) {}
};

template<typename T> class Accessor {
public:
    Accessor(int sizeX, int sizeY, int step)
        : step(step), sizeXActual(sizeX), sizeYActual(sizeY),
          sizeXImgI((sizeXActual + step - 1) / step),
          sizeYImgI((sizeYActual + step - 1) / step),
          sizeXImgF((sizeXActual - 1) / (float)step),
          sizeYImgF((sizeYActual - 1) / (float)step) {}
    virtual ~Accessor() = default;
    virtual unsigned long repr() = 0;
    float sizeXF() { return sizeXImgF; }
    float sizeYF() { return sizeYImgF; }
    int sizeXI() { return sizeXImgI; }
    int sizeYI() { return sizeYImgI; }

    T get(int x, int y = 0) {
        this->conv(x, y);
        y += x / this->sizeXImgI;
        x = x % this->sizeXImgI;
        int cnt = 0;
        T total = 0;
        int startX = x * step;
        int startY = y * step;
        int endX = startX + step;
        int endY = startY + step;

        for (int ix = startX; ix < sizeXActual && ix < endX; ix++) {
            for (int iy = startY; iy < sizeYActual && iy < endY; iy++) {
                total += this->getRaw(ix, iy);
                cnt++;
            }
        }
        return total / cnt;
    }

    bool operator==(const Accessor *a) {
        return repr() == a->repr() && this->step == a->step;
    }

    static PtrS<Accessor> gen(const Vec<T> &data, int step, int width = -1);
    static PtrS<Accessor> gen(const Vec<Vec<T>> &data, int step, int width = -1);

protected:
    int step, sizeXActual, sizeYActual, sizeXImgI, sizeYImgI;
    float sizeXImgF, sizeYImgF;

    virtual const T &getRaw(int x, int y = 0) = 0;
    inline void conv(int &x, int &y) {
        if (x < 0) x += sizeXImgI;
        if (y < 0) y += sizeYImgI;
    }
};

template<typename T> class AccessorV : public Accessor<T> {
public:
    AccessorV(const Vec<T> &data, int step, int width)
        : Accessor<T>(width, data.size() / width, step), data(data) {}

    const T &getRaw(int x, int y) override { return data[y * this->sizeXActual + x]; }

    unsigned long repr() override { return (unsigned long)&data; }

private:
    const std::vector<T> &data;
};

template<typename T> class AccessorVV : public Accessor<T> {
public:
    AccessorVV(const Vec<Vec<T>> &data, int step, int width)
        : Accessor<T>(width, data.size() / width, step), data(data) {}

    const T &getRaw(int x, int y) override { return data[y * this->sizeXActual + x][0]; }

    unsigned long repr() override { return (unsigned long)&data; }

private:
    const std::vector<std::vector<T>> &data;
};

template<typename T>
PtrS<Accessor<T>> Accessor<T>::gen(const Vec<T> &data, int step, int width) {
    return std::make_shared<AccessorV<T>>(data, step, width == -1 ? data.size() : width);
}

template<typename T>
PtrS<Accessor<T>> Accessor<T>::gen(const Vec<Vec<T>> &data, int step, int width) {
    return std::make_shared<AccessorVV<T>>(data, step, width == -1 ? data.size() : width);
}

#endif // UTIL_H
