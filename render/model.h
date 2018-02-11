#ifndef MODEL_H
#define MODEL_H

#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QPair>
#include <QOpenGLFunctions>
#include "data/fileadapter.h"

class Gradient {
public:
    Gradient(QVector<QPair<QColor, float>> data, int steps = 50);
    const QColor &getColor(float pos) const;
private:
    QVector<QColor> cache;
    float step;
};

class Model {
public:
    ~Model() = default;
    const QVector<GLuint> &getIndexT(int dir) const;
    const QVector<GLuint> &getIndexL(int dir) const;
    const QVector<QVector3D> &getPoint() const;
    const QVector<QVector3D> &getNormal() const;
    const QVector<QVector3D> &getColorF() const;
    const QVector<QVector3D> &getPosition() const;
    const QVector<QColor const *> &getColorQ() const;

    static std::unique_ptr<Model> fromQuantity(SimQuantity &sq, int dim = 0);
    static std::unique_ptr<Model> fromQuantity(SimQuantity &sq, float time,
        int dim = 0);

private:
    Model(int sPoint, int sIndexT, int sIndexL);
    static QPair<float, float> getExtreme(const float *data, int total);
    static QVector<std::function<void(
        std::function<void(int)>&, int, int)>> indexFunc;
    static Gradient gradientHeightmap;

    QVector<QVector3D> point;
    QVector<QVector3D> normal;
    QVector<QVector3D> colorF;
    QVector<QColor const *> colorQ;
    QVector<QVector3D> position;
    QVector<QVector<GLuint>> indexT;
    QVector<QVector<GLuint>> indexL;
};


#endif // MODEL_H
