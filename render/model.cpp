#include <render/model.h>
#include <QVector>
#include <QVector3D>
#include <QOpenGLFunctions>
#include <memory>

ModelSmooth::ModelSmooth(const float *data, int sizeX, int sizeY) {
    float max, min;

    Model::getExtreme(data, sizeX * sizeY, max, min);
    init(data, sizeX, sizeY, max, min);
}

ModelSmooth::ModelSmooth(const float *data, int sizeX, int sizeY, float max,
    float min) {
    init(data, sizeX, sizeY, max, min);
}

void ModelSmooth::init(const float *data, int sizeX, int sizeY, float max,
    float min) {
    index = QVector <GLuint>((sizeX - 1) * (sizeY - 1) * 6);
    point = QVector <QVector3D>(sizeX * sizeY);
    normal = QVector <QVector3D>(sizeX * sizeY);
    int total = sizeX * sizeY;
    float range = max - min;

    QVector <float> dataConverted(total);

    for (int i = 0; i < total; i++) {
        dataConverted[i] = (data[i] - min) / range;
    }

    for (int y = 0; y < sizeY; y++) {
        for (int x = 0; x < sizeX; x++) {
            QVector3D vNormal;
            int count = 0;
            int current = y * sizeX + x;
            bool bUp = x != 0;
            bool bDown = x != sizeX - 1;
            bool bRight = y != sizeY - 1;
            bool bLeft = y != 0;
            int iUp = current - 1;
            int iDown = current + 1;
            int iRight = current + sizeX;
            int iLeft = current - sizeX;
            float fCurrent = dataConverted[current];
            float fLeft = dataConverted[iLeft < 0 ? 0 : iLeft];
            float fRight = dataConverted[iRight >= total ? 0 : iRight];
            float fDown = dataConverted[iDown >= total ? 0 : iDown];
            float fUp = dataConverted[iUp < 0 ? 0 : iUp];
            QVector3D vLeft = QVector3D(0, -1, fLeft - fCurrent);
            QVector3D vRight = QVector3D(0, 1, fRight - fCurrent);
            QVector3D vDown = QVector3D(1, 0, fDown - fCurrent);
            QVector3D vUp = QVector3D(-1, 0, fUp - fCurrent);

            point[current] = QVector3D(x, y, dataConverted[current]);

            auto accumulator =
                [&vNormal, &count](bool b1, bool b2, QVector3D v1,
                                   QVector3D v2) {
                    if (b1 && b2) {
                        QVector3D buf = QVector3D::crossProduct(v1, v2);
                        buf.normalize();
                        vNormal += buf;
                        count++;
                    }
                };

            accumulator(bLeft, bDown, vLeft, vDown);
            accumulator(bDown, bRight, vDown, vRight);
            accumulator(bRight, bUp, vRight, vUp);
            accumulator(bUp, bLeft, vUp, vLeft);
            vNormal.normalize();

            normal[current] = vNormal;
        }
    }

    int count = 0;
    for (int y = 0; y < sizeY - 1; y++) {
        for (int x = 0; x < sizeX - 1; x++) {
            GLuint ld = static_cast <GLuint>(y * sizeX + x);
            GLuint rd = static_cast <GLuint>(ld + 1);
            GLuint lu = ld + static_cast <GLuint>(sizeX);
            GLuint ru = rd + static_cast <GLuint>(sizeX);
            index[count++] = lu;
            index[count++] = ld;
            index[count++] = rd;
            index[count++] = rd;
            index[count++] = ru;
            index[count++] = lu;
        }
    }

    size = QVector3D(sizeX - 1, sizeY - 1, 1);
}

QVector <GLuint>& ModelSmooth::getIndex() {
    return index;
}

QVector <QVector3D>& ModelSmooth::getPoint() {
    return point;
}

QVector <QVector3D>& ModelSmooth::getNormal() {
    return normal;
}

QVector3D ModelSmooth::getSize() {
    return size;
}

ModelInsert::ModelInsert(const float *data, int sizeX, int sizeY) {
    float max, min;

    Model::getExtreme(data, sizeX * sizeY, max, min);
    init(data, sizeX, sizeY, max, min);
}

ModelInsert::ModelInsert(const float *data, int sizeX, int sizeY, float max,
    float min) {
    init(data, sizeX, sizeY, max, min);
}

QVector <GLuint>& ModelInsert::getIndex() {
    return index;
}

QVector <QVector3D>& ModelInsert::getPoint() {
    return point;
}

QVector <QVector3D>& ModelInsert::getNormal() {
    return normal;
}

QVector3D ModelInsert::getSize() {
    return size;
}

void ModelInsert::init(const float *data, int sizeX, int sizeY, float max,
    float min) {
    int numPoint = sizeX * sizeY;
    int numInsert = (sizeX - 1) * (sizeY - 1);
    int numTotal = numPoint + numInsert;

    index = QVector <GLuint>(numInsert * 12);
    point = QVector <QVector3D>(numTotal);
    normal = QVector <QVector3D>(numTotal);
    float range = max - min;

    QVector <float> dataConverted(numPoint);

    for (int i = 0; i < numPoint; i++) {
        dataConverted[i] = (data[i] + min) / range;
    }

    for (int y = 0; y < sizeY - 1; y++) {
        for (int x = 0; x < sizeX - 1; x++) {
            int iUL = sizeX * y + x;
            int iLL = iUL + 1;
            int iUR = iUL + sizeX;
            int iLR = iLL + sizeX;
            int iCurrent = y * (sizeX - 1) + x + numPoint;
            float fUL = dataConverted[iUL];
            float fLL = dataConverted[iLL];
            float fUR = dataConverted[iUR];
            float fLR = dataConverted[iLR];
            float fCurrent = (fUL + fLL + fUR + fLR) / 4;
            QVector3D vUL = QVector3D(-0.5, -0.5, fUL - fCurrent);
            QVector3D vLL = QVector3D(0.5, -0.5, fLL - fCurrent);
            QVector3D vUR = QVector3D(-0.5, 0.5, fUR - fCurrent);
            QVector3D vLR = QVector3D(0.5, 0.5, fLR - fCurrent);
            point[iCurrent] = QVector3D(x + 0.5, y + 0.5, fCurrent);

            QVector3D vNormal;
            auto accumulator =
                [&vNormal](QVector3D v1, QVector3D v2) {
                    QVector3D buf = QVector3D::crossProduct(v1, v2);

                    buf.normalize();
                    vNormal += buf;
                };

            accumulator(vUL, vLL);
            accumulator(vLL, vLR);
            accumulator(vLR, vUR);
            accumulator(vUR, vUL);
            vNormal.normalize();
            normal[iCurrent] = vNormal;
        }
    }

    for (int y = 0; y < sizeY; y++) {
        for (int x = 0; x < sizeX; x++) {
            QVector3D vNormal;
            int current = y * sizeX + x;
            bool bUp = x != 0;
            bool bDown = x != sizeX - 1;
            bool bRight = y != sizeY - 1;
            bool bLeft = y != 0;
            int iUp = current - 1;
            int iDown = current + 1;
            int iRight = current + sizeX;
            int iLeft = current - sizeX;
            int iUL = numPoint + (y - 1) * (sizeX - 1) + x - 1;
            int iLL = iUL + 1;
            int iUR = iUL + sizeX - 1;
            int iLR = iUR + 1;
            float fCurrent = dataConverted[current];
            float fLeft = dataConverted[iLeft < 0 ? 0 : iLeft];
            float fRight = dataConverted[iRight >= numPoint ? 0 : iRight];
            float fDown = dataConverted[iDown >= numPoint ? 0 : iDown];
            float fUp = dataConverted[iUp < 0 ? 0 : iUp];
            float fUL = point[iUL < numTotal ? iUL : 0].z();
            float fLL = point[iLL < numTotal ? iLL : 0].z();
            float fUR = point[iUR < numTotal ? iUR : 0].z();
            float fLR = point[iLR < numTotal ? iLR : 0].z();
            QVector3D vLeft = QVector3D(0, -1, fLeft - fCurrent);
            QVector3D vRight = QVector3D(0, 1, fRight - fCurrent);
            QVector3D vDown = QVector3D(1, 0, fDown - fCurrent);
            QVector3D vUp = QVector3D(-1, 0, fUp - fCurrent);
            QVector3D vUL = QVector3D(-0.5, -0.5, fUL - fCurrent);
            QVector3D vLL = QVector3D(0.5, -0.5, fLL - fCurrent);
            QVector3D vUR = QVector3D(-0.5, 0.5, fUR - fCurrent);
            QVector3D vLR = QVector3D(0.5, 0.5, fLR - fCurrent);

            point[current] = QVector3D(x, y, dataConverted[current]);

            auto accumulator =
                [&vNormal](bool b1, bool b2, QVector3D v1, QVector3D v2,
                           QVector3D v3) {
                    if (b1 && b2) {
                        QVector3D buf = QVector3D::crossProduct(v1, v2);
                        buf.normalize();
                        vNormal += buf;
                        buf = QVector3D::crossProduct(v2, v3);
                        buf.normalize();
                        vNormal += buf;
                    }
                };

            accumulator(bLeft, bDown, vLeft, vLL, vDown);
            accumulator(bDown, bRight, vDown, vLR, vRight);
            accumulator(bRight, bUp, vRight, vUR, vUp);
            accumulator(bUp, bLeft, vUp, vUL, vLeft);
            vNormal.normalize();

            normal[current] = vNormal;
        }
    }

    int count = 0;
    for (int y = 0; y < sizeY - 1; y++) {
        for (int x = 0; x < sizeX - 1; x++) {
            GLuint ul = y * sizeX + x;
            GLuint ll = ul + 1;
            GLuint ur = ul + sizeX;
            GLuint lr = ll + sizeX;
            GLuint cn = numPoint + y * (sizeX - 1) + x;
            index[count++] = ul;
            index[count++] = ll;
            index[count++] = cn;
            index[count++] = ll;
            index[count++] = lr;
            index[count++] = cn;
            index[count++] = lr;
            index[count++] = ur;
            index[count++] = cn;
            index[count++] = ur;
            index[count++] = ul;
            index[count++] = cn;
        }
    }

    size = QVector3D(sizeX - 1, sizeY - 1, 1);
}

void Model::getExtreme(const float *data, int total, float&max, float&min) {
    max = data[0];
    min = data[0];
    for (int i = 1; i < total; i++) {
        float d = data[i];
        if (d > max) max = d;
        if (d < min) min = d;
    }
}
