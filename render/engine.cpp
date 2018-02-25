#include "engine.h"
#include "render/axis.h"
#include "util.h"
#include <QColor>
#include <QFont>
#include <QMatrix3x3>
#include <QMatrix>
#include <QOpenGLFunctions>
#include <QOpenGLPaintDevice>
#include <QOpenGLShaderProgram>
#include <QPainter>
#include <QPainterPath>
#include <QScopedPointer>
#include <algorithm>
#include <memory>

using namespace std;

Engine::Engine(shared_ptr<Model> &model, shared_ptr<Axis> &axis)
    : rotX(0), rotY(0), model(model), axis(axis), label(-1) {}

void Engine::resize(int sizeX, int sizeY) {
    this->sizeX = sizeX;
    this->sizeY = sizeY;
}

QPoint Engine::getRotation() { return QPoint(rotX, rotY); }

void Engine::setModel(shared_ptr<Model> &model) { this->model = model; }

void Engine::setRotation(int rotX, int rotY) {
    this->rotX = rotX;
    this->rotY = rotY;
}

void Engine::setBackGround(const QColor &color) {
    this->color =
        QVector3D((float)color.redF(), (float)color.greenF(), (float)color.blueF());
}

void Engine::setLabel(float pos) { label = pos; }

EngineGL::EngineGL(
    shared_ptr<Model> &model, shared_ptr<Axis> &axis, shared_ptr<vector<QVector2D>> &size)
    : Engine(model, axis), size(size) {}

void EngineGL::initialize() {
    initializeOpenGLFunctions();

    programFlat.reset(new QOpenGLShaderProgram());
    programFlat->addShaderFromSourceFile(QOpenGLShader::Vertex, ":render/flat.vsh");
    programFlat->addShaderFromSourceFile(QOpenGLShader::Fragment, ":render/flat.fsh");
    programFlat->link();

    programPlain.reset(new QOpenGLShaderProgram());
    programPlain->addShaderFromSourceFile(QOpenGLShader::Vertex, ":render/plain.vsh");
    programPlain->addShaderFromSourceFile(QOpenGLShader::Fragment, ":render/plain.fsh");
    programPlain->link();

    argFlatVecPnt = programFlat->attributeLocation("vecPnt");
    argFlatVecPos = programFlat->attributeLocation("vecPos");
    argFlatVecNormal = programFlat->attributeLocation("vecNormal");
    argFlatVecColor = programFlat->attributeLocation("vecColor");
    argFlatMatTrans = programFlat->uniformLocation("matTrans");
    argFlatMatModel = programFlat->uniformLocation("matModel");
    argFlatMatNormal = programFlat->uniformLocation("matNormal");
    argFlatVecView = programFlat->uniformLocation("vecView");
    argFlatVecLight = programFlat->uniformLocation("vecLight");

    argPlainMatModel = programPlain->uniformLocation("matModel");
    argPlainMatTrans = programPlain->uniformLocation("matTrans");
    argPlainVecColor = programPlain->attributeLocation("vecColor");
    argPlainVecPnt = programPlain->attributeLocation("vecPnt");

    glClearColor(color.x(), color.y(), color.z(), 1.0f);
    glEnable(GL_BLEND);
    glEnable(GL_POLYGON_SMOOTH);
    // glEnable(GL_CULL_FACE);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}

void EngineGL::render(QPainter &p) {
    p.setRenderHint(QPainter::Antialiasing, true);
    // p.setRenderHint(QPainter::HighQualityAntialiasing);

    p.beginNativePainting();

    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    // view matrix
    QVector3D view(1.5, 0, 0);
    QMatrix4x4 rotation;
    rotation.rotate(-rotX, 0, 0, 1);
    rotation.rotate(-rotY, 0, 1, 0);
    // transform matrix
    QMatrix4x4 matTrans;
    matTrans.translate(0.1, 0);
    matTrans.ortho(-1.1, 1.1, -1.1, 1.1, 0.1, 100);
    matTrans.lookAt(view, QVector3D(0, 0, 0), QVector3D(0, 0, 1));
    matTrans.rotate(rotY, 0, 1, 0);
    matTrans.rotate(rotX, 0, 0, 1);
    QMatrix4x4 matModel;
    // matModel.scale(1.0f / size.x(), 1.0f / size.y(), 1.0f / size.z());
    matModel.translate(-0.5, -0.5f, -0.5f);
    QMatrix4x4 matNormal = matModel.inverted();
    matNormal = matNormal.transposed();
    QMatrix4x4 matAxis = axis->getTransform(rotX, rotY);

    programFlat->bind();
    programFlat->setUniformValue(argFlatMatNormal, matNormal);
    programFlat->setUniformValue(argFlatVecView, rotation * view);
    programFlat->setUniformValue(argFlatVecLight, rotation * view);
    programFlat->setUniformValue(argFlatMatTrans, matTrans);
    programFlat->setUniformValue(argFlatMatModel, matModel);

    glVertexAttribPointer(
        argFlatVecPnt, 3, GL_FLOAT, GL_FALSE, 0, model->getPoint().data());
    glVertexAttribPointer(
        argFlatVecPos, 3, GL_FLOAT, GL_FALSE, 0, model->getPosition().data());
    glVertexAttribPointer(
        argFlatVecNormal, 3, GL_FLOAT, GL_FALSE, 0, model->getNormal().data());
    glVertexAttribPointer(
        argFlatVecColor, 3, GL_FLOAT, GL_FALSE, 0, model->getColorF().data());

    glEnableVertexAttribArray(argFlatVecPnt);
    glEnableVertexAttribArray(argFlatVecPos);
    glEnableVertexAttribArray(argFlatVecNormal);
    glEnableVertexAttribArray(argFlatVecColor);

    glDrawElements(GL_TRIANGLES, model->getIndexT(0).size(), GL_UNSIGNED_INT,
        model->getIndexT(0).data());

    glDisableVertexAttribArray(argFlatVecPnt);
    glDisableVertexAttribArray(argFlatVecPos);
    glDisableVertexAttribArray(argFlatVecNormal);
    glDisableVertexAttribArray(argFlatVecColor);
    programFlat->release();

    programPlain->bind();
    programPlain->setUniformValue(argPlainMatModel, matModel);
    programPlain->setUniformValue(argPlainMatTrans, matTrans);

    glEnableVertexAttribArray(argPlainVecPnt);
    glEnableVertexAttribArray(argPlainVecColor);

    glVertexAttribPointer(
        argPlainVecPnt, 3, GL_FLOAT, GL_FALSE, 0, model->getPoint().data());
    glVertexAttribPointer(
        argPlainVecColor, 3, GL_FLOAT, GL_FALSE, 0, model->getColorF().data());

    glDrawElements(GL_LINES, model->getIndexL(0).size(), GL_UNSIGNED_INT,
        model->getIndexL(0).data());

    if (label >= 0) {
        QVector3D p[2] = {{label, 0, -0.0001}, {label, 1, -0.0001}};
        QVector3D c[2] = {{1, 0, 0}, {1, 0, 0}};
        GLuint i[2] = {0, 1};
        glVertexAttribPointer(argPlainVecPnt, 3, GL_FLOAT, GL_FALSE, 0, p);
        glVertexAttribPointer(argPlainVecColor, 3, GL_FLOAT, GL_FALSE, 0, c);
        glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, i);
    }

    programPlain->setUniformValue(argPlainMatModel, matAxis * matModel);

    glVertexAttribPointer(
        argPlainVecPnt, 3, GL_FLOAT, GL_FALSE, 0, axis->getPoint().constData());
    glVertexAttribPointer(
        argPlainVecColor, 3, GL_FLOAT, GL_FALSE, 0, axis->getColorF().constData());

    GLuint *pnt = (GLuint *)axis->getIndex().constData();

    for (auto i : axis->getSlice(rotX, rotY))
        glDrawElements(GL_LINES, i.second * 2, GL_UNSIGNED_INT, pnt + i.first);

    glDisableVertexAttribArray(argPlainVecPnt);
    glDisableVertexAttribArray(argPlainVecColor);

    glDisable(GL_DEPTH_TEST);

    programPlain->release();

    p.endNativePainting();

    QMatrix4x4 matScreen;
    matScreen.translate(1, -1);
    matScreen =
        QMatrix4x4(sizeX / 2.0, 0, 0, 0, 0, -sizeY / 2.0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1) *
        matScreen;
    QMatrix4x4 matText = matScreen * matTrans * matAxis * matModel;

    QVector<QPair<bool, QVector<QVector3D>>> &num = axis->getNumber(rotX, rotY);
    QString format("%1");
    QFont font = p.font();
    font.setPixelSize(min(sizeX, sizeY) / 45 + 2);
    p.setFont(font);
    for (int i = 0; i < 3; i++) {
        if (num[i].second.isEmpty()) continue;

        float diff = ((*size)[i].y() - (*size)[i].x()) / (num[i].second.size() - 1);
        int f = (num[i].first ? Qt::AlignRight : Qt::AlignLeft) | Qt::AlignVCenter;
        QVector<QVector3D> &ax = num[i].second;
        for (int j = 0; j < ax.size(); j++) {
            QPoint n = (matText * ax[j]).toPoint();
            QRect r = num[i].first ? QRect(n.x() - 100, n.y() - 10, 100, 20)
                                   : QRect(n.x(), n.y() - 10, 100, 20);

            p.drawText(r, f, format.arg((*size)[i].x() + diff * j, 0, 'g', 2));
        }
    }
}

EngineQt::EngineQt(
    shared_ptr<Model> &model, shared_ptr<Axis> &axis, shared_ptr<vector<QVector2D>> size)
    : Engine(model, axis), size(size) {}

QVector3D reflect(const QVector3D &d, const QVector3D &n) {
    return d - 2 * QVector3D::dotProduct(d, n) * n;
}

void EngineQt::render(QPainter &p) {
    p.setBrush(Qt::white);
    p.drawRect(-1, -1, sizeX + 2, sizeY + 2);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::HighQualityAntialiasing, true);
    int dir = (360 - rotX) / 45 % 8;

    // maxtries
    QVector3D view(1.5, 0, 0);
    QMatrix4x4 rotation;
    rotation.rotate(-rotX, 0, 0, 1);
    rotation.rotate(-rotY, 0, 1, 0);
    QVector3D vecViewWorld = rotation * view;
    QVector3D vecLightWorld = vecViewWorld;
    QMatrix4x4 matTrans;
    matTrans.translate(0.1, 0);
    matTrans.ortho(-1.1, 1.1, -1.1, 1.1, 0.1, 100);
    matTrans.lookAt(view, QVector3D(0, 0, 0), QVector3D(0, 0, 1));
    matTrans.rotate(rotY, 0, 1, 0);
    matTrans.rotate(rotX, 0, 0, 1);
    QMatrix4x4 matModel;
    matModel.translate(-0.5, -0.5f, -0.5f);
    QMatrix4x4 matNormal = matModel.inverted();
    matNormal = matNormal.transposed();
    QMatrix4x4 matAxis = axis->getTransform(rotX, rotY);
    QMatrix4x4 matScreen;
    matScreen.translate(1, -1);
    matScreen =
        QMatrix4x4(sizeX / 2.0, 0, 0, 0, 0, -sizeY / 2.0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1) *
        matScreen;
    QMatrix4x4 matFinA = matScreen * matTrans * matAxis * matModel;
    QMatrix4x4 matFinM = matScreen * matTrans * matModel;

    // draw grid
    const QVector<QVector3D> &vPointA = axis->getPoint();
    const QVector<const QColor *> &vColorA = axis->getColorQ();
    const QVector<GLuint> &vIndexA = axis->getIndex();
    QVector<QPair<int, int>> vSliceA = axis->getSlice(rotX, rotY);
    auto draw = [&](int index) {
        const QColor *c = vColorA[vIndexA[index + 1]];
        p.setPen(*c);

        QVector3D pntWorld = matFinA * vPointA[vIndexA[index]];
        QPoint p1 = pntWorld.toPoint();
        pntWorld = matFinA * vPointA[vIndexA[index + 1]];
        QPoint p2 = pntWorld.toPoint();
        p.drawLine(p1, p2);
    };
    for (auto i : vSliceA)
        for (int j = 1; j < i.second; j++) draw(i.first + 2 * j);
    for (auto i : vSliceA) draw(i.first);

    // draw text
    p.setPen(Qt::black);
    QFont font = p.font();
    font.setPixelSize(min(sizeX, sizeY) / 45 + 2);
    p.setFont(font);
    QVector<QPair<bool, QVector<QVector3D>>> &num = axis->getNumber(rotX, rotY);
    QString format("%1");
    for (int i = 0; i < 3; i++) {
        if (num[i].second.isEmpty()) continue;

        float diff = ((*size)[i].y() - (*size)[i].x()) / (num[i].second.size() - 1);
        int f = (num[i].first ? Qt::AlignRight : Qt::AlignLeft) | Qt::AlignVCenter;
        QVector<QVector3D> &ax = num[i].second;
        for (int j = 0; j < ax.size(); j++) {
            QPoint n = (matFinA * ax[j]).toPoint();
            QRect r = num[i].first ? QRect(n.x() - 100, n.y() - 10, 100, 20)
                                   : QRect(n.x(), n.y() - 10, 100, 20);

            p.drawText(r, f, format.arg((*size)[i].x() + diff * j, 0, 'g', 2));
        }
    }

    // draw label
    if (label >= 0) {
        QPoint p1 = (matFinM * QVector3D(label, 0, -0.0001)).toPoint();
        QPoint p2 = (matFinM * QVector3D(label, 1, -0.0001)).toPoint();
        p.setPen(Qt::red);
        p.drawLine(p1, p2);
    }

    // model data
    const vector<QVector3D> &vNormalM = model->getNormal();
    const vector<QVector3D> &vPointM = model->getPoint();
    const vector<QVector3D> &vColorMF = model->getColorF();
    const vector<const QColor *> &vColorMQ = model->getColorQ();
    const vector<QVector3D> &vPositionM = model->getPosition();
    const vector<GLuint> &vIndexMT = model->getIndexT(dir);

    // draw model lines
    QPoint pnt;
    int cnt = 0;
    const vector<GLuint> &vIndexML = model->getIndexL(dir);
    for (auto i : vIndexML) {
        if (cnt++ == 1) {
            p.setPen(*vColorMQ[i]);
            QPoint tmp = (matFinM * vPointM[i]).toPoint();
            p.drawLine(pnt, tmp);
            cnt = 0;
        } else
            pnt = (matFinM * vPointM[i]).toPoint();
    }

    // draw model triangles
    static float (*dot)(const QVector3D &, const QVector3D &) = QVector3D::dotProduct;
    QVector3D vecLight(1, 1, 1);
    QPolygon poly;

    for (auto i : vIndexMT) {
        QVector3D pntWorld = matFinM * vPointM[i];
        poly << pntWorld.toPoint();
        if (cnt++ == 2) {
            QVector3D pos = matModel * vPositionM[i];
            QVector3D normal = matNormal * vNormalM[i];
            normal.normalize();
            QVector3D lightIn = pos - vecLightWorld;
            lightIn.normalize();
            QVector3D reflekt = reflect(lightIn, normal);
            float spec = pow(max(dot(reflekt, -lightIn), 0.0f), 8);
            float base = 0.55 + max(0.3f * dot(normal, -lightIn), 0.0f);
            QVector3D color = base * vColorMF[i] + 0.3 * spec * vecLight;
            QColor c(min(int(color.x() * 255), 255), min(int(color.y() * 255), 255),
                min(int(color.z() * 255), 255));
            p.setPen(c);
            p.setBrush(c);
            p.drawPolygon(poly);
            poly.clear();
            cnt = 0;
        }
    }
}
