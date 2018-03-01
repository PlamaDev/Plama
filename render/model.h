#ifndef MODEL_H
#define MODEL_H

#include "data/project.h"
#include <QOpenGLContext>
#include <QPair>
#include <QVector2D>
#include <QVector3D>

#include "render/gradient.h"

class Model {
public:
    ~Model() = default;
    const std::vector<GLuint> &getIndexT(int dir) const;
    const std::vector<GLuint> &getIndexL(int dir) const;
    const std::vector<QVector3D> &getPoint() const;
    const std::vector<QVector3D> &getNormal() const;
    const std::vector<QVector3D> &getColor() const;
    const std::vector<QVector3D> &getPosition() const;

    void changeData(
        const std::vector<float> &data, int sizeX, int sizeY, QVector2D extreme);

    static std::unique_ptr<Model> fromQuantity(SimQuantity &sq, int dim = 0);
    static std::unique_ptr<Model> fromQuantity(SimQuantity &sq, float time, int dim = 0);
    static std::unique_ptr<Model> fromData(
        const std::vector<float> &data, int sizeX, int sizeY, QVector2D extreme);

private:
    Model(int sPoint, int sIndexT, int sIndexL);
    static QPair<float, float> getExtreme(const float *data, int total);
    static std::vector<std::function<void(std::function<void(int)> &, int, int)>>
        indexFunc;

    std::vector<QVector3D> point;
    std::vector<QVector3D> normal;
    std::vector<QVector3D> color;
    std::vector<QVector3D> position;
    std::vector<std::vector<GLuint>> indexT;
    std::vector<std::vector<GLuint>> indexL;
};

#endif // MODEL_H
