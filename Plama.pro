QT       += core gui widgets
TARGET   = Plama
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
    interface/windowmain.cpp \
    render/model.cpp \
    render/qdiagram.cpp \
    render/engine.cpp \
    dataio/fileio.cpp

HEADERS += \
    interface/windowmain.h \
    render/model.h \
    render/qdiagram.h \
    render/engine.h \
    dataio/fileio.h

FORMS += \
    interface/windowmain.ui

RESOURCES += \
    resources/resources.qrc
