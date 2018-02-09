#ifndef ENGINE_H
#define ENGINE_H

#include <QOpenGLFunctions>
#include <QScopedPointer>
#include <QOpenGLShaderProgram>
#include <QColor>
#include <QVector3D>
#include <QPainter>
#include <memory>
#include "render/model.h"
#include "render/axis.h"

class Engine {
public:
    Engine(std::unique_ptr<Model> &model, std::unique_ptr<Axis> &axis);
    virtual ~Engine() = default;
    virtual void initialize() = 0;
    virtual void render(QPainter &p) = 0;
    virtual void resize(int sizeX, int sizeY);
    QPoint getRotation();
    //void setModel(std::unique_ptr<Model> &model);
    void setRotation(int rotX, int rotY);
    void setBackGround(const QColor &color);

protected:
    int rotX, rotY;
    int sizeX, sizeY;
    QVector3D color;
    std::unique_ptr<Model> model;
    std::unique_ptr<Axis> axis;
};

class EngineSimple : public Engine, protected QOpenGLFunctions {
public:
    EngineSimple(std::unique_ptr<Model> &model, std::unique_ptr<Axis> &axis);
    void initialize() override;
    void render(QPainter &p) override;
    void resize(int sizeX, int sizeY) override;

private:
    int argFlatVecPos;
    int argFlatVecPnt;
    int argFlatVecNormal;
    int argFlatVecColor;
    int argFlatVecView;
    int argFlatVecLight;
    int argFlatMatTrans;
    int argFlatMatModel;
    int argFlatMatNormal;

    int argPlainVecPnt;
    int argPlainVecColor;
    int argPlainMatModel;
    int argPlainMatTrans;

    std::unique_ptr<QOpenGLShaderProgram> programFlat;
    std::unique_ptr<QOpenGLShaderProgram> programPlain;
};

#endif // ENGINE_H
