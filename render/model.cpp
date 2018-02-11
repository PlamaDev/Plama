#include <render/model.h>
#include <QColor>
#include <QPair>

Gradient::Gradient(QVector<QPair<QColor, float>> data, int steps)
    : cache(steps), step(1.0f / steps) {
    QColor c1 = data[0].first;
    QColor c2 = data[0].first;
    float p1 = 0;
    float p2 = 0;
    int count = 0;
    for (auto i : data) {
        c1 = c2;
        p1 = p2;
        c2 = i.first;
        p2 = i.second;
        for (float pos = count*step; pos < p2; count++, pos = count*step) {
            float diff1 = pos - p1;
            float diff2 = p2 - pos;
            float diffT = p2 - p1;
            int r = (c1.red() * diff2 + c2.red() * diff1) / diffT;
            int g = (c1.green() * diff2 + c2.green() * diff1) / diffT;
            int b = (c1.blue() * diff2 + c2.blue() * diff1) / diffT;
            cache[count] = QColor(r, g, b);
        }
    }
}

const QColor &Gradient::getColor(float pos) const {
    int index = pos/step;
    int size = cache.size();
    return cache[index >= size ? size - 1 : index];
}

std::function<void(std::function<void(int)>&, int, int)> f0 =
[](std::function<void(int)> &f, int x, int y) {
    for (int i = 0; i < x; i++) for (int j = 0; j < y; j++) f(j * x + i);
};

std::function<void(std::function<void(int)>&, int, int)> f1 =
[](std::function<void(int)> &f, int x, int y) {
    for (int i = x-1; i >= 0; i--) for (int j = 0; j < y; j++) f(j * x + i);
};

std::function<void(std::function<void(int)>&, int, int)> f2 =
[](std::function<void(int)> &f, int x, int y) {
    for (int i = 0; i < x; i++) for (int j = y-1; j >= 0; j--) f(j * x + i);
};

std::function<void(std::function<void(int)>&, int, int)> f3 =
[](std::function<void(int)> &f, int x, int y) {
    for (int i = x-1; i >= 0; i--) for (int j = y-1; j >= 0; j--) f(j * x + i);
};

QVector<std::function<void(std::function<void(int)>&, int, int)>>
Model::indexFunc{f0, f0, f1, f1, f3, f3, f2, f2};

Gradient Model::gradientHeightmap({{Qt::blue, 0.0f}, {Qt::cyan, 0.2f},
    {Qt::green, 0.4f}, {Qt::yellow, 0.6f}, {Qt::red, 1.0f}}, 200);

const QVector<GLuint> &Model::getIndexT(int dir) const { return indexT[dir]; }
const QVector<GLuint> &Model::getIndexL(int dir) const { return indexL[dir]; }
const QVector<QVector3D> &Model::getPoint() const { return point; }
const QVector<QVector3D> &Model::getNormal() const { return normal; }
const QVector<QVector3D> &Model::getColorF() const { return colorF; }
const QVector<QVector3D> &Model::getPosition() const { return position; }
const QVector<const QColor *> &Model::getColorQ() const { return colorQ; }

std::unique_ptr<Model> Model::fromQuantity(
    SimQuantity &sq, float time, int dim) {
    static QVector<QVector<GLuint>> order{
        {0, 4, 3, 1, 4, 0, 3, 4, 6, 6, 4, 7, 2, 4, 1, 5, 4, 2, 7, 4, 8, 8, 4, 5},
        {1, 4, 0, 0, 4, 3, 3, 4, 6, 6, 4, 7, 2, 4, 1, 5, 4, 2, 8, 4, 5, 7, 4, 8},
        {2, 4, 1, 5, 4, 2, 1, 4, 0, 0, 4, 3, 8, 4, 5, 7, 4, 8, 3, 4, 6, 6, 4, 7},
        {5, 4, 2, 2, 4, 1, 1, 4, 0, 0, 4, 3, 8, 4, 5, 7, 4, 8, 6, 4, 7, 3, 4, 6},
        {8, 4, 5, 7, 4, 8, 5, 4, 2, 2, 4, 1, 6, 4, 7, 3, 4, 6, 1, 4, 0, 0, 4, 3},
        {7, 4, 8, 8, 4, 5, 5, 4, 2, 2, 4, 1, 6, 4, 7, 3, 4, 6, 0, 4, 3, 1, 4, 0},
        {6, 4, 7, 3, 4, 6, 7, 4, 8, 8, 4, 5, 0, 4, 3, 1, 4, 0, 5, 4, 2, 2, 4, 1},
        {3, 4, 6, 6, 4, 7, 7, 4, 8, 8, 4, 5, 0, 4, 3, 1, 4, 0, 2, 4, 1, 5, 4, 2}
    };
    QVector2D extreme = sq.getExtreme();
    int sizeX = sq.getSizeData()[0];
    int sizeY = sq.getSizeData()[1];
    QVector<float> data = sq.getDataAt(time, dim);
    int sizePoint = (sizeX - 1) * (sizeY - 1) * 9;
    int sizeIndex = (sizeX - 1) * (sizeY - 1) * 24;

    std::unique_ptr<Model> ret =
        std::unique_ptr<Model>(new Model(sizePoint, sizeIndex, 0));

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

            for (int i = 0; i < 3; i ++) {
                px[i] = (x + 0.5f * i) / sx;
                py[i] = (y + 0.5f * i) / sy;
            }

            for (int yy = 0; yy < 3; yy++) {
                for (int xx = 0; xx < 3; xx++) {
                    int i = yy * 3 + xx;
                    p[i] = QVector3D(px[xx], py[yy], d[i]);
                }
            }

            for (int i = 0; i < 9; i++) ret->point[offsetPoint + i] = p[i];

            ret->normal[offsetPoint] = QVector3D::crossProduct(p[1] - p[0], p[4] - p[0]);
            ret->normal[offsetPoint+1] = QVector3D::crossProduct(p[2] - p[1], p[4] - p[1]);
            ret->normal[offsetPoint+2] = QVector3D::crossProduct(p[5] - p[2], p[4] - p[2]);
            ret->normal[offsetPoint+3] = QVector3D::crossProduct(p[0] - p[3], p[4] - p[3]);
            ret->normal[offsetPoint+4] = QVector3D(1, 1, 1);
            ret->normal[offsetPoint+5] = QVector3D::crossProduct(p[8] - p[5], p[4] - p[5]);
            ret->normal[offsetPoint+6] = QVector3D::crossProduct(p[3] - p[6], p[4] - p[6]);
            ret->normal[offsetPoint+7] = QVector3D::crossProduct(p[6] - p[7], p[4] - p[7]);
            ret->normal[offsetPoint+8] = QVector3D::crossProduct(p[7] - p[8], p[4] - p[8]);

            for (int i = 0; i < 9; i++) ret->normal[offsetPoint + i].normalize();

            ret->colorQ[offsetPoint] = &(gradientHeightmap.getColor((d[0] + d[4]) / 2));
            ret->colorQ[offsetPoint + 3] = ret->colorQ[offsetPoint];
            ret->colorQ[offsetPoint + 1] = &gradientHeightmap.getColor((d[2] + d[4]) / 2);
            ret->colorQ[offsetPoint + 2] = ret->colorQ[offsetPoint + 1];
            ret->colorQ[offsetPoint + 5] = &gradientHeightmap.getColor((d[8] + d[4]) / 2);
            ret->colorQ[offsetPoint + 8] = ret->colorQ[offsetPoint + 5];
            ret->colorQ[offsetPoint + 7] = &gradientHeightmap.getColor((d[6] + d[4]) / 2);
            ret->colorQ[offsetPoint + 6] = ret->colorQ[offsetPoint + 7];
            ret->colorQ[offsetPoint + 4] = &gradientHeightmap.getColor(0);

            for (int i = offsetPoint; i < offsetPoint + 9; i++ ) {
                const QColor *c = ret->colorQ[i];
                ret->colorF[i] = QVector3D(c->redF(), c->greenF(), c->blueF());
            }

            ret->position[offsetPoint] = (p[0] + p[1] + p[4]) / 3;
            ret->position[offsetPoint + 1] = (p[1] + p[2] + p[4]) / 3;
            ret->position[offsetPoint + 2] = (p[2] + p[5] + p[4]) / 3;
            ret->position[offsetPoint + 3] = (p[0] + p[3] + p[4]) / 3;
            ret->position[offsetPoint + 4] = p[4];
            ret->position[offsetPoint + 5] = (p[5] + p[8] + p[4]) / 3;
            ret->position[offsetPoint + 6] = (p[6] + p[3] + p[4]) / 3;
            ret->position[offsetPoint + 7] = (p[6] + p[7] + p[4]) / 3;
            ret->position[offsetPoint + 8] = (p[7] + p[8] + p[4]) / 3;

            int offsetIndex = (y * (sizeX - 1) + x) * 24;

            for(int j = 0; j < 8; j++)
                for (int i = 0; i < 24; i++)
                    ret->indexT[j][offsetIndex + i] = order[j][i] + offsetPoint;
        }
    }
    return ret;
}

std::unique_ptr<Model> Model::fromQuantity(SimQuantity &sq, int dim) {
    static QColor c = Qt::blue;

    QVector<float> y(sq.getData().size());
    for (int i = 0; i < sq.getData().size(); i++)
        y[i] = sq.getData()[sq.getDim() * i + dim][0];
    const QVector<float> &x = sq.getTimes();

    std::unique_ptr<Model> ret =
        std::unique_ptr<Model>(new Model(x.size(), 0, (x.size() - 1) * 2));

    float xmin = x[0];
    float xmax = x[x.size() - 1];
    float xdiff = xmax - xmin;
    float ymin = sq.getMin();
    float ymax = sq.getMax();
    float ydiff = ymax - ymin;

    if (ydiff == 0) {
        if (ymin == 0) {
            ymin = -1;
            ymax = 1;
            ydiff = 2;
        } else {
            ymin *= 0.9;
            ymax *= 1.1;
            ydiff = ymax - ymin;
        }
    }

    for (int i = 0; i < x.size(); i++) {
        ret->point[i] = QVector3D((x[i] - xmin) / xdiff, (y[i] - ymin) / ydiff, 0);
        ret->normal[i] = QVector3D(0, 0, 1);
        ret->colorQ[i] = &c;
        ret->colorF[i] = QVector3D(0, 0, 1);
        ret->position[i] = ret->point[i];
    }

    for (int j = 0; j < 8; j++) {
        for (int i = 0; i < x.size() - 1; i++) {
            ret->indexL[j][2 * i] = i;
            ret->indexL[j][2 * i + 1] = i + 1;
        }
    }

    return ret;
}

Model::Model(int sPoint, int sIndexT, int sIndexL)
    : point(sPoint), normal(sPoint), colorF(sPoint), colorQ(sPoint),
      position(sPoint), indexT(8, QVector<GLuint>(sIndexT)),
      indexL(8, QVector<GLuint>(sIndexL)) {}

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
