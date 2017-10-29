#include "engine.h"
#include <QScopedPointer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QMatrix3x3>
#include <QMatrix>
#include <QColor>

void Engine::setModel(QScopedPointer<Model>& model) {
  this->model.swap(model);
}

void Engine::setRotation(int rotX, int rotY) {
  this->rotX = rotX;
  this->rotY = rotY;
}

void Engine::setBackGround(const QColor &color) {
  this->color = QVector3D(static_cast<float>(color.redF()), static_cast<float>(color.greenF()), static_cast<float>(color.blueF()));
}

void EngineSimple::initialize() {
  initializeOpenGLFunctions();
  QScopedPointer<QOpenGLShaderProgram> ptr(new QOpenGLShaderProgram());
  program.swap(ptr);
  program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":res/render/plain.vsh");
  program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":res/render/plain.fsh");
  program->link();
  argVecPos = program->attributeLocation("vecPos");
  argVecNormal = program->attributeLocation("vecNormal");
  argMatTrans = program->uniformLocation("matTrans");
  argMatColor = program->uniformLocation("matColor");
  argMatModel = program->uniformLocation("matModel");
  argVecLight = program->uniformLocation("vecLight");
  argFAmbient = program->uniformLocation("fAmbient");
  argFDiff = program->uniformLocation("fDiff");
  argVecView = program->uniformLocation("vecView");
  argVecSpec = program->uniformLocation("vecSpec");
  argMatNormal = program->uniformLocation("matNormal");
  glClearColor(color.x(), color.y(), color.z(), 1.0f);
  glEnable(GL_DEPTH_TEST);
}

void EngineSimple::render() {
  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_DEPTH_BUFFER_BIT);

  program->bind();

  program->setUniformValue(argVecSpec, 2.5f*QVector3D(0.20f, 0.20f, 0.15f));

  // view matrix
  QVector3D view(1.5, 0, 1.5);
  QMatrix4x4 rotation;
  rotation.rotate(-rotX, 0, 0, 1);
  rotation.rotate(-rotY, 0, 1, 0);
  program->setUniformValue(argVecView, rotation*view);

  // transform matrix
  QMatrix4x4 matTrans;
  matTrans.perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
  matTrans.lookAt(view, QVector3D(0, 0, 0), QVector3D(0, 0, 1));
  matTrans.rotate(rotY, 0, 1, 0);
  matTrans.rotate(rotX, 0, 0, 1);
  program->setUniformValue(argMatTrans, matTrans);

  QVector3D size = model->getSize();
  QMatrix4x4 matModel;
  matModel.scale(1.0f/size.x(), 1.0f/size.y(), 1.0f/size.z());
  matModel.translate(-size.x()/2.0f, -size.y()/2.0f, -size.z()/2.0f);
  program->setUniformValue(argMatModel, matModel);

  QMatrix4x4 matNormal = matModel.inverted();
  program->setUniformValue(argMatNormal, matNormal.transposed());

  QMatrix4x4 color(0.0f, 0.0f, 1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f);
  program->setUniformValue(argMatColor, color);

  QVector3D light(3.0f, -3.0f, 3.0f);
  program->setUniformValue(argVecLight, light);
  program->setUniformValue(argFAmbient, 0.6f);
  program->setUniformValue(argFDiff, 0.7f);

  glVertexAttribPointer(static_cast<GLuint>(argVecPos), 3, GL_FLOAT, GL_FALSE, 0, model->getPoint().constData());
  glVertexAttribPointer(static_cast<GLuint>(argVecNormal), 3, GL_FLOAT, GL_FALSE, 0, model->getNormal().constData());

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glDrawElements(GL_TRIANGLES, model->getIndex().size(), GL_UNSIGNED_INT, model->getIndex().constData());

  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(0);

  program->release();
}

void EngineSimple::resize(int sizeX, int sizeY) {
  (void)sizeX;
  (void)sizeY;
}

