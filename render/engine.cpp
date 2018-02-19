#include "engine.h"
#include <QScopedPointer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QMatrix3x3>
#include <QMatrix>
#include <QColor>
#include <QPainter>
#include <QOpenGLPaintDevice>
#include <QPainterPath>
#include <memory>
#include <algorithm>
#include "render/axis.h"
#include "util.h"

Engine::Engine(std::unique_ptr<Model> &&model, std::unique_ptr<Axis> &&axis)
    : rotX(0), rotY(0), model(std::move(model)), axis(std::move(axis)), label(-1) {}

void Engine::resize(int sizeX, int sizeY) {
    this->sizeX = sizeX;
    this->sizeY = sizeY;
}

QPoint Engine::getRotation() { return QPoint(rotX, rotY); }

void Engine::setModel(std::unique_ptr<Model> &&model) { this->model = std::move(model); }

void Engine::setRotation(int rotX, int rotY) {
    this->rotX = rotX;
    this->rotY = rotY;
}

void Engine::setBackGround(const QColor &color) {
    this->color = QVector3D((float)color.redF(),
            (float)color.greenF(), (float)color.blueF());
}

void Engine::setLabel(float pos) { label = pos; }

EngineGL::EngineGL(std::unique_ptr<Model> &&model,
    std::unique_ptr<Axis> &&axis, const QVector<QVector2D> &size)
    : Engine (std::move(model), std::move(axis)), size(size) {}

void EngineGL::initialize() {
    initializeOpenGLFunctions();

    programFlat.reset(new QOpenGLShaderProgram());
    programFlat->addShaderFromSourceFile(QOpenGLShader::Vertex,
        ":res/render/flat.vsh");
    programFlat->addShaderFromSourceFile(QOpenGLShader::Fragment,
        ":res/render/flat.fsh");
    programFlat->link();

    programPlain.reset(new QOpenGLShaderProgram());
    programPlain->addShaderFromSourceFile(QOpenGLShader::Vertex,
        ":res/render/plain.vsh");
    programPlain->addShaderFromSourceFile(QOpenGLShader::Fragment,
        ":res/render/plain.fsh");
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
    //glEnable(GL_CULL_FACE);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

}

void EngineGL::render(QPainter &p) {
    p.setRenderHint(QPainter::Antialiasing, true);
    //p.setRenderHint(QPainter::HighQualityAntialiasing);

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
    //matModel.scale(1.0f / size.x(), 1.0f / size.y(), 1.0f / size.z());
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

    glVertexAttribPointer(argFlatVecPnt, 3, GL_FLOAT,
        GL_FALSE, 0, model->getPoint().constData());
    glVertexAttribPointer(argFlatVecPos, 3, GL_FLOAT,
        GL_FALSE, 0, model->getPosition().constData());
    glVertexAttribPointer(argFlatVecNormal, 3, GL_FLOAT,
        GL_FALSE, 0, model->getNormal().constData());
    glVertexAttribPointer(argFlatVecColor, 3, GL_FLOAT,
        GL_FALSE, 0, model->getColorF().constData());

    glEnableVertexAttribArray(argFlatVecPnt);
    glEnableVertexAttribArray(argFlatVecPos);
    glEnableVertexAttribArray(argFlatVecNormal);
    glEnableVertexAttribArray(argFlatVecColor);

    glDrawElements(GL_TRIANGLES, model->getIndexT(0).size(), GL_UNSIGNED_INT,
        model->getIndexT(0).constData());

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

    glVertexAttribPointer(argPlainVecPnt, 3, GL_FLOAT, GL_FALSE, 0,
        model->getPoint().constData());
    glVertexAttribPointer(argPlainVecColor, 3, GL_FLOAT, GL_FALSE, 0,
        model->getColorF().constData());

    glDrawElements(GL_LINES, model->getIndexL(0).size(), GL_UNSIGNED_INT,
        model->getIndexL(0).constData());

    if (label >= 0) {
        QVector3D p[2] = {{label, 0, -0.0001}, {label, 1, -0.0001}};
        QVector3D c[2] = {{1, 0, 0}, {1, 0, 0}};
        GLuint i[2] = {0, 1};
        glVertexAttribPointer(argPlainVecPnt, 3, GL_FLOAT, GL_FALSE, 0, p);
        glVertexAttribPointer(argPlainVecColor, 3, GL_FLOAT, GL_FALSE, 0, c);
        glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, i);
    }

    programPlain->setUniformValue(argPlainMatModel, matAxis*matModel);

    glVertexAttribPointer(argPlainVecPnt, 3, GL_FLOAT, GL_FALSE, 0,
        axis->getPoint().constData());
    glVertexAttribPointer(argPlainVecColor, 3, GL_FLOAT, GL_FALSE, 0,
        axis->getColorF().constData());

    GLuint *pnt = (GLuint *)axis->getIndex().constData();

    for (auto i : axis->getSlice(rotX, rotY))
        glDrawElements(GL_LINES, i.second*2, GL_UNSIGNED_INT, pnt + i.first);

    glDisableVertexAttribArray(argPlainVecPnt);
    glDisableVertexAttribArray(argPlainVecColor);

    glDisable(GL_DEPTH_TEST);

    programPlain->release();

    p.endNativePainting();

    QMatrix4x4 matScreen;
    matScreen.translate(1, -1);
    matScreen = QMatrix4x4(sizeX / 2.0, 0, 0, 0, 0, -sizeY / 2.0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 1) * matScreen;
    QMatrix4x4 matText = matScreen * matTrans * matAxis * matModel;

    QVector<QPair<bool, QVector<QVector3D>>> &num = axis->getNumber(rotX, rotY);
    QString format("%1");
    for (int i = 0; i < 3; i++) {
        if (num[i].second.isEmpty()) continue;

        float diff = (size[i].y() - size[i].x()) / (num[i].second.size()-1);
        int f = (num[i].first ? Qt::AlignRight : Qt::AlignLeft) | Qt::AlignVCenter;
        QVector<QVector3D> &ax = num[i].second;
        for (int j = 0; j < ax.size(); j++) {
            QPoint n = (matText * ax[j]).toPoint();
            QRect r = num[i].first ? QRect(n.x() - 100, n.y() - 10, 100, 20) :
                QRect(n.x(), n.y() - 10, 100, 20);

            p.drawText(r, f, format.arg(size[i].x() + diff * j, 0, 'g', 2));
        }
    }
}

EngineQt::EngineQt(std::unique_ptr<Model> &&model,
    std::unique_ptr<Axis> &&axis, const QVector<QVector2D> &size)
    : Engine (std::move(model), std::move(axis)), size(size) {}

QVector3D reflect(const QVector3D &d, const QVector3D &n) {
    return d - 2 * QVector3D::dotProduct(d, n) * n;
}

void EngineQt::render(QPainter &p) {
    p.setBrush(Qt::white);
    p.drawRect(-1, -1, sizeX + 2, sizeY + 2);
    p.setRenderHint(QPainter::Antialiasing, true);
    int dir = (360 - rotX) / 45 % 8;

    QVector3D view(1.5, 0, 0);
    QMatrix4x4 rotation;
    rotation.rotate(-rotX, 0, 0, 1);
    rotation.rotate(-rotY, 0, 1, 0);
    QVector3D vecViewWorld = rotation * view;
    QVector3D vecLightWorld = vecViewWorld;
    // transform matrix
    QMatrix4x4 matTrans;
    matTrans.translate(0.1, 0);
    matTrans.ortho(-1.1, 1.1, -1.1, 1.1, 0.1, 100);
    matTrans.lookAt(view, QVector3D(0, 0, 0), QVector3D(0, 0, 1));
    matTrans.rotate(rotY, 0, 1, 0);
    matTrans.rotate(rotX, 0, 0, 1);
    QMatrix4x4 matModel;
    //matModel.scale(1.0f / size.x(), 1.0f / size.y(), 1.0f / size.z());
    matModel.translate(-0.5, -0.5f, -0.5f);
    QMatrix4x4 matNormal = matModel.inverted();
    matNormal = matNormal.transposed();
    QMatrix4x4 matAxis = axis->getTransform(rotX, rotY);

    QMatrix4x4 matScreen;
    matScreen.translate(1, -1);
    matScreen = QMatrix4x4(sizeX / 2.0, 0, 0, 0, 0, -sizeY / 2.0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 1) * matScreen;
    QMatrix4x4 matText = matScreen * matTrans * matAxis * matModel;


    const QVector<QVector3D> &vPointA = axis->getPoint();
    const QVector<const QColor *> &vColorA = axis->getColorQ();
    const QVector<GLuint> &vIndexA = axis->getIndex();
    QVector<QPair<int, int>> vSliceA = axis->getSlice(rotX, rotY);

    QMatrix4x4 matFinA = matScreen * matTrans * matAxis * matModel;
    int cnt = 0;

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
        for (int j = 1; j < i.second; j++)
            draw(i.first + 2 * j);

    for (auto i : vSliceA)
        draw(i.first);

    p.setPen(Qt::black);
    QVector<QPair<bool, QVector<QVector3D>>> &num = axis->getNumber(rotX, rotY);
    QString format("%1");
    for (int i = 0; i < 3; i++) {
        if (num[i].second.isEmpty()) continue;

        float diff = (size[i].y() - size[i].x()) / (num[i].second.size()-1);
        int f = (num[i].first ? Qt::AlignRight : Qt::AlignLeft) | Qt::AlignVCenter;
        QVector<QVector3D> &ax = num[i].second;
        for (int j = 0; j < ax.size(); j++) {
            QPoint n = (matText * ax[j]).toPoint();
            QRect r = num[i].first ? QRect(n.x() - 100, n.y() - 10, 100, 20) :
                QRect(n.x(), n.y() - 10, 100, 20);

            p.drawText(r, f, format.arg(size[i].x() + diff * j, 0, 'g', 2));
        }
    }

    const QVector<QVector3D> &vNormalM = model->getNormal();
    const QVector<QVector3D> &vPointM = model->getPoint();
    const QVector<QVector3D> &vColorM = model->getColorF();
    const QVector<QVector3D> &vPositionM = model->getPosition();
    const QVector<GLuint> &vIndexM = model->getIndexT(dir);

    static float (*dot)(const QVector3D &,
        const QVector3D &) = QVector3D::dotProduct;

    QMatrix4x4 matFinM = matScreen * matTrans * matModel;
    QVector3D vecLight(1, 1, 1);
    cnt = 0;
    QPolygon poly;
    p.setPen(Qt::NoPen);
    for (auto i : vIndexM) {
        QVector3D pntWorld = matFinM * vPointM[i];
        poly << pntWorld.toPoint();
        if (cnt++ == 2) {
            QVector3D posWorld = matModel * vPositionM[i];
            QVector3D normalWorld = matNormal * vNormalM[i];
            normalWorld.normalize();
            QVector3D lightIn = posWorld - vecLightWorld;
            lightIn.normalize();
            QVector3D vecReflect = reflect(lightIn, normalWorld);
            float spec = pow(std::max(dot(vecReflect, -lightIn), 0.0f), 8);
            float base = 0.55 + std::max(0.3f * dot(normalWorld, -lightIn), 0.0f);
            QVector3D color = base*vColorM[i] + 0.3*spec*vecLight;
            QColor c(std::min(int(color.x()*255), 255), std::min(int(color.y()*255), 255),
                std::min(int(color.z()*255), 255));
            p.setBrush(c);
            p.drawPolygon(poly);
            poly.clear();
            cnt = 0;
        }
    }


}
