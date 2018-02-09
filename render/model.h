#ifndef MODEL_H
#define MODEL_H

#include <QVector>
#include <QVector3D>
#include <QPair>
#include <QOpenGLFunctions>

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
    virtual ~Model() = default;
    virtual const QVector<GLuint> &getIndex(int dir) const = 0;
    virtual const QVector<QVector3D> &getPoint() const = 0;
    virtual const QVector<QVector3D> &getNormal() const = 0;
    virtual const QVector<QVector3D> &getColorF() const = 0;
    virtual const QVector<QVector3D> &getPosition() const = 0;
    virtual const QVector<QColor const *> &getColorQ() const = 0;
    virtual const QVector3D &getSize() const = 0;
    virtual bool isFlat() const = 0;
    static QPair<float, float> getExtreme(const float *data, int total);
    static QVector<std::function<void(
        std::function<void(int)>&, int, int)>> indexFunc;
    static Gradient gradientHeightmap;
};

class ModelDivided : public Model {
public:
    ModelDivided(const float *data, int sizeX, int sizeY, QVector3D size);
    ModelDivided(const float *data, int sizeX, int sizeY,
        QPair<float, float> extreme, QVector3D size);
    virtual ~ModelDivided() = default;
    const QVector<GLuint> &getIndex(int dir) const override;
    const QVector<QVector3D> &getPoint() const override;
    const QVector<QVector3D> &getNormal() const override;
    const QVector<QVector3D> &getColorF() const override;
    const QVector<QColor const *> &getColorQ() const override;
    const QVector<QVector3D> &getPosition() const override;
    const QVector3D &getSize() const override;
    bool isFlat() const override;
private:
    QVector<QVector3D> point;
    QVector<QVector3D> normal;
    QVector<QVector3D> colorF;
    QVector<QVector3D> position;
    QVector<QColor const *> colorQ;
    QVector<GLuint> index;
    QVector3D size;
    Gradient gradient;
    int dir;
    static QVector<QVector<GLuint>> order;
};

#endif // MODEL_H
