#include <QColor>
#include <QDebug>
#include <QPair>
#include <render/model.h>

using namespace std;

function<void(function<void(int)> &, int, int)> f0 = //
    [](function<void(int)> &f, int x, int y) {
        for (int i = 0; i < x; i++)
            for (int j = 0; j < y; j++) f(j * x + i);
    };

function<void(function<void(int)> &, int, int)> f1 = //
    [](function<void(int)> &f, int x, int y) {
        for (int i = x - 1; i >= 0; i--)
            for (int j = 0; j < y; j++) f(j * x + i);
    };

function<void(function<void(int)> &, int, int)> f2 = //
    [](function<void(int)> &f, int x, int y) {
        for (int i = 0; i < x; i++)
            for (int j = y - 1; j >= 0; j--) f(j * x + i);
    };

function<void(function<void(int)> &, int, int)> f3 = //
    [](function<void(int)> &f, int x, int y) {
        for (int i = x - 1; i >= 0; i--)
            for (int j = y - 1; j >= 0; j--) f(j * x + i);
    };

vector<function<void(function<void(int)> &, int, int)>> Model::indexFunc{
    f0, f0, f1, f1, f3, f3, f2, f2};

const vector<GLuint> &Model::getIndexT(int dir) const { return indexT[dir]; }
const vector<GLuint> &Model::getIndexL(int dir) const { return indexL[dir]; }
const vector<QVector3D> &Model::getPoint() const { return point; }
const vector<QVector3D> &Model::getNormal() const { return normal; }
const vector<QVector3D> &Model::getColorF() const { return colorF; }
const vector<QVector3D> &Model::getPosition() const { return position; }
const vector<const QColor *> &Model::getColorQ() const { return colorQ; }

void Model::changeData(
    const std::vector<float> &data, int sizeX, int sizeY, QVector2D extreme) {
    static vector<vector<GLuint>> order{
        {0, 4, 3, 1, 4, 0, 3, 4, 6, 6, 4, 7, 2, 4, 1, 5, 4, 2, 7, 4, 8, 8, 4, 5},
        {1, 4, 0, 0, 4, 3, 3, 4, 6, 6, 4, 7, 2, 4, 1, 5, 4, 2, 8, 4, 5, 7, 4, 8},
        {2, 4, 1, 5, 4, 2, 1, 4, 0, 0, 4, 3, 8, 4, 5, 7, 4, 8, 3, 4, 6, 6, 4, 7},
        {5, 4, 2, 2, 4, 1, 1, 4, 0, 0, 4, 3, 8, 4, 5, 7, 4, 8, 6, 4, 7, 3, 4, 6},
        {8, 4, 5, 7, 4, 8, 5, 4, 2, 2, 4, 1, 6, 4, 7, 3, 4, 6, 1, 4, 0, 0, 4, 3},
        {7, 4, 8, 8, 4, 5, 5, 4, 2, 2, 4, 1, 6, 4, 7, 3, 4, 6, 0, 4, 3, 1, 4, 0},
        {6, 4, 7, 3, 4, 6, 7, 4, 8, 8, 4, 5, 0, 4, 3, 1, 4, 0, 5, 4, 2, 2, 4, 1},
        {3, 4, 6, 6, 4, 7, 7, 4, 8, 8, 4, 5, 0, 4, 3, 1, 4, 0, 2, 4, 1, 5, 4, 2}};

    float sx = sizeX - 1;
    float sy = sizeY - 1;
    float height = extreme.y() - extreme.x();
    for (int y = 0; y < sizeY - 1; y++) {
        for (int x = 0; x < sizeX - 1; x++) {
            float d[9];
            float px[3];
            float py[3];
            QVector3D p[9];

            int offsetRaw = y * sizeX + x;
            int offsetPoint = (y * (sizeX - 1) + x) * 9;

            if (height == 0) {
                for (int i = 0; i < 9; i++) d[i] = data[offsetPoint];
            } else {
                d[0] = data[offsetRaw];
                d[2] = data[offsetRaw + 1];
                d[6] = data[offsetRaw + sizeX];
                d[8] = data[offsetRaw + sizeX + 1];
                d[0] = (d[0] - extreme.x()) / height;
                d[2] = (d[2] - extreme.x()) / height;
                d[6] = (d[6] - extreme.x()) / height;
                d[8] = (d[8] - extreme.x()) / height;
                d[1] = (d[0] + d[2]) / 2;
                d[3] = (d[0] + d[6]) / 2;
                d[5] = (d[2] + d[8]) / 2;
                d[7] = (d[6] + d[8]) / 2;
                d[4] = (d[3] + d[5]) / 2;
            }

            for (int i = 0; i < 3; i++) {
                px[i] = (x + 0.5f * i) / sx;
                py[i] = (y + 0.5f * i) / sy;
            }

            for (int yy = 0; yy < 3; yy++) {
                for (int xx = 0; xx < 3; xx++) {
                    int i = yy * 3 + xx;
                    p[i] = QVector3D(px[xx], py[yy], d[i]);
                }
            }

            for (int i = 0; i < 9; i++) point[offsetPoint + i] = p[i];

            normal[offsetPoint] = QVector3D::crossProduct(p[1] - p[0], p[4] - p[0]);
            normal[offsetPoint + 1] = QVector3D::crossProduct(p[2] - p[1], p[4] - p[1]);
            normal[offsetPoint + 2] = QVector3D::crossProduct(p[5] - p[2], p[4] - p[2]);
            normal[offsetPoint + 3] = QVector3D::crossProduct(p[0] - p[3], p[4] - p[3]);
            normal[offsetPoint + 4] = QVector3D(1, 1, 1);
            normal[offsetPoint + 5] = QVector3D::crossProduct(p[8] - p[5], p[4] - p[5]);
            normal[offsetPoint + 6] = QVector3D::crossProduct(p[3] - p[6], p[4] - p[6]);
            normal[offsetPoint + 7] = QVector3D::crossProduct(p[6] - p[7], p[4] - p[7]);
            normal[offsetPoint + 8] = QVector3D::crossProduct(p[7] - p[8], p[4] - p[8]);

            for (int i = 0; i < 9; i++) normal[offsetPoint + i].normalize();

            colorQ[offsetPoint] = &(Gradient::HEIGHT_MAP.getColorQ((d[0] + d[4]) / 2));
            colorQ[offsetPoint + 3] = colorQ[offsetPoint];
            colorQ[offsetPoint + 1] = &Gradient::HEIGHT_MAP.getColorQ((d[2] + d[4]) / 2);
            colorQ[offsetPoint + 2] = colorQ[offsetPoint + 1];
            colorQ[offsetPoint + 5] = &Gradient::HEIGHT_MAP.getColorQ((d[8] + d[4]) / 2);
            colorQ[offsetPoint + 8] = colorQ[offsetPoint + 5];
            colorQ[offsetPoint + 7] = &Gradient::HEIGHT_MAP.getColorQ((d[6] + d[4]) / 2);
            colorQ[offsetPoint + 6] = colorQ[offsetPoint + 7];
            colorQ[offsetPoint + 4] = &Gradient::HEIGHT_MAP.getColorQ(0);

            for (int i = offsetPoint; i < offsetPoint + 9; i++) {
                const QColor *c = colorQ[i];
                colorF[i] = QVector3D(c->redF(), c->greenF(), c->blueF());
            }

            position[offsetPoint] = (p[0] + p[1] + p[4]) / 3;
            position[offsetPoint + 1] = (p[1] + p[2] + p[4]) / 3;
            position[offsetPoint + 2] = (p[2] + p[5] + p[4]) / 3;
            position[offsetPoint + 3] = (p[0] + p[3] + p[4]) / 3;
            position[offsetPoint + 4] = p[4];
            position[offsetPoint + 5] = (p[5] + p[8] + p[4]) / 3;
            position[offsetPoint + 6] = (p[6] + p[3] + p[4]) / 3;
            position[offsetPoint + 7] = (p[6] + p[7] + p[4]) / 3;
            position[offsetPoint + 8] = (p[7] + p[8] + p[4]) / 3;
        }
    }

    for (int i = 0; i < 8; i++) {
        int cnt = 0;
        function<void(int)> func = [&](int j) {
            int offsetPoint = j * 9;
            for (int j = 0; j < 24; j++) indexT[i][cnt++] = order[i][j] + offsetPoint;
        };
        indexFunc[i](func, sizeX - 1, sizeY - 1);
    }
}

unique_ptr<Model> Model::fromQuantity(SimQuantity &sq, float time, int dim) {
    return fromData(sq.getDataAt(time, dim), sq.getSizeData()[0], sq.getSizeData()[1],
        sq.getExtreme());
}

std::unique_ptr<Model> Model::fromData(
    const std::vector<float> &data, int sizeX, int sizeY, QVector2D extreme) {
    int sizePoint = (sizeX - 1) * (sizeY - 1) * 9;
    int sizeIndex = (sizeX - 1) * (sizeY - 1) * 24;

    unique_ptr<Model> ret = unique_ptr<Model>(new Model(sizePoint, sizeIndex, 0));
    ret->changeData(data, sizeX, sizeY, extreme);
    return ret;
}

unique_ptr<Model> Model::fromQuantity(SimQuantity &sq, int dim) {
    static QColor c = Qt::blue;

    vector<float> y(sq.getData().size());
    for (size_t i = 0; i < sq.getData().size(); i++)
        y[i] = sq.getData()[sq.getDim() * i + dim][0];
    const vector<float> &x = sq.getTimes();

    unique_ptr<Model> ret = unique_ptr<Model>(new Model(x.size(), 0, (x.size() - 1) * 2));

    float xMin = x[0];
    float xMax = x[x.size() - 1];
    float xDiff = xMax - xMin;
    float yMin = sq.getMin();
    float yMax = sq.getMax();
    float yDiff = yMax - yMin;

    if (yDiff == 0) {
        if (yMin == 0) {
            yMin = -1;
            yMax = 1;
            yDiff = 2;
        } else {
            yMin *= 0.9;
            yMax *= 1.1;
            yDiff = yMax - yMin;
        }
    }

    for (size_t i = 0; i < x.size(); i++) {
        ret->point[i] = QVector3D((x[i] - xMin) / xDiff, (y[i] - yMin) / yDiff, 0);
        ret->normal[i] = QVector3D(0, 0, 1);
        ret->colorQ[i] = &c;
        ret->colorF[i] = QVector3D(0, 0, 1);
        ret->position[i] = ret->point[i];
    }

    for (int j = 0; j < 8; j++) {
        for (size_t i = 0; i < x.size() - 1; i++) {
            ret->indexL[j][2 * i] = i;
            ret->indexL[j][2 * i + 1] = i + 1;
        }
    }

    return ret;
}

Model::Model(int sPoint, int sIndexT, int sIndexL)
    : point(sPoint), normal(sPoint), colorF(sPoint), colorQ(sPoint), position(sPoint),
      indexT(8, vector<GLuint>(sIndexT)), indexL(8, vector<GLuint>(sIndexL)) {}

QPair<float, float> Model::getExtreme(const float *data, int total) {
    float max = *data;
    float min = *data;
    for (int i = 1; i < total; i++) {
        float f = data[i];
        if (f > max) max = f;
        if (f < min) min = f;
    }
    return QPair<float, float>(min, max);
}
