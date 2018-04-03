#ifndef MODEL_H
#define MODEL_H

#include "data/project.h"
#include "gradient.h"
#include <QOpenGLContext>
#include <QVector3D>
#include <memory>
#include <vector>

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
    bool setQuantity(SimQuantity &sq, float time, int step);

private:
    enum enumType { LINE, HEIGHT, VECTOR };
    typedef std::shared_ptr<Sampler> DATA;

    bool checkSame(Model::enumType type, std::vector<DATA> &&data);
    void checkSize(int point, int indexT, int indexL);
    void genLine(DATA x, DATA y, VectorD2D extreme);
    void genHeight(DATA data, VectorD2D extreme);
    void genVector(DATA dataX, DATA dataY, VectorD2D extreme,
        float ratio = 1); // ratio = model x size / model y size

    static std::vector<std::function<void(std::function<void(int)> &, int, int)>>
        indexFunc;

    enumType type;
    std::vector<DATA> data;
    std::vector<QVector3D> point;
    std::vector<QVector3D> normal;
    std::vector<QVector3D> color;
    std::vector<QVector3D> position;
    std::vector<std::vector<GLuint>> indexT;
    std::vector<std::vector<GLuint>> indexL;
};

#endif // MODEL_H
