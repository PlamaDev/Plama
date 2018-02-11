#include "axis.h"

#include<QVector>
#include<QVector3D>
#include<QDebug>

const float Axis::dist = 0.1f;
const float Axis::extra = 0.05f;
const QVector3D Axis::black = QVector3D(0.4, 0.4, 0.4);
const QVector3D Axis::grey = QVector3D(0.8, 0.8, 0.8);

Axis::Axis(int sizeX, int sizeY, int sizeZ)
    : sizeX(sizeX), sizeY(sizeY), sizeZ(sizeZ), offset(2 * OFFSET) {
    point = QVector<QVector3D>(12 * (sizeX + sizeY + sizeZ + 1));
    color = QVector<QVector3D>(12 * (sizeX + sizeY + sizeZ + 1));
    index = QVector<GLuint>((sizeX + sizeY + sizeY + 3) * 16 + 12);

    int countPnt = 0;
    int countIdx = 0;
    int tmp;

    for (int j = 0; j < 2; j++) {
        float diffX = 1.0f / sizeX;
        float diffY = 1.0f / sizeY;
        float diffZ = 1.0f / sizeZ;

        int xyStart = countPnt;
        for (int i = 0; i < sizeX; i++) {
            point[countPnt] = QVector3D(diffX * i, 0, -dist);
            color[countPnt++] = grey;
        }
        for (int i = 0; i < sizeY; i++) {
            point[countPnt] = QVector3D(1, diffY * i, -dist);
            color[countPnt++] = grey;
        }
        tmp = countPnt;
        for (int i = 0; i < sizeX; i++) {
            point[countPnt] = QVector3D(1 - diffX * i, 1, -dist);
            color[countPnt++] = grey;
        }
        for (int i = 0; i < sizeY; i++) {
            point[countPnt] = QVector3D(0, 1 - diffY * i, -dist);
            color[countPnt++] = grey;
        }
        color[tmp] = black;

        int markXStrt = countPnt;
        for (int i = 0; i < sizeX + 1; i++) {
            point[countPnt] = QVector3D(diffX * i, 1 + extra, -dist);
            color[countPnt++] = black;
        }
        int markXRight = countPnt;
        for (int i = 0; i < sizeX + 1; i++) {
            point[countPnt] = QVector3D(diffX * i, 1, -dist - extra);
            color[countPnt++] = black;
        }
        int markYStrt = countPnt;
        for (int i = 0; i < sizeY + 1; i++) {
            point[countPnt] = QVector3D(1 + extra, diffY * i, -dist);
            color[countPnt++] = black;
        }
        int markYRight = countPnt;
        for (int i = 0; i < sizeY + 1; i++) {
            point[countPnt] = QVector3D(1, diffY * i, -dist - extra);
            color[countPnt++] = black;
        }

        int xzStart = countPnt;
        for (int i = 0; i < sizeX; i++) {
            point[countPnt] = QVector3D(diffX * i, -dist, 0);
            color[countPnt++] = grey;
        }
        for (int i = 0; i < sizeZ; i++) {
            point[countPnt] = QVector3D(1, -dist, diffZ * i);
            color[countPnt++] = grey;
        }
        tmp = countPnt;
        for (int i = 0; i < sizeX; i++) {
            point[countPnt] = QVector3D(1 - diffX * i, -dist, 1);
            color[countPnt++] = grey;
        }
        for (int i = 0; i < sizeZ; i++) {
            point[countPnt] = QVector3D(0, -dist, 1 - diffZ * i);
            color[countPnt++] = grey;
        }
        color[tmp] = black;

        int markZStrt = countPnt;
        for (int i = 0; i < sizeZ + 1; i++) {
            point[countPnt] = QVector3D(1 + extra, -dist, diffZ * i);
            color[countPnt++] = black;
        }
        int markZRight = countPnt;
        for (int i = 0; i < sizeZ + 1; i++) {
            point[countPnt] = QVector3D(1, -dist - extra, diffZ * i);
            color[countPnt++] = black;
        }

        int yzStart = countPnt;
        for (int i = 0; i < sizeY; i++) {
            point[countPnt] = QVector3D(-dist, diffY * i, 0);
            color[countPnt++] = grey;
        }
        for (int i = 0; i < sizeZ; i++) {
            point[countPnt] = QVector3D(-dist, 1, diffZ * i);
            color[countPnt++] = grey;
        }
        for (int i = 0; i < sizeY; i++) {
            point[countPnt] = QVector3D(-dist, 1 - diffY * i, 1);
            color[countPnt++] = grey;
        }
        for (int i = 0; i < sizeZ; i++) {
            point[countPnt] = QVector3D(-dist, 0, 1 - diffZ * i);
            color[countPnt++] = grey;
        }

        offset[XY_X_B + j * OFFSET] = countIdx;
        index[countIdx++] = xyStart + 2 * sizeX + sizeY;
        index[countIdx++] = xyStart + sizeX + sizeY;
        offset[XY_X_G + j * OFFSET] = countIdx;
        tmp = countIdx;
        for (int i = 0; i < sizeY + 1; i++) {
            index[countIdx++] = xyStart + sizeX + i;
            index[countIdx++] = xyStart + 2 * sizeX  + 2 * sizeY - i;
        }
        index[tmp + 1] = xyStart;
        offset[XY_Y_B + j * OFFSET] = countIdx;
        index[countIdx++] = xyStart + sizeX;
        index[countIdx++] = xyStart + sizeX + sizeY;
        offset[XY_Y_G + j * OFFSET] = countIdx;
        for (int i = 0; i < sizeX + 1; i++) {
            index[countIdx++] = xyStart + 2 * sizeX + sizeY - i;
            index[countIdx++] = xyStart + i;
        }

        offset[X_S + j * OFFSET] = countIdx;
        for (int i = 0; i < sizeX + 1; i++) {
            index[countIdx++] = xyStart + 2 * sizeX + sizeY - i;
            index[countIdx++] = markXStrt + i;
        }
        offset[X_R + j * OFFSET] = countIdx;
        for (int i = 0; i < sizeX + 1; i++) {
            index[countIdx++] = xyStart + 2 * sizeX + sizeY - i;
            index[countIdx++] = markXRight + i;
        }
        offset[Y_S + j * OFFSET] = countIdx;
        for (int i = 0; i < sizeY + 1; i++) {
            index[countIdx++] = xyStart + sizeX + i;
            index[countIdx++] = markYStrt + i;
        }
        offset[Y_R + j * OFFSET] = countIdx;
        for (int i = 0; i < sizeY + 1; i++) {
            index[countIdx++] = xyStart + sizeX + i;
            index[countIdx++] = markYRight + i;
        }

        offset[XZ_X + j * OFFSET] = countIdx;
        tmp = countIdx;
        for (int i = 0; i < sizeZ + 1; i++) {
            index[countIdx++] = xzStart + sizeX + i;
            index[countIdx++] = xzStart + 2 * sizeX + 2 * sizeZ - i;
        }
        index[tmp + 1] = xzStart;
        offset[XZ_Z_B + j * OFFSET] = countIdx;
        index[countIdx++] = xzStart + sizeX;
        index[countIdx++] = xzStart + sizeX + sizeZ;
        offset[XZ_Z_G + j * OFFSET] = countIdx;
        for (int i = 0; i < sizeX + 1; i++) {
            index[countIdx++] = xzStart + 2 * sizeX + sizeZ - i;
            index[countIdx++] = xzStart + i;
        }

        offset[Z_S + j * OFFSET] = countIdx;
        for (int i = 0; i < sizeZ + 1; i++) {
            index[countIdx++] = xzStart + sizeX + i;
            index[countIdx++] = markZStrt + i;
        }
        offset[Z_R + j * OFFSET] = countIdx;
        for (int i = 0; i < sizeZ + 1; i++) {
            index[countIdx++] = xzStart + sizeX + i;
            index[countIdx++] = markZRight + i;
        }

        offset[YZ_Y + j * OFFSET] = countIdx;
        tmp = countIdx;
        for (int i = 0; i < sizeZ + 1; i++) {
            index[countIdx++] = yzStart + sizeY + i;
            index[countIdx++] = yzStart + 2 * sizeY + 2 * sizeZ - i;
        }
        index[tmp + 1] = yzStart;
        offset[YZ_Z + j * OFFSET] = countIdx;
        for (int i = 0; i < sizeY + 1; i++) {
            index[countIdx++] = yzStart + 2 * sizeY + sizeZ - i;
            index[countIdx++] = yzStart + i;
        }

        tmp = sizeX;
        sizeX = sizeY;
        sizeY = tmp;
    }
}

QVector<QVector3D> &Axis::getPoint() { return point; }
QVector<QVector3D> &Axis::getColor() { return color; }
QVector<GLuint> &Axis::getIndex() { return index; }

QVector<QPair<int, int> > Axis::getSlice(int rotX, int rotY) {
    int rx = (360 - rotX) % 90;
    int dir = (360 - rotX) / 45 % 8;
    return getSlice(rx > 10 || rotY > 20, rx < 80
            || rotY > 20, rotY < 70, rx > 45, rotY > 20, dir % 4 > 1);
}

QVector<QPair<int, int> > Axis::getSlice(
    bool xEnable, bool yEnable, bool zEnable, bool zStrait, bool xyStrait,
    bool invert) {
    typedef QPair<int, int> P;
    QVector<QPair<int, int>> ret;
    int i = invert ? OFFSET : 0;
    int x = invert ? sizeY : sizeX;
    int y = invert ? sizeX : sizeY;
    ret.append(P(offset[(xEnable ? XY_X_B : XY_X_G) + i], y + 1));
    ret.append(P(offset[(yEnable ? XY_Y_B : XY_Y_G) + i], x + 1));
    if (xEnable) ret.append(P(offset[(xyStrait ? X_S : X_R) + i], x + 1));
    if (yEnable) ret.append(P(offset[(xyStrait ? Y_S : Y_R) + i], y + 1));
    if (zEnable) ret.append(P(offset[XZ_X + i], x + sizeZ + 2));
    if (zEnable) ret.append(P(offset[(zStrait ? Z_S : Z_R) + i], sizeZ + 1));
    if (zEnable) ret.append(P(offset[YZ_Y + i], sizeZ + y + 2));
    return ret;
}

QVector<QPair<bool, QVector<QVector3D>>> &Axis::getNumber(int rotX, int rotY) {
    int rx = (360 - rotX) % 90;
    int dir = (360 - rotX) / 45 % 8;
    return getNumber(dir, rx > 10 || rotY > 20, rx < 80 || rotY > 20,
            rotY < 70, rx > 45, rotY > 20);
}

QMatrix4x4 Axis::getTransform(int rotX, int rotY) {
    return getTransform((360-rotX) / 45 % 8, rotY >= 70);
}

QMatrix4x4 Axis::getTransform(int dir, bool flipX) {
    QMatrix4x4 ret, tmp;
    ret.rotate(90 * (dir / 2), 0, 0, 1);
    if (flipX) tmp.data()[5] = -1;
    return  ret * tmp;
}

QVector<QPair<bool, QVector<QVector3D>>> &Axis::getNumber(int dir,
    bool xEnable, bool yEnable, bool zEnable, bool zStrait, bool xyStrait) {
    static QVector<QPair<bool, QVector<QVector3D>>> ret(3);  // true for right align
    for (int i = 0; i < 3; i++) ret[i].second.clear();
    float diffX = 1.0f / sizeX;
    float diffY = 1.0f / sizeY;
    float diffZ = 1.0f / sizeZ;
    float xyEx = xyStrait ? 2 * extra : 0;
    float zEx = xyStrait ? 0 : -2 * extra;
    float xEx = zStrait ? 2 * extra : 0;
    float yEx = -dist + (zStrait ? 0 : -2 * extra);

    int d = dir/2;
    if (d == 0) {
        if (xEnable) for (int i = 0; i < sizeX + 1; i++)
                ret[0].second.append(QVector3D(diffX * i, 1 + xyEx, -dist + zEx));
        if (yEnable) {
            if (zEnable) for (int i = 0; i < sizeY + 1; i++)
                    ret[1].second.append(QVector3D(1 + xyEx, diffY * i, -dist + zEx));
            else for (int i = 0; i < sizeY + 1; i++)
                    ret[1].second.append(QVector3D(1 + xyEx, 1 - diffY * i, -dist + zEx));
        }
        ret[0].first = !zEnable;
        ret[1].first = true;
    } else if (d == 1) {
        if (xEnable) for (int i = 0; i < sizeY + 1; i++)
                ret[1].second.append(QVector3D(diffY * i, 1 + xyEx, -dist + zEx));
        if (yEnable) {
            if (zEnable) for (int i = 0; i < sizeX + 1; i++)
                    ret[0].second.append(QVector3D(1 + xyEx, 1 - diffX * i, -dist + zEx));
            else for (int i = 0; i < sizeX + 1; i++)
                    ret[0].second.append(QVector3D(1 + xyEx, diffX * i, -dist + zEx));
        }
        ret[0].first = true;
        ret[1].first = !zEnable;
    } else if (d == 2) {
        if (xEnable) for (int i = 0; i < sizeX + 1; i++)
                ret[0].second.append(QVector3D(1 - diffX * i, 1 + xyEx, -dist + zEx));
        if (yEnable) {
            if (zEnable) for (int i = 0; i < sizeY + 1; i++)
                    ret[1].second.append(QVector3D(1 + xyEx, 1 - diffY * i, -dist + zEx));
            else for (int i = 0; i < sizeY + 1; i++)
                    ret[1].second.append(QVector3D(1 + xyEx, diffY * i, -dist + zEx));
        }
        ret[0].first = !zEnable;
        ret[1].first = true;
    } else {
        if (xEnable) for (int i = 0; i < sizeY + 1; i++)
                ret[1].second.append(QVector3D(1 - diffY * i, 1 + xyEx, -dist + zEx));
        if (yEnable) {
            if (zEnable) for (int i = 0; i < sizeX + 1; i++)
                    ret[0].second.append(QVector3D(1 + xyEx, diffX * i, -dist + zEx));
            else for (int i = 0; i < sizeX + 1; i++)
                    ret[0].second.append(QVector3D(1 + xyEx, 1 - diffX * i, -dist + zEx));
        }
        ret[0].first = true;
        ret[1].first = !zEnable;
    }
    if (zEnable) for (int i = 0; i < sizeZ + 1; i++)
            ret[2].second.append(QVector3D(1 + xEx, yEx, diffZ * i));
    ret[2].first = true;

    return ret;
}
