QT       += core gui widgets svg
TARGET   = Plama
TEMPLATE = app

CONFIG += no_keywords c++14

SOURCES += \
    main.cpp \
    gui/windowmain.cpp \
    render/engine.cpp \
    render/model.cpp \
    render/axis.cpp \
    util.cpp \
    gui/plot.cpp \
    data/project.cpp \
    render/bar.cpp \
    render/gradient.cpp

HEADERS += \
    gui/windowmain.h \
    render/engine.h \
    render/model.h \
    render/axis.h \
    util.h \
    gui/plot.h \
    data/project.h \
    render/bar.h \
    render/gradient.h

RESOURCES += \
    res/res.qrc

unix: LIBS += -lpython3.6m
unix: INCLUDEPATH += /usr/include/python3.6m
win32: LIBS += -L $$(PY_LIBS) -lpython36
win32:INCLUDEPATH += -I $$(PY_INCLUDE)

FORMS +=



