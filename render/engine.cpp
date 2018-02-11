#include "engine.h"
#include <QScopedPointer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QMatrix3x3>
#include <QMatrix>
#include <QColor>
#include <QPainter>
#include <QOpenGLPaintDevice>
#include <memory>
#include "render/axis.h"
#include "util.h"

Engine::Engine(std::unique_ptr<Model> &&model, std::unique_ptr<Axis> &&axis)
    : rotX(0), rotY(0), model(std::move(model)), axis(std::move(axis)) {}

void Engine::resize(int sizeX, int sizeY) {
    this->sizeX = sizeX;
    this->sizeY = sizeY;
}

QPoint Engine::getRotation() {
    return QPoint(rotX, rotY);
}

void Engine::setRotation(int rotX, int rotY) {
    this->rotX = rotX;
    this->rotY = rotY;
}

void Engine::setBackGround(const QColor &color) {
    this->color = QVector3D((float)color.redF(),
            (float)color.greenF(), (float)color.blueF());
}

EngineSimple::EngineSimple(std::unique_ptr<Model> &&model,
    std::unique_ptr<Axis> &&axis, const QVector<QVector2D> &size)
    : Engine (std::move(model), std::move(axis)), size(size) {}

void EngineSimple::initialize() {
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

void EngineSimple::render(QPainter &p) {
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::HighQualityAntialiasing);

    p.beginNativePainting();

    glEnable(GL_DEPTH_TEST);
    int degree = unify(-rotX, 360).second;

    int dir = degree / 45;
    dir %= 8;
    int rotXDiff = degree % 45;

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
    QMatrix4x4 rotateAxis;

    rotateAxis.rotate(90 * (dir / 2), 0, 0, 1);

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

    programPlain->setUniformValue(argPlainMatModel, rotateAxis*matModel);

    glVertexAttribPointer(argPlainVecPnt, 3, GL_FLOAT, GL_FALSE, 0,
        axis->getPoint().constData());
    glVertexAttribPointer(argPlainVecColor, 3, GL_FLOAT, GL_FALSE, 0,
        axis->getColor().constData());

    GLuint *pnt = (GLuint *)axis->getIndex().constData();

    for (auto i : axis->getSlice(dir, rotXDiff, rotY))
        glDrawElements(GL_LINES, i.second*2, GL_UNSIGNED_INT, pnt + i.first);

    glDisableVertexAttribArray(argPlainVecPnt);
    glDisableVertexAttribArray(argPlainVecColor);

    glDisable(GL_DEPTH_TEST);

    programPlain->release();

    QMatrix4x4 matScreen;
    matScreen.translate(1, -1);
    matScreen = QMatrix4x4(sizeX / 2.0, 0, 0, 0, 0, -sizeY / 2.0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 1) * matScreen;
    QMatrix4x4 matText = matScreen * matTrans * rotateAxis * matModel;

    QVector<QVector<QVector3D>> &num = axis->getNumber(dir, rotXDiff, rotY);
    QString format("%1");
    for (int i = 0; i < 3; i++) {
        if (num[i].isEmpty()) continue;

        bool left = i == 0;
        if (dir % 4 > 1) left = !left;

        float diff = (size[i].y() - size[i].x()) / (num[i].size()-1);
        int f = (left ? Qt::AlignLeft : Qt::AlignRight) | Qt::AlignVCenter;
        QVector<QVector3D> &ax = num[i];
        for (int j = 0; j < ax.size(); j++) {
            QPoint n = (matText * ax[j]).toPoint();
            QRect r = left ? QRect(n.x(), n.y() - 10, 100, 20) :
                QRect(n.x() - 100, n.y() - 10, 100, 20);

            p.drawText(r, f, format.arg(size[i].x() + diff * j, 0, 'g', 2));
        }
    }
}

void EngineSimple::resize(int sizeX, int sizeY) {
    Engine::resize(sizeX, sizeY);
    glClear(GL_COLOR_BUFFER_BIT);
}
