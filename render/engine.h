#ifndef ENGINE_H
#define ENGINE_H

#include "render/model.h"
#include <QOpenGLFunctions>
#include <QScopedPointer>
#include <QOpenGLShaderProgram>
#include <QColor>
#include <QVector3D>

class Engine {
public:
  Engine() = default;
  virtual ~Engine() = default;
  virtual void initialize() = 0;
  virtual void render() = 0;
  virtual void resize(int sizeX, int sizeY) = 0;
  void setModel(QScopedPointer<Model>& model);
  void setRotation(int rotX, int rotY);
  void setBackGround(const QColor& color);

protected:
  int rotX, rotY;
  QVector3D color;
  QScopedPointer<Model> model;
};

class EngineSimple : public Engine, protected QOpenGLFunctions {
public:
  void initialize();
  void render();
  void resize(int sizeX, int sizeY);

private:
  int argVecPos;
  int argVecNormal;

  int argMatTrans;
  int argMatColor;
  int argMatModel;
  int argVecLight;
  int argVecView;
  int argFAmbient;
  int argFDiff;
  int argVecSpec;
  int argMatNormal;

  QScopedPointer< QOpenGLShaderProgram> program;
};

#endif // ENGINE_H
