#include "engine.h"
#include "axis.h"
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
#include <QPen>
#include <QScopedPointer>
#include <algorithm>
#include <memory>

using namespace std;

QVector3D reflect(const QVector3D &d, const QVector3D &n) {
    return d - 2 * QVector3D::dotProduct(d, n) * n;
}

void genMatrix(int rX, int rY, int sX, int sY, bool enBar, const Axis &axis,
    const Bar &bar, QMatrix4x4 &matModel, QMatrix4x4 &matTrans, QMatrix4x4 &matNorm,
    QMatrix4x4 &matScreen, QMatrix4x4 &matBar, QMatrix4x4 &matAxis, QVector3D &vecView) {
    static QVector3D vecViewRaw(1.5, 0, 0);
    static QMatrix4x4 matShift(0.8, 0, 0, -0.2, 0, 0.8, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    matAxis = axis.getTransform(rX, rY);

    // view matrix
    QMatrix4x4 matView;
    matView.rotate(-rX, 0, 0, 1);
    matView.rotate(-rY, 0, 1, 0);
    vecView = matView * vecViewRaw;

    // model matrix
    matModel = QMatrix4x4();
    matModel.translate(-0.5, -0.5f, -0.5f);
    matNorm = QMatrix4x4();
    matNorm = matModel.inverted();
    matNorm = matNorm.transposed();

    // scale test
    QMatrix4x4 matTest;
    matTest.ortho(1, 1, 1, 1, 0.1, 100);
    matTest.lookAt(vecViewRaw, QVector3D(0, 0, 0), QVector3D(0, 0, 1));
    matTest.rotate(rY, 0, 1, 0);
    matTest.rotate(rX, 0, 0, 1);
    matTest = matTest * matAxis * matModel;

    static vector<QVector3D> testers = {
        {1.1, 1.1, -0.2}, {1.1, -0.1, -0.2}, {1.1, -0.2, 1}};
    float size = 0;
    {
        float t = 0;
        for (auto &i : testers) {
            QVector3D v = matTest * i;
            float f1 = abs(v.x());
            float f2 = abs(v.y());
            size = max(max(f1, f2), size);
            t += f1;
            t += f2;
        }
        float f = t / 6 / size;
        size *= 1 + 0.2 * f * f;
    }

    // transform matrix
    matTrans = QMatrix4x4();
    matTrans.ortho(-size, size, -size, size, 0.1, 100);
    matTrans.lookAt(vecViewRaw, QVector3D(0, 0, 0), QVector3D(0, 0, 1));
    matTrans.rotate(rY, 0, 1, 0);
    matTrans.rotate(rX, 0, 0, 1);
    if (enBar) matTrans = matShift * matTrans;

    // others
    matScreen = QMatrix4x4(sX / 2.0, 0, 0, 0, 0, -sY / 2.0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
    matScreen.translate(1, -1);
    matBar = bar.getTransform();
}

void drawTriangle(QPainter &p, const QMatrix4x4 &mat, const vector<QVector3D> &pnt,
    const vector<QVector3D> &clr, const GLuint *ind, int n) {
    if (ind == nullptr) return;
    QPolygonF pl;
    for (int i = 0; i < n; i++) {
        pl.clear();
        QPointF pn[3];
        for (int j = 0; j < 3; j++) pn[j] = (mat * pnt[*(ind + 3 * i + j)]).toPointF();
        QVector3D c = clr[*(ind + 3 * i + 2)];
        QColor cq = QColor(c.x() * 255, c.y() * 255, c.z() * 255, 255);
        for (int i = 0; i < 3; i++) pl << pn[i];
        p.setBrush(cq);
        p.drawPolygon(pl);
    }
}

void drawTriangle(QPainter &p, const QMatrix4x4 &matModel, const QMatrix4x4 &matTrans,
    const QMatrix4x4 &matNormal, const vector<QVector3D> &pnt,
    const vector<QVector3D> &clr, const vector<QVector3D> &pos,
    const vector<QVector3D> &nor, const QVector3D &view, const GLuint *ind, int n) {
    static float (*dot)(const QVector3D &, const QVector3D &) = QVector3D::dotProduct;
    static QVector3D light(1, 1, 1);
    if (ind == nullptr) return;
    QPolygonF pl;
    QMatrix4x4 matFin = matTrans * matModel;
    for (int i = 0; i < n; i++) {
        pl.clear();
        QPointF pn[3];
        for (int j = 0; j < 3; j++) pn[j] = (matFin * pnt[*(ind + 3 * i + j)]).toPointF();
        QVector3D vecColor = clr[*(ind + 3 * i + 2)];
        QVector3D vecPos = matModel * pos[*(ind + 3 * i + 2)];
        QVector3D vecNor = matNormal * nor[*(ind + 3 * i + 2)];
        vecNor.normalize();
        QVector3D vecLight = vecPos - view;
        vecLight.normalize();
        QVector3D reflekt = reflect(vecLight, vecNor);
        float spec = pow(max(dot(reflekt, -vecLight), 0.0f), 8);
        float base = 0.55 + max(0.3f * dot(vecNor, -vecLight), 0.0f);
        QVector3D color = base * vecColor + 0.3 * spec * light;
        QColor cq = QColor(min(int(color.x() * 255), 255), min(int(color.y() * 255), 255),
            min(int(color.z() * 255), 255));
        for (int i = 0; i < 3; i++) pl << pn[i];
        p.setBrush(cq);
        p.drawPolygon(pl);
    }
}

void drawLine(QPainter &p, const QMatrix4x4 &mat, const vector<QVector3D> &pnt,
    const vector<QVector3D> &clr, const GLuint *ind, int n, float width) {
    if (ind == nullptr) return;
    for (int i = 1;; i++) {
        QPointF p1 = (mat * pnt[*(ind + 2 * i)]).toPointF();
        QPointF p2 = (mat * pnt[*(ind + 2 * i + 1)]).toPointF();
        QVector3D c = clr[*(ind + 2 * i + 1)];
        QColor cq = QColor(c.x() * 255, c.y() * 255, c.z() * 255, 255);
        p.setPen(QPen(cq, width));
        p.drawLine(p1, p2);
        if (i == 0) break;
        if (i + 1 == n) i = -1;
    }
}

void drawText(QPainter &painter, QString &&string, const QMatrix4x4 &matrix,
    const QVector3D &anchor, bool left) {
    int i = painter.font().pixelSize() * string.length() * 100;
    int f = (left ? Qt::AlignRight : Qt::AlignLeft) | Qt::AlignVCenter;
    QPointF n = (matrix * anchor).toPointF();
    QRect r = left ? QRect(n.x() - i, n.y() - i / 10, i, i / 5)
                   : QRect(n.x(), n.y() - i / 10, i, i / 5);
    painter.drawText(r, f, string);
}

Engine::Engine(std::shared_ptr<Model> &model, std::shared_ptr<Axis> &axis,
    std::shared_ptr<Bar> &bar, std::shared_ptr<std::vector<QVector2D>> &size)
    : rotX(0), rotY(0), model(model), axis(axis), bar(bar), size(size), label(-1),
      enShader(false), enBar(false) {}

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
void Engine::setShader(bool en) { enShader = en; }
void Engine::setBar(bool en) { enBar = en; }

EngineGL::EngineGL(std::shared_ptr<Model> &model, std::shared_ptr<Axis> &axis,
    std::shared_ptr<Bar> &bar, std::shared_ptr<std::vector<QVector2D>> &size)
    : Engine(model, axis, bar, size) {}

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
    glDisable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}

void EngineGL::render(QPainter &p) {
    p.setRenderHint(QPainter::Antialiasing, true);

    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 matModel, matTrans, matNorm, matScreen, matBar, matAxis;
    QVector3D vecView;
    genMatrix(rotX, rotY, sizeX, sizeY, enBar, *axis, *bar, matModel, matTrans, matNorm,
        matScreen, matBar, matAxis, vecView);
    QMatrix4x4 matText = matScreen * matTrans * matAxis * matModel;

    // draw text
    QFont font = p.font();
    font.setPixelSize(min(sizeX, sizeY) / 35 + 2);
    p.setFont(font);
    const vector<QPair<bool, vector<QVector3D>>> &num = axis->getNumber(rotX, rotY);
    QString format("%1");

    for (int i = 0; i < 3; i++) {
        auto pnts = num[i].second;
        if (pnts.size() == 0) continue;
        float diff = ((*size)[i].y() - (*size)[i].x()) / (pnts.size() - 1);
        for (size_t j = 0; j < pnts.size(); j++) {
            drawText(p, format.arg((*size)[i].x() + diff * j, 0, 'g', 2), matText,
                num[i].second[j], num[i].first);
        }
    }

    if (enBar) {
        const vector<QVector3D> &ns = bar->getNumber();
        float start = (*size)[2].x();
        float diff = ((*size)[2].y() - start) / (ns.size() - 1);
        for (size_t j = 0; j < ns.size(); j++) {
            drawText(p, format.arg(start + diff * j, 0, 'g', 2), matScreen * matBar,
                ns[j], false);
        }
    }

    p.beginNativePainting();
    glEnable(GL_DEPTH_TEST);
    if (enShader) {
        programFlat->bind();
        programFlat->setUniformValue(argFlatMatNormal, matNorm);
        programFlat->setUniformValue(argFlatVecView, vecView);
        programFlat->setUniformValue(argFlatVecLight, vecView);
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
    } else {
        programPlain->bind();
        programPlain->setUniformValue(argPlainMatModel, matModel);
        programPlain->setUniformValue(argPlainMatTrans, matTrans);

        glVertexAttribPointer(
            argPlainVecPnt, 3, GL_FLOAT, GL_FALSE, 0, model->getPoint().data());
        glVertexAttribPointer(
            argPlainVecColor, 3, GL_FLOAT, GL_FALSE, 0, model->getColorF().data());

        glEnableVertexAttribArray(argPlainVecPnt);
        glEnableVertexAttribArray(argPlainVecColor);

        glDrawElements(GL_TRIANGLES, model->getIndexT(0).size(), GL_UNSIGNED_INT,
            model->getIndexT(0).data());

        glDisableVertexAttribArray(argPlainVecPnt);
        glDisableVertexAttribArray(argPlainVecColor);
        programPlain->release();
    }

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
        argPlainVecPnt, 3, GL_FLOAT, GL_FALSE, 0, axis->getPoint().data());
    glVertexAttribPointer(
        argPlainVecColor, 3, GL_FLOAT, GL_FALSE, 0, axis->getColorF().data());
    const GLuint *pnt = axis->getIndex().data();
    for (auto i : axis->getSlice(rotX, rotY))
        glDrawElements(GL_LINES, i.second * 2, GL_UNSIGNED_INT, pnt + i.first);

    if (enBar) {
        programPlain->setUniformValue(argPlainMatModel, bar->getTransform());
        programPlain->setUniformValue(argPlainMatTrans, QMatrix4x4());
        glVertexAttribPointer(
            argPlainVecPnt, 3, GL_FLOAT, GL_FALSE, 0, bar->getPoint().data());
        glVertexAttribPointer(
            argPlainVecColor, 3, GL_FLOAT, GL_FALSE, 0, bar->getColorF().data());
        pnt = bar->getIndex().data();
        QPair<int, int> pair = bar->getSliceL();
        glDrawElements(GL_LINES, pair.second * 2, GL_UNSIGNED_INT, pnt + pair.first);
        pair = bar->getSliceT();
        glDrawElements(GL_TRIANGLES, pair.second * 3, GL_UNSIGNED_INT, pnt + pair.first);

        glDisableVertexAttribArray(argPlainVecPnt);
        glDisableVertexAttribArray(argPlainVecColor);
    }

    glDisable(GL_DEPTH_TEST);
    programPlain->release();

    p.endNativePainting();
}

EngineQt::EngineQt(std::shared_ptr<Model> &model, std::shared_ptr<Axis> &axis,
    std::shared_ptr<Bar> &bar, std::shared_ptr<std::vector<QVector2D>> size)
    : Engine(model, axis, bar, size) {}

void EngineQt::render(QPainter &p) {
    QMatrix4x4 matModel, matTrans, matNorm, matScreen, matBar, matAxis;
    QVector3D vecView;
    genMatrix(rotX, rotY, sizeX, sizeY, enBar, *axis, *bar, matModel, matTrans, matNorm,
        matScreen, matBar, matAxis, vecView);
    QMatrix4x4 matFinA = matScreen * matTrans * matAxis * matModel;
    QMatrix4x4 matFinM = matScreen * matTrans * matModel;
    int dir = (360 - rotX) / 45 % 8;
    int canvas = min(sizeX, sizeY);
    float width = canvas / 500.0;
    QString format("%1");
    const vector<QVector3D> &vPointA = axis->getPoint();
    const vector<QVector3D> &vColorA = axis->getColorF();
    const vector<GLuint> &vIndexA = axis->getIndex();
    vector<QPair<int, int>> vSliceA = axis->getSlice(rotX, rotY);
    const vector<QVector3D> &vNormalM = model->getNormal();
    const vector<QVector3D> &vPointM = model->getPoint();
    const vector<QVector3D> &vColorMF = model->getColorF();
    const vector<QVector3D> &vPositionM = model->getPosition();
    const vector<GLuint> &vIndexMT = model->getIndexT(dir);
    const vector<QVector3D> &vPointB = bar->getPoint();
    const vector<QVector3D> &vColorB = bar->getColorF();
    const vector<GLuint> &vIndexB = bar->getIndex();
    const vector<QVector3D> &vNumberB = bar->getNumber();

    p.setBrush(Qt::white);
    p.drawRect(-1, -1, sizeX + 2, sizeY + 2);

    // draw grid
    for (auto i : vSliceA)
        drawLine(p, matFinA, vPointA, vColorA, vIndexA.data() + i.first, i.second, width);

    // draw text
    p.setPen(QPen(Qt::black, width));
    QFont font = p.font();
    font.setPixelSize(min(sizeX, sizeY) / 35 + 2);
    p.setFont(font);
    const vector<QPair<bool, vector<QVector3D>>> &num = axis->getNumber(rotX, rotY);

    for (int i = 0; i < 3; i++) {
        auto pnts = num[i].second;
        if (pnts.size() == 0) continue;
        float diff = ((*size)[i].y() - (*size)[i].x()) / (pnts.size() - 1);
        for (size_t j = 0; j < pnts.size(); j++) {
            drawText(p, format.arg((*size)[i].x() + diff * j, 0, 'g', 2), matFinA,
                num[i].second[j], num[i].first);
        }
    }

    // draw label
    if (label >= 0) {
        QPointF p1 = (matFinM * QVector3D(label, 0, -0.0001)).toPointF();
        QPointF p2 = (matFinM * QVector3D(label, 1, -0.0001)).toPointF();
        p.setPen(QPen(Qt::red, width));
        p.drawLine(p1, p2);
    }

    // draw model lines
    const vector<GLuint> &vIndexML = model->getIndexL(dir);
    drawLine(p, matFinM, vPointM, vColorMF, vIndexML.data(), vIndexML.size() / 2, width);

    // draw model triangles
    p.setPen(Qt::NoPen);
    if (enShader)
        drawTriangle(p, matModel, matScreen * matTrans, matNorm, vPointM, vColorMF,
            vPositionM, vNormalM, vecView, vIndexMT.data(), vIndexMT.size() / 3);
    else
        drawTriangle(p, matScreen * matTrans * matModel, vPointM, vColorMF,
            vIndexMT.data(), vIndexMT.size() / 3);

    // draw bar
    if (!enBar) return;
    QPair<int, int> slice = bar->getSliceT();
    drawTriangle(p, matScreen * matBar, vPointB, vColorB, vIndexB.data() + slice.first,
        slice.second);
    slice = bar->getSliceL();
    drawLine(p, matScreen * matBar, vPointB, vColorB, vIndexB.data() + slice.first,
        slice.second, width);

    p.setPen(QPen(Qt::black, width));
    float diff = ((*size)[2].y() - (*size)[2].x()) / (vNumberB.size() - 1);
    for (size_t j = 0; j < vNumberB.size(); j++) {
        drawText(p, format.arg((*size)[2].x() + diff * j, 0, 'g', 2), matScreen * matBar,
            vNumberB[j], false);
    }
}
