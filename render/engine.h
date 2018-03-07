#ifndef ENGINE_H
#define ENGINE_H

#include "render/axis.h"
#include "render/bar.h"
#include "render/model.h"
#include <QColor>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QPaintDevice>
#include <QPainter>
#include <QScopedPointer>
#include <QVector3D>
#include <memory>

class Engine {
public:
    Engine(std::shared_ptr<Model> &model, std::shared_ptr<Axis> &axis,
        std::shared_ptr<Bar> &bar, std::shared_ptr<std::vector<QVector2D>> &size);
    virtual ~Engine() = default;
    virtual void initialize() = 0;
    virtual void render(QPainter &p) = 0;
    virtual void resize(int sizeX, int sizeY);
    QPoint getRotation();
    void setModel(std::shared_ptr<Model> &model);
    void setRotation(int rotX, int rotY);
    void setLabel(float pos);
    void setShader(bool en);
    void setEnBar(bool en);
    void setEnLabel(bool en);

protected:
    int rotX, rotY;
    int sizeX, sizeY;
    std::shared_ptr<Model> model;
    std::shared_ptr<Axis> axis;
    std::shared_ptr<Bar> bar;
    std::shared_ptr<std::vector<QVector2D>> size;
    float label;
    bool enShader, enBar, enLabel;
};

class EngineGL : public Engine, protected QOpenGLFunctions {
public:
    EngineGL(std::shared_ptr<Model> &model, std::shared_ptr<Axis> &axis,
        std::shared_ptr<Bar> &bar, std::shared_ptr<std::vector<QVector2D>> &size);
    void initialize() override;
    void render(QPainter &p) override;

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

class EngineQt : public Engine {
public:
    EngineQt(std::shared_ptr<Model> &model, std::shared_ptr<Axis> &axis,
        std::shared_ptr<Bar> &bar, std::shared_ptr<std::vector<QVector2D>> size);
    void initialize() override{};
    void render(QPainter &p) override;
    void render(QPaintDevice &p);
};

#endif // ENGINE_H
