#include "axis.h"
#include <QDebug>
#include <QVector3D>

using namespace std;

Axis::Axis(int sizeX, int sizeY, int sizeZ)
    : point(12 * (sizeX + sizeY + sizeZ + 1)), color(12 * (sizeX + sizeY + sizeZ + 1)),
      index((sizeX + sizeY + sizeY + 3) * 16 + 12), sizeX(sizeX), sizeY(sizeY),
      sizeZ(sizeZ), offset(2 * OFFSET) {

    int countPnt = 0;
    int countIdx = 0;
    int tmp;

    for (int j = 0; j < 2; j++) {
        float diffX = 1.0f / sizeX;
        float diffY = 1.0f / sizeY;
        float diffZ = 1.0f / sizeZ;

        int xyStart = countPnt;
        for (int i = 0; i < sizeX; i++) {
            point[countPnt] = QVector3D(diffX * i, 0, -DIST);
            color[countPnt++] = GREY;
        }
        for (int i = 0; i < sizeY; i++) {
            point[countPnt] = QVector3D(1, diffY * i, -DIST);
            color[countPnt++] = GREY;
        }
        tmp = countPnt;
        for (int i = 0; i < sizeX; i++) {
            point[countPnt] = QVector3D(1 - diffX * i, 1, -DIST);
            color[countPnt++] = GREY;
        }
        for (int i = 0; i < sizeY; i++) {
            point[countPnt] = QVector3D(0, 1 - diffY * i, -DIST);
            color[countPnt++] = GREY;
        }
        color[tmp] = BLACK;

        int markXStrt = countPnt;
        for (int i = 0; i < sizeX + 1; i++) {
            point[countPnt] = QVector3D(diffX * i, 1 + EXTRA, -DIST);
            color[countPnt++] = BLACK;
        }
        int markXRight = countPnt;
        for (int i = 0; i < sizeX + 1; i++) {
            point[countPnt] = QVector3D(diffX * i, 1, -DIST - EXTRA);
            color[countPnt++] = BLACK;
        }
        int markYStrt = countPnt;
        for (int i = 0; i < sizeY + 1; i++) {
            point[countPnt] = QVector3D(1 + EXTRA, diffY * i, -DIST);
            color[countPnt++] = BLACK;
        }
        int markYRight = countPnt;
        for (int i = 0; i < sizeY + 1; i++) {
            point[countPnt] = QVector3D(1, diffY * i, -DIST - EXTRA);
            color[countPnt++] = BLACK;
        }

        int xzStart = countPnt;
        for (int i = 0; i < sizeX; i++) {
            point[countPnt] = QVector3D(diffX * i, -DIST, 0);
            color[countPnt++] = GREY;
        }
        for (int i = 0; i < sizeZ; i++) {
            point[countPnt] = QVector3D(1, -DIST, diffZ * i);
            color[countPnt++] = GREY;
        }
        tmp = countPnt;
        for (int i = 0; i < sizeX; i++) {
            point[countPnt] = QVector3D(1 - diffX * i, -DIST, 1);
            color[countPnt++] = GREY;
        }
        for (int i = 0; i < sizeZ; i++) {
            point[countPnt] = QVector3D(0, -DIST, 1 - diffZ * i);
            color[countPnt++] = GREY;
        }
        color[tmp] = BLACK;

        int markZStrt = countPnt;
        for (int i = 0; i < sizeZ + 1; i++) {
            point[countPnt] = QVector3D(1 + EXTRA, -DIST, diffZ * i);
            color[countPnt++] = BLACK;
        }
        int markZRight = countPnt;
        for (int i = 0; i < sizeZ + 1; i++) {
            point[countPnt] = QVector3D(1, -DIST - EXTRA, diffZ * i);
            color[countPnt++] = BLACK;
        }

        int yzStart = countPnt;
        for (int i = 0; i < sizeY; i++) {
            point[countPnt] = QVector3D(-DIST, diffY * i, 0);
            color[countPnt++] = GREY;
        }
        for (int i = 0; i < sizeZ; i++) {
            point[countPnt] = QVector3D(-DIST, 1, diffZ * i);
            color[countPnt++] = GREY;
        }
        for (int i = 0; i < sizeY; i++) {
            point[countPnt] = QVector3D(-DIST, 1 - diffY * i, 1);
            color[countPnt++] = GREY;
        }
        for (int i = 0; i < sizeZ; i++) {
            point[countPnt] = QVector3D(-DIST, 0, 1 - diffZ * i);
            color[countPnt++] = GREY;
        }

        offset[XY_X_B + j * OFFSET] = countIdx;
        index[countIdx++] = xyStart + 2 * sizeX + sizeY;
        index[countIdx++] = xyStart + sizeX + sizeY;
        offset[XY_X_G + j * OFFSET] = countIdx;
        tmp = countIdx;
        for (int i = 0; i < sizeY + 1; i++) {
            index[countIdx++] = xyStart + sizeX + i;
            index[countIdx++] = xyStart + 2 * sizeX + 2 * sizeY - i;
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

        offset[XZ + j * OFFSET] = countIdx;
        index[countIdx++] = xzStart + sizeX;
        index[countIdx++] = xzStart + sizeX + sizeZ;
        for (int i = 0; i < sizeX; i++) {
            index[countIdx++] = xzStart + 2 * sizeX + sizeZ - i;
            index[countIdx++] = xzStart + i;
        }
        tmp = countIdx;
        for (int i = 0; i < sizeZ + 1; i++) {
            index[countIdx++] = xzStart + sizeX + i;
            index[countIdx++] = xzStart + 2 * sizeX + 2 * sizeZ - i;
        }
        index[tmp + 1] = xzStart;

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

        offset[YZ + j * OFFSET] = countIdx;
        tmp = countIdx;
        for (int i = 0; i < sizeZ + 1; i++) {
            index[countIdx++] = yzStart + sizeY + i;
            index[countIdx++] = yzStart + 2 * sizeY + 2 * sizeZ - i;
        }
        index[tmp + 1] = yzStart;
        for (int i = 0; i < sizeY + 1; i++) {
            index[countIdx++] = yzStart + 2 * sizeY + sizeZ - i;
            index[countIdx++] = yzStart + i;
        }

        tmp = sizeX;
        sizeX = sizeY;
        sizeY = tmp;
    }
}

const vector<QVector3D> &Axis::getPoint() const { return point; }
const vector<QVector3D> &Axis::getColor() const { return color; }
const vector<GLuint> &Axis::getIndex() const { return index; }

vector<QPair<int, int>> Axis::getSlice(int rotX, int rotY) {
    int rx = (360 - rotX) % 90;
    int dir = (360 - rotX) / 45 % 8;
    return getSlice(rx > 20 || rotY > 20, rx < 70 || rotY > 20, rotY<70, rx> 45,
        rotY > 20, dir % 4 > 1);
}

vector<QPair<int, int>> Axis::getSlice(
    bool xEnable, bool yEnable, bool zEnable, bool zStrait, bool xyStrait, bool invert) {
    typedef QPair<int, int> P;
    vector<QPair<int, int>> ret;
    int i = invert ? OFFSET : 0;
    int x = invert ? sizeY : sizeX;
    int y = invert ? sizeX : sizeY;
    ret.push_back(P(offset[(xEnable ? XY_X_B : XY_X_G) + i], y + 1));
    ret.push_back(P(offset[(yEnable ? XY_Y_B : XY_Y_G) + i], x + 1));
    if (xEnable) ret.push_back(P(offset[(xyStrait ? X_S : X_R) + i], x + 1));
    if (yEnable) ret.push_back(P(offset[(xyStrait ? Y_S : Y_R) + i], y + 1));
    if (zEnable) ret.push_back(P(offset[XZ + i], x + sizeZ + 2));
    if (zEnable) ret.push_back(P(offset[(zStrait ? Z_S : Z_R) + i], sizeZ + 1));
    if (zEnable) ret.push_back(P(offset[YZ + i], sizeZ + y + 2));
    return ret;
}

const vector<QPair<EnumPosition, vector<QVector3D>>> &Axis::getNumber(
    int rotX, int rotY) const {
    int rx = (360 - rotX) % 90;
    int dir = (360 - rotX) / 45 % 8;
    return getNumber(dir, rx > 20 || rotY > 20, rx < 70 || rotY > 20, rotY<70, rx> 45,
        rotY > 20, rx == 0);
}

// position, base line dir, expand dir
const vector<PositionInfo> &Axis::getLabel(int dir, bool xEnable, bool yEnable,
    bool zEnable, bool zStrait, bool xyStrait, bool front) const {
    static vector<PositionInfo> ret(3);
    float xy1 = xyStrait ? 1 + 2 * EXTRA : 1;
    float xy2 = xyStrait ? -DIST : -DIST - 2 * EXTRA;
    float z1 = zStrait ? 1 + 2 * EXTRA : 1;
    float z2 = zStrait ? -DIST : -DIST - 2 * EXTRA;
    int i1 = dir % 4 > 1 ? 1 : 0;
    int i2 = dir % 4 > 1 ? 0 : 1;

    if (zEnable) {
        ret[i1] = {xEnable ? (front ? PARALLEL : CENTER) : DISABLED, {0.5, xy1, xy2},
            {1, 0, 0}, xyStrait ? QVector3D{0, 1, 0} : QVector3D{0, 1, -1}};
        ret[i2] = {yEnable ? (front ? PARALLEL : CENTER) : DISABLED, {xy1, 0.5, xy2},
            {0, 1, 0}, xyStrait ? QVector3D{1, 0, 0} : QVector3D{1, 0, -1}};
        ret[2] = {zEnable ? PARALLEL : DISABLED, {z1, z2, 0.5}, {0, 0, 1},
            zStrait ? QVector3D{1, 0, 0} : QVector3D{0, -1, 0}};
    } else {
        ret[i1] = {xEnable ? PARALLEL : DISABLED, {0.5, xy1, xy2}, {1, 0, 0},
            xyStrait ? QVector3D{0, 1, 0} : QVector3D{0, 1, -1}};
        ret[i2] = {yEnable ? PARALLEL : DISABLED, {xy1, 0.5, xy2}, {0, 1, 0},
            xyStrait ? QVector3D{1, 0, 0} : QVector3D{1, 0, -1}};
        ret[2] = {DISABLED, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
    }

    return ret;
}

const vector<PositionInfo> &Axis::getLabel(int rotX, int rotY) const {
    int rx = (360 - rotX) % 90;
    int dir = (360 - rotX) / 45 % 8;
    return getLabel(dir, rx > 20 || rotY > 20, rx < 70 || rotY > 20, rotY<70, rx> 45,
        rotY > 20, rx == 0);
}

bool Axis::getDir(int rotX, int rotY) const {
    (void)rotX;
    return rotY < 70;
}

QMatrix4x4 Axis::getTransform(int rotX, int rotY) const {
    return getTransform((360 - rotX) / 45 % 8, rotY >= 70);
}

QMatrix4x4 Axis::getTransform(int dir, bool flipX) const {
    QMatrix4x4 ret, tmp;
    ret.rotate(90 * (dir / 2), 0, 0, 1);
    if (flipX) tmp.data()[5] = -1;
    return ret * tmp;
}

const vector<QPair<EnumPosition, vector<QVector3D>>> &Axis::getNumber(int dir,
    bool xEnable, bool yEnable, bool zEnable, bool zStrait, bool xyStrait,
    bool front) const {
    static vector<QPair<EnumPosition, vector<QVector3D>>> ret(3); // true for right align
    for (int i = 0; i < 3; i++) ret[i].second.clear();
    float diffX = 1.0f / sizeX;
    float diffY = 1.0f / sizeY;
    float diffZ = 1.0f / sizeZ;
    float xyEx = xyStrait ? 2 * EXTRA : 0;
    float zEx = xyStrait ? 0 : -2 * EXTRA;
    float xEx = zStrait ? 2 * EXTRA : 0;
    float yEx = -DIST + (zStrait ? 0 : -2 * EXTRA);

    int d = dir / 2;
    if (d == 0) {
        if (xEnable)
            for (int i = 0; i < sizeX + 1; i++)
                ret[0].second.push_back(QVector3D(diffX * i, 1 + xyEx, -DIST + zEx));
        if (yEnable) {
            if (zEnable)
                for (int i = 0; i < sizeY + 1; i++)
                    ret[1].second.push_back(QVector3D(1 + xyEx, diffY * i, -DIST + zEx));
            else
                for (int i = 0; i < sizeY + 1; i++)
                    ret[1].second.push_back(
                        QVector3D(1 + xyEx, 1 - diffY * i, -DIST + zEx));
        }
        ret[0].first = zEnable ? RIGHT : LEFT;
        ret[1].first = !front && xEnable ? LEFT : CENTER;
    } else if (d == 1) {
        if (xEnable)
            for (int i = 0; i < sizeY + 1; i++)
                ret[1].second.push_back(QVector3D(diffY * i, 1 + xyEx, -DIST + zEx));
        if (yEnable) {
            if (zEnable)
                for (int i = 0; i < sizeX + 1; i++)
                    ret[0].second.push_back(
                        QVector3D(1 + xyEx, 1 - diffX * i, -DIST + zEx));
            else
                for (int i = 0; i < sizeX + 1; i++)
                    ret[0].second.push_back(QVector3D(1 + xyEx, diffX * i, -DIST + zEx));
        }
        ret[0].first = !front && xEnable ? LEFT : CENTER;
        ret[1].first = zEnable ? RIGHT : LEFT;
    } else if (d == 2) {
        if (xEnable)
            for (int i = 0; i < sizeX + 1; i++)
                ret[0].second.push_back(QVector3D(1 - diffX * i, 1 + xyEx, -DIST + zEx));
        if (yEnable) {
            if (zEnable)
                for (int i = 0; i < sizeY + 1; i++)
                    ret[1].second.push_back(
                        QVector3D(1 + xyEx, 1 - diffY * i, -DIST + zEx));
            else
                for (int i = 0; i < sizeY + 1; i++)
                    ret[1].second.push_back(QVector3D(1 + xyEx, diffY * i, -DIST + zEx));
        }
        ret[0].first = zEnable ? RIGHT : LEFT;
        ret[1].first = !front && xEnable ? LEFT : CENTER;
    } else {
        if (xEnable)
            for (int i = 0; i < sizeY + 1; i++)
                ret[1].second.push_back(QVector3D(1 - diffY * i, 1 + xyEx, -DIST + zEx));
        if (yEnable) {
            if (zEnable)
                for (int i = 0; i < sizeX + 1; i++)
                    ret[0].second.push_back(QVector3D(1 + xyEx, diffX * i, -DIST + zEx));
            else
                for (int i = 0; i < sizeX + 1; i++)
                    ret[0].second.push_back(
                        QVector3D(1 + xyEx, 1 - diffX * i, -DIST + zEx));
        }
        ret[0].first = !front && xEnable ? LEFT : CENTER;
        ret[1].first = zEnable ? RIGHT : LEFT;
    }
    if (zEnable)
        for (int i = 0; i < sizeZ + 1; i++)
            ret[2].second.push_back(QVector3D(1 + xEx, yEx, diffZ * i));
    ret[2].first = LEFT;

    return ret;
}
