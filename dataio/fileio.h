#ifndef FILEIO_H
#define FILEIO_H

#include <QVector>

class Fileio
{
public:
  Fileio();
  const float *getData();
  int getSizeX();
  int getSizeY();

private:
  QVector<float> data;
  int sizeX;
  int sizeY;
};

#endif // FILEIO_H
