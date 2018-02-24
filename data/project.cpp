#include "project.h"
#include <Python.h>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QStandardPaths>
#include <QtDebug>
#include <algorithm>
#include <vector>

using namespace std;

ProjectLoader::ProjectLoader() : main(PyImport_AddModule("__main__")), list() {
    // read script
    QFile f(":script/plugins.py");

    f.open(QFile::ReadOnly);
    QTextStream ts(&f);
    QString code = ts.readAll();
    f.close();
    QByteArray ba = code.toUtf8();
    char *cmd = ba.data();
    // config dir to PyObject
    QStringList dirs =
        QStandardPaths::standardLocations(QStandardPaths::AppConfigLocation);
    PyObject *pDirs = PyList_New(0);
    for (QString s : dirs)
        PyList_Append(pDirs, PyUnicode_FromString(s.toLocal8Bit().data()));
    PyObject *args = Py_BuildValue("(O)", pDirs);
    // run python
    PyObject *rec;
    PyObject *globals = PyModule_GetDict(main);
    rec = PyRun_String(cmd, Py_file_input, globals, globals);
    Py_DECREF(rec);
    PyObject *init = PyObject_GetAttrString(main, "init");
    PyObject *plugins = PyObject_CallObject(init, args);
    Py_DECREF(args);
    Py_DECREF(init);
    // PyObject to QStringList
    int len = PyObject_Length(plugins);
    QStringList ret;
    for (int i = 0; i < len; i++) {
        PyObject *name = PyList_GetItem(plugins, i);
        char *plugin = PyUnicode_AsUTF8(name);
        list << QString(plugin);
    }
    Py_DECREF(plugins);
}

const QStringList &ProjectLoader::plugins() const { return list; }

unique_ptr<Project> ProjectLoader::load(QString name, QStringList files) const {
    PyObject *pfiles = PyList_New(0);

    for (QString file : files)
        PyList_Append(pfiles, PyUnicode_FromString(file.toLocal8Bit().data()));
    PyObject *load = PyObject_GetAttrString(main, "load");
    PyObject *args = Py_BuildValue("(s, O)", name.toLocal8Bit().data(), pfiles);
    PyObject *data = PyObject_CallObject(load, args);

    Py_DECREF(pfiles);
    Py_DECREF(load);
    Py_DECREF(args);

    return make_unique<Project>(data);
}

Project::Project(PyObject *data)
    : nodes(), data(data, [](PyObject *o) { Py_DECREF(o); }) {
    PyObject *value = PyTuple_GetItem(data, 0);
    PyObject *error = PyTuple_GetItem(data, 1);

    if (error == Py_None) {
        Py_ssize_t len = PyList_Size(value);
        for (Py_ssize_t i = 0; i < len; i++) nodes.emplace_back(PyList_GetItem(value, i));
    }
}

QString Project::getError() const {
    PyObject *err = PyTuple_GetItem(data.get(), 1);
    return err == Py_None ? "" : QString(PyUnicode_AsUTF8(err));
}

const vector<SimTreeNode> &Project::getTopLevelNodes() const { return nodes; }

SimTreeNode::SimTreeNode(PyObject *data)
    : raw(data), name(PyUnicode_AsUTF8(PyDict_GetItemString(data, "name"))),
      abbr(PyUnicode_AsUTF8(PyDict_GetItemString(data, "abbr"))), quantities(),
      children() {
    PyObject *pChildren = PyDict_GetItemString(data, "children");
    PyObject *pQuantities = PyDict_GetItemString(data, "quantities");
    Py_ssize_t len = PyList_Size(pChildren);

    for (Py_ssize_t i = 0; i < len; i++)
        children.emplace_back(PyList_GetItem(pChildren, i));
    len = PyList_Size(pQuantities);
    for (Py_ssize_t i = 0; i < len; i++)
        quantities.emplace_back(PyList_GetItem(pQuantities, i));
}

const QString &SimTreeNode::getName() const { return name; }
const QString &SimTreeNode::getAbbr() const { return abbr; }
const vector<SimQuantity> &SimTreeNode::getData() const { return quantities; }
const vector<SimTreeNode> &SimTreeNode::getChildren() const { return children; }

SimQuantity::SimQuantity(PyObject *data)
    : raw(data), name(PyUnicode_AsUTF8(PyDict_GetItemString(data, "name"))),
      dimData(PyLong_AsLong(PyDict_GetItemString(data, "dimData"))), times(), sizeModel(),
      sizeData(), data(), max(0), min(0), initialized(false) {
    PyObject *pTimes = PyDict_GetItemString(data, "times");
    PyObject *pSizeData = PyDict_GetItemString(data, "sizeData");
    PyObject *pSizeModel = PyDict_GetItemString(data, "sizeModel");
    Py_ssize_t len = PyList_Size(pTimes);

    for (Py_ssize_t i = 0; i < len; i++)
        times.push_back(PyFloat_AsDouble(PyList_GetItem(pTimes, i)));
    len = PyList_Size(pSizeModel);
    for (Py_ssize_t i = 0; i < len; i++) {
        PyObject *o = PyList_GetItem(pSizeModel, i);
        sizeModel.push_back(QVector2D(PyFloat_AsDouble(PyList_GetItem(o, 0)),
            PyFloat_AsDouble(PyList_GetItem(o, 1))));
    }

    len = PyList_Size(pSizeData);
    for (Py_ssize_t i = 0; i < len; i++)
        sizeData.push_back(PyLong_AsLong(PyList_GetItem(pSizeData, i)));
}

const QString &SimQuantity::getName() const { return name; }
const vector<float> &SimQuantity::getTimes() const { return times; }
const vector<QVector2D> &SimQuantity::getSizeModel() const { return sizeModel; }
const vector<int> &SimQuantity::getSizeData() const { return sizeData; }

const vector<float> &SimQuantity::getDataAt(float time, int dim) const {
    auto t = lower_bound(times.begin(), times.end(), time);
    int d = t - times.begin();
    const vector<float> &ret = data[dimData * d + dim];
    return ret;
}

const vector<float> &SimQuantity::getDataAt(float time, int dim) {
    if (!initialized) initData();
    return ((const SimQuantity *)this)->getDataAt(time, dim);
}

const vector<vector<float>> &SimQuantity::getData() {
    if (!initialized) initData();
    return data;
}

float SimQuantity::getMax() const { return max; }
float SimQuantity::getMin() const { return min; }
QVector2D SimQuantity::getExtreme() const { return QVector2D(min, max); }

QVector2D SimQuantity::getExtreme() {
    if (!initialized) initData();
    return ((const SimQuantity *)this)->getExtreme();
}

int SimQuantity::getDim() const { return dimData; }

QString SimQuantity::getError() const {
    if (!initialized) return "Data not initialized";
    return error;
}

QString SimQuantity::getError() {
    if (!initialized) initData();
    return error;
}

void SimQuantity::initData() {

    PyObject *pDataFunc = PyDict_GetItemString(raw, "data");
    PyObject *pDataRet = PyObject_CallObject(pDataFunc, NULL);
    PyObject *pData = PyTuple_GetItem(pDataRet, 0);
    PyObject *pErr = PyTuple_GetItem(pDataRet, 1);
    error = pErr == Py_None ? "" : QString(PyUnicode_AsUTF8(pErr));
    initialized = true;

    if (!error.isEmpty()) return;

    Py_ssize_t lenI = PyList_Size(pData);
    float f = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(pData, 0), 0));
    max = f;
    min = f;

    for (Py_ssize_t i = 0; i < lenI; i++) {
        vector<float> buf;
        PyObject *pBuf = PyList_GetItem(pData, i);
        Py_ssize_t lenJ = PyList_Size(pBuf);
        for (Py_ssize_t j = 0; j < lenJ; j++) {
            float f = PyFloat_AsDouble(PyList_GetItem(pBuf, j));
            if (f > max) max = f;
            if (f < min) min = f;
            buf.push_back(f);
        }
        data.push_back(buf);
    }

    Py_DECREF(pDataRet);
}
