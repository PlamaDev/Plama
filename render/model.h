#ifndef MODEL_H
#define MODEL_H

#include "data/project.h"
#include <QOpenGLFunctions>
#include <QPair>
#include <QVector2D>
#include <QVector3D>

class Gradient {
public:
    Gradient(std::vector<QPair<QColor, float>> data, int steps = 50);
    const QColor &getColor(float pos) const;

private:
    std::vector<QColor> cache;
    float step;
};

class Model {
public:
    ~Model() = default;
    const std::vector<GLuint> &getIndexT(int dir) const;
    const std::vector<GLuint> &getIndexL(int dir) const;
    const std::vector<QVector3D> &getPoint() const;
    const std::vector<QVector3D> &getNormal() const;
    const std::vector<QVector3D> &getColorF() const;
    const std::vector<QVector3D> &getPosition() const;
    const std::vector<QColor const *> &getColorQ() const;

    static std::unique_ptr<Model> fromQuantity(SimQuantity &sq, int dim = 0);
    static std::unique_ptr<Model> fromQuantity(SimQuantity &sq, float time, int dim = 0);
    static std::unique_ptr<Model> fromData(
        const std::vector<float> &data, int sizeX, int sizeY, QVector2D extreme);

private:
    Model(int sPoint, int sIndexT, int sIndexL);
    static QPair<float, float> getExtreme(const float *data, int total);
    static std::vector<std::function<void(std::function<void(int)> &, int, int)>>
        indexFunc;
    static Gradient gradientHeightmap;

    std::vector<QVector3D> point;
    std::vector<QVector3D> normal;
    std::vector<QVector3D> colorF;
    std::vector<QColor const *> colorQ;
    std::vector<QVector3D> position;
    std::vector<std::vector<GLuint>> indexT;
    std::vector<std::vector<GLuint>> indexL;
};

#endif // MODEL_H
