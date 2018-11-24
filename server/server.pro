QT          += core network
QT          -= gui
CONFIG      += console
osx:CONFIG  -= app_bundle

PRJDIR       = ./
include($$PRJDIR/commondir.pri)

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    parser/Parser.cpp \
    parser/Search.cpp \
    parser/Utils.cpp \
    CHandler.cpp \
    HTTPServer.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

########################################################
# Poppler Include & Library configuration.             #
########################################################

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

########################################################
# QHTTP Include & Library configuration.               #
########################################################
LIBS        += -lqhttp

HEADERS += \
    parser/Parser.h \
    parser/Search.h \
    parser/Utils.h \
    CHandler.h

