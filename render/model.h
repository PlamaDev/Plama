#ifndef MODEL_H
#define MODEL_H

#include <QVector>
#include <QVector3D>
#include <QOpenGLFunctions>

class Model {
public:
  virtual ~Model() = default;
  virtual QVector<GLuint>& getIndex() = 0;
  virtual QVector<QVector3D>& getPoint() = 0;
  virtual QVector<QVector3D>& getNormal() = 0;
  virtual QVector3D getSize() = 0;
  static void getExtreme(const float* data, int total, float& max, float& min);
};

class ModelSmooth : public Model {
public:
  ~ModelSmooth() override = default;
  ModelSmooth(const float* data, int sizeX, int sizeY);
  ModelSmooth(const float* data, int sizeX, int sizeY, float max, float min);
  QVector<GLuint>& getIndex() override;
  QVector<QVector3D>& getPoint() override;
  QVector<QVector3D>& getNormal() override;
  QVector3D getSize() override;

private:
  void init(const float* data, int sizeX, int sizeY, float max, float min);
  QVector<GLuint> index;
  QVector<QVector3D> point;
  QVector<QVector3D> normal;
  QVector3D size;
};

class ModelInsert : public Model {
public:
  ~ModelInsert() override = default;
  ModelInsert(const float* data, int sizeX, int sizeY);
  ModelInsert(const float* data, int sizeX, int sizeY, float max, float min);
  QVector<GLuint>& getIndex() override;
  QVector<QVector3D>& getPoint() override;
  QVector<QVector3D>& getNormal() override;
  QVector3D getSize() override;


private:
  void init(const float* data, int sizeX, int sizeY, float max, float min);
  QVector<GLuint> index;
  QVector<QVector3D> point;
  QVector<QVector3D> normal;
  QVector3D size;
};

#endif // MODEL_H
