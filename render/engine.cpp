#include "engine.h"
#include "axis.h"
#include "util/util.h"
#include <QColor>
#include <QFont>
#include <QFontMetricsF>
#include <QMatrix3x3>
#include <QMatrix>
#include <QOpenGLFunctions>
#include <QOpenGLPaintDevice>
#include <QOpenGLShaderProgram>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QRectF>
#include <QScopedPointer>
#include <QSysInfo>
#include <QTextDocument>
#include <algorithm>
#include <memory>

using namespace std;

QVector3D reflect(const QVector3D &d, const QVector3D &n) {
    return d - 2 * QVector3D::dotProduct(d, n) * n;
}

void genMatrix(int rX, int rY, int sX, int sY, bool enBar, const Axis &axis,
    QMatrix4x4 &matModel, QMatrix4x4 &matTrans, QMatrix4x4 &matNormal,
    QMatrix4x4 &matScreen, QMatrix4x4 &matBar, QMatrix4x4 &matAxis, QVector3D &vecView) {
    static QVector3D vecViewRaw(1.5, 0, 0);
    QMatrix4x4 matShift = enBar
        ? QMatrix4x4(0.8, 0, 0, -0.12, 0, 0.85, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)
        : QMatrix4x4(1, 0, 0, 0.08, 0, 1.05, 0, 0.05, 0, 0, 1, 0, 0, 0, 0, 1);
    matAxis = axis.getTransform(rX, rY);

    // view matrix
    QMatrix4x4 matView;
    matView.rotate(-rX, 0, 0, 1);
    matView.rotate(-rY, 0, 1, 0);
    vecView = matView * vecViewRaw;

    // model matrix
    matModel = QMatrix4x4();
    matModel.translate(-0.5, -0.5f, -0.5f);
    matNormal = QMatrix4x4();
    matNormal = matModel.inverted();
    matNormal = matNormal.transposed();

    // scale test
    QMatrix4x4 matTest;
    matTest.ortho(1, 1, 1, 1, 0.1, 100);
    matTest.lookAt(vecViewRaw, QVector3D(0, 0, 0), QVector3D(0, 0, 1));
    matTest.rotate(rY, 0, 1, 0);
    matTest.rotate(rX, 0, 0, 1);
    matTest = matTest * matAxis * matModel;

    static vector<QVector3D> testers = {
        {1.25, 1.25, -0.2}, {1.25, -0.25, -0.2}, {1.25, -0.2, 1}};
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
        float f = t / 7 / size;
        size *= 1 + 0.4 * f * f;
    }

    // transform matrix
    matTrans = QMatrix4x4();
    matTrans.ortho(-size, size, -size, size, 0.1, 100);
    matTrans.lookAt(vecViewRaw, QVector3D(0, 0, 0), QVector3D(0, 0, 1));
    matTrans.rotate(rY, 0, 1, 0);
    matTrans.rotate(rX, 0, 0, 1);
    matTrans = matShift * matTrans;

    // others
    matScreen = QMatrix4x4(sX / 2.0, 0, 0, 0, 0, -sY / 2.0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
    matScreen.translate(1, -1);
    matBar = QMatrix4x4(0.06, 0, 0, 0.62, 0, 1, 0, -0.5, 0, 0, 1, 0, 0, 0, 0, 1);
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

void drawText(QPainter &painter, QString &str, const QMatrix4x4 &matrix,
    const QVector3D &anchor, EnumPosition align) {
    int i = painter.font().pixelSize() * str.length() * 100;
    int f = 0;
    QRectF r;
    QPointF n = (matrix * anchor).toPointF();
    switch (align) {
    case LEFT:
        f = Qt::AlignRight;
        r = QRectF(n.x() - i, n.y() - i / 10, i, i / 5);
        break;
    case RIGHT:
        f = Qt::AlignLeft;
        r = QRectF(n.x(), n.y() - i / 10, i, i / 5);
        break;
    case CENTER:
        f = Qt::AlignCenter;
        r = QRectF(n.x() - i / 2, n.y() - i / 10, i, i / 5);
        break;
    default: Q_ASSERT(false);
    }
    f |= Qt::AlignVCenter;
    painter.drawText(r, f, str);
}

void drawTextRich(QPainter &painter, QTextDocument &td, QPointF pos, QVector2D dir) {
    const float coef = 180 / PI;
    painter.save();
    QSizeF s = td.size();
    VectorD2D polar;
    toPolar(dir.x(), dir.y(), polar);
    float angle = coef * polar.second;
    if (angle >= 90 && angle < 270) angle -= 180;
    painter.translate(pos);
    painter.rotate(angle);
    painter.translate(-s.width() / 2, -s.height() / 2);
    td.drawContents(&painter);
    painter.restore();
}

void drawTextRich(QPainter &painter, QTextDocument &td, QPointF pos) {
    painter.save();
    QSizeF s = td.size();
    pos.rx() -= s.width() / 2;
    pos.ry() -= s.height() / 2;
    painter.translate(pos);
    td.drawContents(&painter);
    painter.restore();
}

QPointF getDiff(QVector2D line, QVector2D expand, float x, float y, float extra = 0) {
    y *= 0.8;
    x *= 0.95;
    VectorD2D linePoloar;
    toPolar(line.x(), line.y(), linePoloar);
    VectorD2D expandPolar;
    toPolar(expand.x(), expand.y(), expandPolar);
    VectorD2D textPolar[4];
    QVector2D textCatsn[4];
    toPolar(x, y, textPolar[0]);
    toPolar(x, -y, textPolar[1]);
    toPolar(x, 0, textPolar[2]);
    toPolar(0, y, textPolar[3]);
    expandPolar.second -= linePoloar.second;
    for (auto &i : textPolar) i.second -= linePoloar.second;
    for (int i = 0; i < 4; i++)
        toCatsn(textPolar[i].first, textPolar[i].second, textCatsn[i]);
    QVector2D expandCatsn;
    toCatsn(expandPolar.first, expandPolar.second, expandCatsn);
    float lenOut = max(fabs(textCatsn[0].y()), fabs(textCatsn[1].y()));
    float lenIn = max(fabs(textCatsn[2].y()), fabs(textCatsn[3].y()));
    float len = (2 * lenOut + lenIn) / 3;
    expandPolar.first *= fabs(len / expandCatsn.y());
    expandPolar.second += linePoloar.second;
    toCatsn(expandPolar.first + extra, expandPolar.second, expandCatsn);
    return expandCatsn.toPointF();
}

QVector2D get2DVector(const QMatrix4x4 &matrix, QVector3D vector) {
    return (matrix * vector - matrix * QVector3D()).toVector2D();
}

void drawTextRich(QPainter &painter, float space, QString &str, const QMatrix4x4 &matrix,
    PositionInfo posInfo) {
    static QTextDocument td;
    static QString color = "<font color=\"black\">%1</font>";
    if (posInfo.align == DISABLED) return;
    td.setHtml(color.arg(str));
    QFont font = painter.font();
    QFontMetricsF met(font);
    font.setPixelSize(1.3 * font.pixelSize());
    td.setDefaultFont(font);
    QVector2D nExpand;
    QVector2D nLine = get2DVector(matrix, posInfo.line);
    if (posInfo.align == PARALLEL) {
        nExpand = QVector2D(nLine.y(), -nLine.x());
        if (QVector2D::dotProduct(nExpand, get2DVector(matrix, posInfo.expand)) < 0)
            nExpand *= -1;
        QPointF pPos = (matrix * posInfo.base).toPointF() +
            getDiff(nLine, nExpand, 1.2 * space, met.height(), met.height() * 0.6);
        drawTextRich(painter, td, pPos, nLine);
    } else {
        nExpand = get2DVector(matrix, posInfo.expand);
        float h = met.height() * 1.6;
        float w = space + td.size().width() / 1.6;
        QPointF pPos = (matrix * posInfo.base).toPointF() + getDiff(nLine, nExpand, w, h);
        drawTextRich(painter, td, pPos);
    }
}

float getAngle(const QMatrix4x4 &matrix, QVector3D p1, QVector3D p2) {
    const float conv = 180 / PI;
    p1 = matrix * p1;
    p2 = matrix * p2;
    QVector3D pd = p2 - p1;
    QVector2D polar;
    toPolar(pd.x(), pd.y(), polar);
    return conv * polar.y();
}

Engine::Engine(std::shared_ptr<Model> &model, std::shared_ptr<Axis> &axis,
    std::shared_ptr<Bar> &bar, std::shared_ptr<std::vector<VectorD2D>> &size,
    std::shared_ptr<std::vector<QString>> &labels)
    : rotX(0), rotY(0), model(model), axis(axis), bar(bar), size(size), labels(labels),
      line(-1), enShader(false), enBar(false), enLine(true) {}

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

void Engine::setLine(float pos) { line = pos; }
void Engine::setShader(bool en) { enShader = en; }
void Engine::setEnBar(bool en) { enBar = en; }
void Engine::setEnLabel(bool en) { enLine = en; }

EngineGL::EngineGL(std::shared_ptr<Model> &model, std::shared_ptr<Axis> &axis,
    std::shared_ptr<Bar> &bar, std::shared_ptr<std::vector<VectorD2D>> &size,
    std::shared_ptr<std::vector<QString>> &labels)
    : Engine(model, axis, bar, size, labels) {}

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

    glClearColor(1, 1, 1, 1);
    glEnable(GL_BLEND);
    glEnable(GL_POLYGON_SMOOTH);
    glDisable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}

void EngineGL::render(QPainter &p) {
    QImage image(sizeX, sizeY, QImage::Format_ARGB32);
    QPainter pImage(&image);
    image.fill(qRgba(255, 255, 255, 255));
    pImage.setRenderHint(QPainter::Antialiasing, true);
    pImage.setRenderHint(QPainter::TextAntialiasing, true);
    pImage.setRenderHint(QPainter::SmoothPixmapTransform, true);

    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);

    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 matModel, matTrans, matNorm, matScreen, matBar, matAxis;
    QVector3D vecView;
    genMatrix(rotX, rotY, sizeX, sizeY, enBar, *axis, matModel, matTrans, matNorm,
        matScreen, matBar, matAxis, vecView);
    QMatrix4x4 matText = matScreen * matTrans * matAxis * matModel;

    // draw text
    QFont font = p.font();

    font.setStyleHint(QFont::SansSerif, QFont::PreferAntialias);
    font.setPixelSize(magnitude(sizeX, sizeY) / (enBar ? 70 : 56) + 1);
    pImage.setFont(font);
    QFontMetricsF met(font);
    float textWidth[]{0, 0, 0};

    const vector<QPair<EnumPosition, vector<QVector3D>>> &num =
        axis->getNumber(rotX, rotY);
    const vector<PositionInfo> &ls = axis->getLabel(rotX, rotY);

    for (int i = 0; i < 3; i++) {
        auto pnts = num[i].second;
        if (pnts.size() == 0) continue;
        double diff = ((*size)[i].second - (*size)[i].first) / (pnts.size() - 1);
        for (size_t j = 0; j < pnts.size(); j++) {
            QString s = format((*size)[i].first + diff * j);
            float w = met.width(s);
            if (w > textWidth[i]) textWidth[i] = w;
            drawText(pImage, s, matText, num[i].second[j], num[i].first);
        }
        drawTextRich(pImage, textWidth[i], (*labels)[i], matText, ls[i]);
    }

    if (enBar) {
        const vector<QVector3D> &ns = bar->getNumber();
        double start = (*size)[2].first;
        double diff = ((*size)[2].second - start) / (ns.size() - 1);
        for (size_t j = 0; j < ns.size(); j++) {
            QString s = format(start + diff * j);
            float w = met.width(s);
            if (w > textWidth[2]) textWidth[2] = w;
            drawText(pImage, s, matScreen * matBar, ns[j], RIGHT);
        }
        drawTextRich(
            pImage, textWidth[2], (*labels)[2], matScreen * matBar, bar->getLabel());
    }

    p.drawImage(QPoint(0, 0), image);

    p.beginNativePainting();
    glEnable(GL_DEPTH_TEST);
    // draw model triangle
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
            argFlatVecColor, 3, GL_FLOAT, GL_FALSE, 0, model->getColor().data());

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
            argPlainVecColor, 3, GL_FLOAT, GL_FALSE, 0, model->getColor().data());

        glEnableVertexAttribArray(argPlainVecPnt);
        glEnableVertexAttribArray(argPlainVecColor);

        glDrawElements(GL_TRIANGLES, model->getIndexT(0).size(), GL_UNSIGNED_INT,
            model->getIndexT(0).data());

        glDisableVertexAttribArray(argPlainVecPnt);
        glDisableVertexAttribArray(argPlainVecColor);
        programPlain->release();
    }

    // draw model line
    programPlain->bind();
    programPlain->setUniformValue(argPlainMatModel, matModel);
    programPlain->setUniformValue(argPlainMatTrans, matTrans);

    glEnableVertexAttribArray(argPlainVecPnt);
    glEnableVertexAttribArray(argPlainVecColor);

    glVertexAttribPointer(
        argPlainVecPnt, 3, GL_FLOAT, GL_FALSE, 0, model->getPoint().data());
    glVertexAttribPointer(
        argPlainVecColor, 3, GL_FLOAT, GL_FALSE, 0, model->getColor().data());
    glDrawElements(GL_LINES, model->getIndexL(0).size(), GL_UNSIGNED_INT,
        model->getIndexL(0).data());

    // draw line
    if (line >= 0 && enLine) {
        QVector3D p[2] = {{line, 0, -0.0001}, {line, 1, -0.0001}};
        QVector3D c[2] = {{1, 0, 0}, {1, 0, 0}};
        GLuint i[2] = {0, 1};
        glVertexAttribPointer(argPlainVecPnt, 3, GL_FLOAT, GL_FALSE, 0, p);
        glVertexAttribPointer(argPlainVecColor, 3, GL_FLOAT, GL_FALSE, 0, c);
        glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, i);
    }

    // draw grid
    programPlain->setUniformValue(argPlainMatModel, matAxis * matModel);
    glVertexAttribPointer(
        argPlainVecPnt, 3, GL_FLOAT, GL_FALSE, 0, axis->getPoint().data());
    glVertexAttribPointer(
        argPlainVecColor, 3, GL_FLOAT, GL_FALSE, 0, axis->getColor().data());
    const GLuint *pnt = axis->getIndex().data();
    for (auto i : axis->getSlice(rotX, rotY))
        glDrawElements(GL_LINES, i.second * 2, GL_UNSIGNED_INT, pnt + i.first);

    // draw bar
    if (enBar) {
        programPlain->setUniformValue(argPlainMatModel, matBar);
        programPlain->setUniformValue(argPlainMatTrans, QMatrix4x4());
        glVertexAttribPointer(
            argPlainVecPnt, 3, GL_FLOAT, GL_FALSE, 0, bar->getPoint().data());
        glVertexAttribPointer(
            argPlainVecColor, 3, GL_FLOAT, GL_FALSE, 0, bar->getColor().data());
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
    std::shared_ptr<Bar> &bar, std::shared_ptr<std::vector<VectorD2D>> size,
    std::shared_ptr<std::vector<QString>> &labels)
    : Engine(model, axis, bar, size, labels) {}

void EngineQt::render(QPainter &p) {
    // p.fillRect(0, 0, sizeX, sizeY, Qt::white);
    QMatrix4x4 matModel, matTrans, matNorm, matScreen, matBar, matAxis;
    QVector3D vecView;
    genMatrix(rotX, rotY, sizeX, sizeY, enBar, *axis, matModel, matTrans, matNorm,
        matScreen, matBar, matAxis, vecView);
    QMatrix4x4 matFinA = matScreen * matTrans * matAxis * matModel;
    QMatrix4x4 matFinM = matScreen * matTrans * matModel;
    int dir = (360 - rotX) / 45 % 8;
    int canvas = min(sizeX, sizeY);
    float width = canvas / 500.0;
    QFont font("sans-serif");
    float textWidth[]{0, 0, 0};
    font.setStyleHint(QFont::SansSerif, QFont::PreferAntialias);
    font.setPixelSize(magnitude(sizeX, sizeY) / (enBar ? 60 : 50) + 1);
    p.setFont(font);
    QFontMetricsF met(font);
    const vector<QVector3D> &vAPoint = axis->getPoint();
    const vector<QVector3D> &vAColor = axis->getColor();
    const vector<GLuint> &vAIndex = axis->getIndex();
    vector<QPair<int, int>> vASlice = axis->getSlice(rotX, rotY);
    const vector<QPair<EnumPosition, vector<QVector3D>>> &vANumber =
        axis->getNumber(rotX, rotY);
    const vector<PositionInfo> &ls = axis->getLabel(rotX, rotY);
    const vector<QVector3D> &vMNormal = model->getNormal();
    const vector<QVector3D> &vMPoint = model->getPoint();
    const vector<QVector3D> &vMColor = model->getColor();
    const vector<QVector3D> &vMPos = model->getPosition();
    const vector<GLuint> &vMIndexT = model->getIndexT(dir);
    const vector<GLuint> &vMIndexL = model->getIndexL(dir);
    const vector<QVector3D> &vBPoint = bar->getPoint();
    const vector<QVector3D> &vBColor = bar->getColor();
    const vector<GLuint> &vBIndex = bar->getIndex();
    const vector<QVector3D> &vBNumber = bar->getNumber();

    // draw grid
    for (auto i : vASlice)
        drawLine(p, matFinA, vAPoint, vAColor, vAIndex.data() + i.first, i.second, width);

    p.setPen(QPen(Qt::black, width));
    for (int i = 0; i < 3; i++) {
        auto pnts = vANumber[i].second;
        if (pnts.size() == 0) continue;
        double diff = ((*size)[i].second - (*size)[i].first) / (pnts.size() - 1);
        for (size_t j = 0; j < pnts.size(); j++) {
            QString s = format((*size)[i].first + diff * j);
            float w = met.width(s);
            if (w > textWidth[i]) textWidth[i] = w;
            drawText(p, s, matFinA, vANumber[i].second[j], vANumber[i].first);
        }
        drawTextRich(p, textWidth[i], (*labels)[i], matFinA, ls[i]);
    }

    // draw label
    if (line >= 0 && enLine) {
        QPointF p1 = (matFinM * QVector3D(line, 0, -0.0001)).toPointF();
        QPointF p2 = (matFinM * QVector3D(line, 1, -0.0001)).toPointF();
        p.setPen(QPen(Qt::red, width));
        p.drawLine(p1, p2);
    }

    // draw model
    drawLine(p, matFinM, vMPoint, vMColor, vMIndexL.data(), vMIndexL.size() / 2, width);
    p.setPen(Qt::NoPen);
    if (enShader)
        drawTriangle(p, matModel, matScreen * matTrans, matNorm, vMPoint, vMColor, vMPos,
            vMNormal, vecView, vMIndexT.data(), vMIndexT.size() / 3);
    else
        drawTriangle(p, matScreen * matTrans * matModel, vMPoint, vMColor,
            vMIndexT.data(), vMIndexT.size() / 3);

    // draw bar
    if (!enBar) return;
    QPair<int, int> slice = bar->getSliceT();
    drawTriangle(p, matScreen * matBar, vBPoint, vBColor, vBIndex.data() + slice.first,
        slice.second);
    slice = bar->getSliceL();
    drawLine(p, matScreen * matBar, vBPoint, vBColor, vBIndex.data() + slice.first,
        slice.second, width);

    p.setPen(QPen(Qt::black, width));
    double diff = ((*size)[2].second - (*size)[2].first) / (vBNumber.size() - 1);
    for (size_t j = 0; j < vBNumber.size(); j++) {
        QString s = format((*size)[2].first + diff * j);
        float w = met.width(s);
        if (w > textWidth[2]) textWidth[2] = w;
        drawText(p, s, matScreen * matBar, vBNumber[j], RIGHT);
    }
    drawTextRich(p, textWidth[2], (*labels)[2], matScreen * matBar, bar->getLabel());
}
