#ifndef QFILEADAPTER_H
#define QFILEADAPTER_H
#pragma clang diagnostic ignored "-Wunknown-warning-option"

#include <QObject>
#include <QVector>
#include <QVector2D>
#include <QList>
#include <QMap>
#include <QFile>
#include <QTextStream>
#include <QByteArray>
#include <QSharedPointer>
#include <memory>
#include <Python.h>

class SimTreeNode;

class SimQuantity {
public:
    SimQuantity(PyObject *data);
    const QString &getName() const;
    const QVector<float> &getTimes() const;
    const QVector<QVector2D> &getSizeModel() const;
    const QVector<int> &getSizeData() const;
    const QVector<float> &getDataAt(float time, int dim = 0) const;
    const QVector<float> &getDataAt(float time, int dim = 0);
    const QVector<QVector<float>> &getData();
    float getMax() const;
    float getMin() const;
    QVector2D getExtreme() const;
    QVector2D getExtreme();
    int getDim() const;

private:
    PyObject *raw;
    QString name;
    int dimData;
    bool uniform;
    QVector<float> times;
    QVector<QVector2D> sizeModel;
    QVector<int> sizeData;
    QVector<QVector<float>> data;
    void initData();
    float max, min;
};

class SimTreeNode {
public:
    SimTreeNode(PyObject *data);
    const QString &getName() const;
    const QString &getAbbr() const;
    const QList<SimQuantity> &getData() const;
    const QList<SimTreeNode> &getChildren() const;
private:
    PyObject *raw;
    QString name;
    QString abbr;
    QSharedPointer<QList<SimQuantity>> quantities;
    QSharedPointer<QList<SimTreeNode>> children;
};

class Project {
public:
    Project(PyObject *data);
    const QList<SimTreeNode> &getTopLevelNodes() const;
private:
    QSharedPointer<QList<SimTreeNode>> nodes;
    QSharedPointer<PyObject> data;
};

class ProjectLoader {
public:
    ProjectLoader();
    const QStringList &plugins() const;
    std::unique_ptr<Project> load(QString name, QStringList files) const;
private:
    PyObject *main;
    QStringList list;
};

#endif // PROJECT_H
