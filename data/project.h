#ifndef PROJECT_H
#define PROJECT_H

#include <Python.h>
#include <cmath>
#include <QPair>
#include <QTextStream>
#include <QVector2D>
#include <memory>

class SimTreeNode;

class SimQuantity {
public:
    SimQuantity(PyObject *data);
    const QString &getName() const;
    const std::vector<float> &getTimes() const;
    const std::vector<QVector2D> &getSizeModel() const;
    const std::vector<int> &getSizeData() const;
    const std::vector<float> &getDataAt(float time, int dim = 0) const;
    const std::vector<float> &getDataAt(float time, int dim = 0);
    const std::vector<std::vector<float>> &getData();
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
    std::vector<float> times;
    std::vector<QVector2D> sizeModel;
    std::vector<int> sizeData;
    std::vector<std::vector<float>> data;
    void initData();
    float max, min;
    bool initialized;
};

class SimTreeNode {
public:
    SimTreeNode(PyObject *data);
    SimTreeNode(SimTreeNode &&) = default;
    SimTreeNode &operator=(const SimTreeNode &) = delete;
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
    std::unique_ptr<Project> load(QString name) const;

private:
    const static int STRING_LIST = 1;
    PyObject *globals;
    QStringList list;
    static PyObject *buildArgs(const std::vector<QPair<QString, int>> &types);
    static PyObject *buildStringList(const QStringList &value);
};

#endif // PROJECT_H
