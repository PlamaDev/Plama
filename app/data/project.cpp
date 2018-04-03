#include "project.h"
#include <Python.h>
#include <QApplication>
#include <QFileDialog>
#include <QStandardPaths>
#include <QtDebug>
#include <algorithm>
#include <cmath>
#include <vector>

using namespace std;

ProjectLoader::ProjectLoader() : globals(PyImport_AddModule("__main__")), list() {
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
    PyObject *variables = PyModule_GetDict(globals);
    rec = PyRun_String(cmd, Py_file_input, variables, variables);
    Py_DecRef(rec);
    PyObject *init = PyObject_GetAttrString(globals, "init");
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

std::unique_ptr<Project> ProjectLoader::load(QString name) const {
    PyObject *fArgs = PyObject_GetAttrString(globals, "args");
    PyObject *fLoad = PyObject_GetAttrString(globals, "load");
    PyObject *args = Py_BuildValue("(s)", name.toLocal8Bit().data());
    PyObject *types = PyObject_CallObject(fArgs, args);
    Py_DECREF(args);
    int len = PyObject_Length(types);
    vector<QPair<QString, int>> typesConverted(len);
    for (int i = 0; i < len; i++) {
        PyObject *arg = PyList_GetItem(types, i);
        QString argName = QString(PyUnicode_AsUTF8(PyTuple_GetItem(arg, 0)));
        int argType = PyLong_AsLong(PyTuple_GetItem(arg, 1));
        typesConverted[i] = {argName, argType};
    }
    PyObject *pluginArgs = buildArgs(typesConverted);
    args = Py_BuildValue("(s, O)", name.toLocal8Bit().data(), pluginArgs);
    PyObject *data = PyObject_CallObject(fLoad, args);
    Py_DECREF(pluginArgs);
    Py_DECREF(args);
    Py_DECREF(types);
    return make_unique<Project>(data);
}

PyObject *ProjectLoader::buildArgs(const std::vector<QPair<QString, int>> &types) {
    PyObject *builder = PyList_New(0);
    for (auto &i : types) {
        switch (i.second) {
        case STRING_LIST:
            PyList_Append(builder,
                buildStringList(QFileDialog::getOpenFileNames(nullptr, i.first)));
            QApplication::processEvents();
            break;
        default: Q_ASSERT(true);
        }
    }
    return builder;
}

PyObject *ProjectLoader::buildStringList(const QStringList &value) {
    PyObject *ret = PyList_New(0);
    for (const auto &i : value)
        PyList_Append(ret, PyUnicode_FromString(i.toLocal8Bit().data()));
    return ret;
}

Project::Project(PyObject *data)
    : nodes(), data(data, [](PyObject *o) { //
          Py_DECREF(o);
      }) {
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
      sizeData(), labels(), data(), max(0), min(0), initialized(false) {
    PyObject *pTimes = PyDict_GetItemString(data, "times");
    PyObject *pSizeData = PyDict_GetItemString(data, "sizeData");
    PyObject *pSizeModel = PyDict_GetItemString(data, "sizeModel");
    PyObject *pLabels = PyDict_GetItemString(data, "labels");
    Py_ssize_t len = PyList_Size(pTimes);

    for (Py_ssize_t i = 0; i < len; i++)
        times.push_back(PyFloat_AsDouble(PyList_GetItem(pTimes, i)));
    len = PyList_Size(pSizeModel);
    for (Py_ssize_t i = 0; i < len; i++) {
        PyObject *o = PyList_GetItem(pSizeModel, i);
        sizeModel.emplace_back(PyFloat_AsDouble(PyList_GetItem(o, 0)),
            PyFloat_AsDouble(PyList_GetItem(o, 1)));
    }

    len = PyList_Size(pSizeData);
    for (Py_ssize_t i = 0; i < len; i++)
        sizeData.push_back(PyLong_AsLong(PyList_GetItem(pSizeData, i)));

    for (Py_ssize_t i = 0; i < PyList_Size(pSizeData) + 2; i++) {
        labels.emplace_back(PyUnicode_AsUTF8(PyList_GetItem(pLabels, i)));
    }
}

const QString &SimQuantity::getName() const { return name; }
const vector<double> &SimQuantity::getTimes() const { return times; }
const vector<VectorD2D> &SimQuantity::getSizeModel() const { return sizeModel; }
const vector<int> &SimQuantity::getSizeData() const { return sizeData; }

const vector<double> &SimQuantity::getDataAt(double time, int dim) {
    auto t = lower_bound(times.begin(), times.end(), time);
    int d = t - times.begin();
    const vector<double> &ret = getData()[dimData * d + dim];
    return ret;
}

const vector<vector<double>> &SimQuantity::getData() {
    if (!initialized) initData();
    return data;
}

const std::vector<QString> &SimQuantity::getLabels() const { return labels; }

VectorD2D SimQuantity::getExtreme() {
    if (!initialized) initData();
    return VectorD2D(min, max);
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
    for (Py_ssize_t i = 0; i < lenI; i++) {
        vector<double> buf;
        PyObject *pBuf = PyList_GetItem(pData, i);
        Py_ssize_t lenJ = PyList_Size(pBuf);
        for (Py_ssize_t j = 0; j < lenJ; j++)
            buf.push_back(PyFloat_AsDouble(PyList_GetItem(pBuf, j)));
        data.push_back(buf);
    }

    if (dimData == 1) {
        max = data[0][0];
        min = max;
        for (auto i : data) {
            for (auto j : i) {
                if (j > max) max = j;
                if (j < min) min = j;
            }
        }
    } else if (dimData == 2) {
        size_t sizeSection = data.size() / 2;
        size_t sizeNumber = 1;
        for (auto i : sizeData) sizeNumber *= i;

        max = magnitude(data[0][0], data[1][0]);
        min = 0;
        for (size_t i = 0; i < sizeSection; i++) {
            vector<double> &vx = data[2 * i];
            vector<double> &vy = data[2 * i + 1];
            for (size_t j = 0; j < sizeNumber; j++) {
                float x = vx[j];
                float y = vy[j];
                float m = magnitude(x, y);
                if (m > max) max = m;
            }
        }
    }

    Py_DECREF(pDataRet);
}
