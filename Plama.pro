QT       += core gui widgets
TARGET   = Plama
TEMPLATE = app

CONFIG += no_keywords

SOURCES += \
    main.cpp \
    gui/windowmain.cpp \
    data/fileadapter.cpp \
    render/engine.cpp \
    render/model.cpp \
    gui/openglplot.cpp \
    render/axis.cpp \
    util.cpp

HEADERS += \
    gui/windowmain.h \
    data/fileadapter.h \
    render/engine.h \
    render/model.h \
    gui/openglplot.h \
    render/axis.h \
    util.h

RESOURCES += res/resources.qrc

unix: LIBS += -lpython3.6m
unix: INCLUDEPATH += /usr/include/python3.6m

DISTFILES +=
