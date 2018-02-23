QT       += core gui widgets
TARGET   = Plama
TEMPLATE = app

CONFIG += no_keywords

SOURCES += \
    main.cpp \
    gui/windowmain.cpp \
    render/engine.cpp \
    render/model.cpp \
    render/axis.cpp \
    util.cpp \
    gui/plot.cpp \
    data/project.cpp

HEADERS += \
    gui/windowmain.h \
    render/engine.h \
    render/model.h \
    render/axis.h \
    util.h \
    gui/plot.h \
    data/project.h

RESOURCES += \
    res/res.qrc

unix: LIBS += -lpython3.6m
unix: INCLUDEPATH += /usr/include/python3.6m

DESTDIR = .build
OBJECTS_DIR = .build
MOC_DIR = .build
RCC_DIR = .build
UI_DIR = .build

DISTFILES +=
