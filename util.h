#ifndef UTIL_H
#define UTIL_H

#include <QPair>
#include <QString>

QPair<int, int> unify(int number, int radix);

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
