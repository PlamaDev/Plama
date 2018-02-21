#ifndef QFILEADAPTER_H
#define QFILEADAPTER_H

#include <QObject>
#include <QVector>
#include <QVector2D>
#include <QList>
#include <QMap>
#include <QFile>
#include <QTextStream>
#include <QByteArray>
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
    QString getError() const;
    QString getError();

private:
    PyObject *raw;
    QString name, error;
    int dimData;
    bool uniform;
    QVector<float> times;
    QVector<QVector2D> sizeModel;
    QVector<int> sizeData;
    QVector<QVector<float>> data;
    void initData();
    float max, min;
    bool initialized;
};

class SimTreeNode {
public:
    SimTreeNode(PyObject *data);
    SimTreeNode(SimTreeNode &&) = default;
    SimTreeNode &operator= (const SimTreeNode &) = delete;
    const QString &getName() const;
    const QString &getAbbr() const;
    const std::vector<SimQuantity> &getData() const;
    const std::vector<SimTreeNode> &getChildren() const;
private:
    PyObject *raw;
    QString name;
    QString abbr;
    std::vector<SimQuantity> quantities;
    std::vector<SimTreeNode> children;
};

class Project {
public:
    Project(PyObject *data);
    QString getError() const;
    const std::vector<SimTreeNode> &getTopLevelNodes() const;
private:
    std::vector<SimTreeNode> nodes;
    std::unique_ptr<PyObject, std::function<void(PyObject *)>> data;
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
