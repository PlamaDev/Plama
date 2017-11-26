QT       += core gui widgets opengl
TARGET   = Plama
TEMPLATE = app

CONFIG += no_keywords

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
    interface/windowmain.cpp \
    render/model.cpp \
    render/engine.cpp \
    dataio/fileio.cpp \
    interface/qsplitview.cpp \
    interface/qplot.cpp \
    dataio/fileadapter.cpp

HEADERS += \
    interface/windowmain.h \
    render/model.h \
    render/engine.h \
    dataio/fileio.h \
    interface/qsplitview.h \
    interface/qplot.h \
    dataio/fileadapter.h

INCLUDEPATH += /usr/include/python3.6m

FORMS += \
    interface/windowmain.ui

RESOURCES += \
    resources/resources.qrc


unix: LIBS += -lpython3.6m
