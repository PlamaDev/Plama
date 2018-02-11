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
    render/axis.cpp \
    util.cpp \
    gui/plot.cpp

HEADERS += \
    gui/windowmain.h \
    data/fileadapter.h \
    render/engine.h \
    render/model.h \
    render/axis.h \
    util.h \
    gui/plot.h

RESOURCES += res/resources.qrc

unix: LIBS += -lpython3.6m
unix: INCLUDEPATH += /usr/include/python3.6m

DISTFILES +=
