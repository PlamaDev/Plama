#ifndef QFILEADAPTER_H
#define QFILEADAPTER_H
#pragma clang diagnostic ignored "-Wunknown-warning-option"

#include <QObject>
#include <QVector>
#include <QPoint>
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
    bool isUniform() const;
    const QString &getName() const;
    const QVector<float> &getTimes() const;
    const QVector<float> &getSizeModel() const;
    const QVector<int> &getSizeData() const;
    const QVector<float> &getDataAt(float time, int dim = 0) const;

private:
    PyObject *raw;
    QString name;
    int dimData;
    bool uniform;
    QSharedPointer<QVector<float>> times;
    QSharedPointer<QVector<float>> sizeModel;
    QSharedPointer<QVector<int>> sizeData;
    QSharedPointer<QList<QVector<float>>> data;
    void initData() const;
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

class FileAdapter {
public:
    FileAdapter(PyObject *data);
    const QList<SimTreeNode> &getTopLevelNodes() const;
private:
    QSharedPointer<QList<SimTreeNode>> nodes;
    QSharedPointer<PyObject> data;
};

class FileAdapterManager {
public:
    FileAdapterManager();
    const QStringList &plugins() const;
    std::unique_ptr<FileAdapter> load(QString name, QStringList files) const;
private:
    PyObject *main;
    QStringList list;
};

#endif // QFILEADAPTER_H
