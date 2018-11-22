#-------------------------------------------------
#
# Project created by QtCreator 2018-11-22T21:22:38
#
#-------------------------------------------------

QT       -= gui

TARGET = parser
TEMPLATE = lib
CONFIG += staticlib

PRJDIR    = ./
include($$PRJDIR/commondir.pri)

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        parser.cpp \
    Parser.cpp \
    Utils.cpp

HEADERS += \
        parser.h \
    Parser.h \
    Utils.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../3rdparty/poppler-library/build/lib/release/ -lpoppler-library
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../3rdparty/poppler-library/build/lib/debug/ -lpoppler-library
else:unix: LIBS += -L$$PWD/../3rdparty/poppler-library/build/lib/ -lpoppler-library

INCLUDEPATH += $$PWD/../3rdparty/poppler-library/build/include
DEPENDPATH += $$PWD/../3rdparty/poppler-library/build/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../3rdparty/poppler-library/build/lib/release/libpoppler-library.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../3rdparty/poppler-library/build/lib/debug/libpoppler-library.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../3rdparty/poppler-library/build/lib/release/poppler-library.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../3rdparty/poppler-library/build/lib/debug/poppler-library.lib
else:unix: PRE_TARGETDEPS += $$PWD/../3rdparty/poppler-library/build/lib/libpoppler-library.a
