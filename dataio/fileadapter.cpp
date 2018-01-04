#include "fileadapter.h"
#include <Python.h>
#include <QtDebug>
#include <QVector>
#include <QStandardPaths>
#include <QSharedPointer>
#include <QScopedPointer>
#include <algorithm>

FileAdapterManager::FileAdapterManager() :
    main(PyImport_AddModule("__main__")), list() {
    // read script
    QFile f(":/res/script/plugins.py");

    f.open(QFile::ReadOnly);
    QTextStream ts(&f);
    QString code = ts.readAll();
    f.close();
    QByteArray ba = code.toUtf8();
    char *cmd = ba.data();
    // config dir to PyObject
    QStringList dirs = QStandardPaths::standardLocations(
            QStandardPaths::AppConfigLocation);
    PyObject *dirsp = PyList_New(0);
    for (QString s : dirs) {
        PyList_Append(dirsp, PyUnicode_FromString(s.toLocal8Bit().data()));
    }
    PyObject *args = Py_BuildValue("(O)", dirsp);
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

const QStringList&FileAdapterManager::plugins() const {
    return list;
}

QSharedPointer <FileAdapter> FileAdapterManager::load(QString name,
    QStringList files) const {
    PyObject *pfiles = PyList_New(0);

    for (QString file : files) {
        PyList_Append(pfiles, PyUnicode_FromString(file.toLocal8Bit().data()));
    }
    PyObject *load = PyObject_GetAttrString(main, "load");
    PyObject *args = Py_BuildValue("(s, O)", name.toLocal8Bit().data(), pfiles);
    PyObject *data = PyObject_CallObject(load, args);

    Py_DECREF(pfiles);
    Py_DECREF(load);
    Py_DECREF(args);

    if (data == Py_None) {
        Py_DECREF(data);
        return QSharedPointer <FileAdapter>();
    } else {
        return QSharedPointer <FileAdapter>(new FileAdapter(data));
        //PyErr_Print();
        //return QSharedPointer <FileAdapter>();
    }
}

FileAdapter::FileAdapter(PyObject *data) :
    nodes(new QList <SimTreeNode>()),
    data(data, [](PyObject *p) {
    Py_DECREF(p);
}) {
    Py_ssize_t len = PyList_Size(data);
    for (Py_ssize_t i = 0; i < len; i++) {
        nodes->append(SimTreeNode(PyList_GetItem(data, i)));
    }
}

const QList <SimTreeNode>&FileAdapter::getTopLevelNodes() const {
    return *nodes;
}

SimTreeNode::SimTreeNode(PyObject *data) : raw(data),
    name(PyUnicode_AsUTF8(PyDict_GetItemString(data, "name"))),
    abbr(PyUnicode_AsUTF8(PyDict_GetItemString(data, "abbr"))),
    quantities(new QList <SimQuantity>()), children(new QList <SimTreeNode> ) {
    PyObject *pchildren = PyDict_GetItemString(data, "children");
    PyObject *pquantities = PyDict_GetItemString(data, "quantities");
    Py_ssize_t len = PyList_Size(pchildren);

    for (Py_ssize_t i = 0; i < len; i++) {
        children->append(SimTreeNode(PyList_GetItem(pchildren, i)));
    }
    len = PyList_Size(pquantities);
    for (Py_ssize_t i = 0; i < len; i++) {
        quantities->append(SimQuantity(PyList_GetItem(pquantities, i)));
    }
}

const QString&SimTreeNode::getName() const {
    return name;
}

const QString&SimTreeNode::getAbbr() const {
    return abbr;
}

const QList <SimQuantity>&SimTreeNode::getData() const {
    return *quantities;
}

const QList <SimTreeNode>&SimTreeNode::getChildren() const {
    return *children;
}

SimQuantity::SimQuantity(PyObject *data) :
    raw(data), name(PyUnicode_AsUTF8(PyDict_GetItemString(data, "name"))),
    dimData(PyLong_AsLong(PyDict_GetItemString(data, "dimData"))),
    times(new QVector <float>()), sizeModel(new QVector <float>()),
    sizeData(new QVector <int>()), data(new QList <QVector <float> >()) {
    PyObject *pTimes = PyDict_GetItemString(data, "times");
    PyObject *pSizeData = PyDict_GetItemString(data, "sizeData");
    PyObject *pSizeModel = PyDict_GetItemString(data, "sizeModel");
    Py_ssize_t len = PyList_Size(pTimes);

    for (Py_ssize_t i = 0; i < len; i++) {
        times->append(PyFloat_AsDouble(PyList_GetItem(pTimes, i)));
    }
    len = PyList_Size(pSizeModel);
    for (Py_ssize_t i = 0; i < len; i++) {
        sizeModel->append(PyFloat_AsDouble(PyList_GetItem(pSizeModel, i)));
    }
    len = PyList_Size(pSizeData);
    for (Py_ssize_t i = 0; i < len; i++) {
        sizeData->append(PyLong_AsLong(PyList_GetItem(pSizeData, i)));
    }

    if (times->size() < 2) {
        uniform = true;
    } else {
        uniform = true;
        float step = (times->last() - times->first()) / times->size();
        float error = step / 40;
        QVector <float>::iterator ip = times->begin();
        QVector <float>::iterator ie = times->end();
        for (QVector <float>::iterator i = ip + 1; i != ie; i++) {
            float diff = *i - *ip - step;
            if (diff < -error || diff > error) {
                uniform = false;
                break;
            }
        }
    }
}

bool SimQuantity::isUniform() const {
    return uniform;
}

const QString&SimQuantity::getName() const {
    return name;
}

const QVector <float>&SimQuantity::getTimes() const {
    return *times;
}

const QVector <float>&SimQuantity::getSizeModel() const {
    return *sizeModel;
}

const QVector <int>&SimQuantity::getSizeData() const {
    return *sizeData;
}

const QVector <float>& SimQuantity::getDataAt(float time, int dim) const {
    if (data->size() == 0) {
        initData();
    }
    if (uniform) {
        float step = (times->last() - times->first()) / times->size();
        int index = time / step;
        if (index >= times->size()) index = times->size();
        return (*data)[index * dimData + dim];
    } else {
        return (*data)[0]; // TODO
    }
}

void SimQuantity::initData() const {
    PyObject *args = PyTuple_New(0);
    PyObject *pDataF = PyDict_GetItemString(raw, "data");
    PyObject *pData = PyObject_CallObject(pDataF, args);

    Py_DECREF(args);
    Py_ssize_t lenI = PyList_Size(pData);
    for (Py_ssize_t i = 0; i < lenI; i++) {
        QVector <float> buf;
        PyObject *pBuf = PyList_GetItem(pData, i);
        Py_ssize_t lenJ = PyList_Size(pBuf);
        for (Py_ssize_t j = 0; j < lenJ; j++) {
            buf.append(PyFloat_AsDouble(PyList_GetItem(pBuf, j)));
        }
        data->append(buf);
    }
}
