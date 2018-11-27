# This project demonstrates how to use QtWebAppLib by including the
# sources into this project.

TARGET = Server
TEMPLATE = app
QT = core network
CONFIG += console

HEADERS += \
           src/requestmapper.h \
           src/controller/dumpcontroller.h \
           src/controller/templatecontroller.h \
           src/controller/formcontroller.h \
           src/controller/fileuploadcontroller.h \
           src/controller/sessioncontroller.h \
           src/controller/parsercontroller.h \
           src/parser/Parser.h \
           src/parser/Search.h \
           src/parser/Utils.h \
           src/documentcache.h \

SOURCES += src/main.cpp \
           src/requestmapper.cpp \
           src/controller/dumpcontroller.cpp \
           src/controller/templatecontroller.cpp \
           src/controller/formcontroller.cpp \
           src/controller/fileuploadcontroller.cpp \
           src/controller/sessioncontroller.cpp \
           src/controller/parsercontroller.cpp \
           src/parser/Parser.cpp \
           src/parser/Search.cpp \
           src/parser/Utils.cpp \

OTHER_FILES += etc/* etc/docroot/* etc/templates/* etc/ssl/* logs/* ../readme.txt

#---------------------------------------------------------------------------------------
# The following lines include the sources of the QtWebAppLib library
#---------------------------------------------------------------------------------------

include(../3rdparty/QtWebApp/QtWebApp/logging/logging.pri)
include(../3rdparty/QtWebApp/QtWebApp/httpserver/httpserver.pri)
include(../3rdparty/QtWebApp/QtWebApp/templateengine/templateengine.pri)
# Not used: include(../QtWebApp/qtservice/qtservice.pri)

#---------------------------------------------------------------------------------------
# The following lines include the library of the poppler-library
#---------------------------------------------------------------------------------------

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

