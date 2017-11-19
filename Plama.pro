QT       += core gui widgets opengl
TARGET   = Plama
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
    interface/windowmain.cpp \
    render/model.cpp \
    render/engine.cpp \
    dataio/fileio.cpp \
    interface/qsplitview.cpp \
    interface/qplot.cpp

HEADERS += \
    interface/windowmain.h \
    render/model.h \
    render/engine.h \
    dataio/fileio.h \
    interface/qsplitview.h \
    interface/qplot.h

FORMS += \
    interface/windowmain.ui

RESOURCES += \
    resources/resources.qrc
