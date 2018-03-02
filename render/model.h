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
    Model();
    ~Model() = default;
    const std::vector<GLuint> &getIndexT(int dir) const;
    const std::vector<GLuint> &getIndexL(int dir) const;
    const std::vector<QVector3D> &getPoint() const;
    const std::vector<QVector3D> &getNormal() const;
    const std::vector<QVector3D> &getColor() const;
    const std::vector<QVector3D> &getPosition() const;
    bool setQuantity(SimQuantity &sq, float time);

private:
    enum enumType { LINE, HEIGHT, VECTOR };
    typedef const std::vector<float> &DATA;

    void genLine(DATA x, const std::vector<std::vector<float>> &y, QVector2D extreme);
    void genLine(DATA x, DATA y, QVector2D extreme);
    void genLineImpl(DATA x, DATA y, QVector2D extreme);
    void genHeight(DATA data, int sizeX, int sizeY, QVector2D extreme);
    void genVector(DATA dataX, DATA dataY, int sizeX, int sizeY);
    bool checkSame(Model::enumType type, std::vector<const void *> &&data);
    void checkSize(int point, int indexT, int indexL);

    static QPair<float, float> getExtreme(const float *data, int total);
    static std::vector<std::function<void(std::function<void(int)> &, int, int)>>
        indexFunc;

    enumType type;
    std::vector<const void *> data;
    std::vector<QVector3D> point;
    std::vector<QVector3D> normal;
    std::vector<QVector3D> color;
    std::vector<QVector3D> position;
    std::vector<std::vector<GLuint>> indexT;
    std::vector<std::vector<GLuint>> indexL;
};

#endif // MODEL_H
