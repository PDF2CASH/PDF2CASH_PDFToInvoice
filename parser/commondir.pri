# specifying common dirs

PRJNAMEFOLDER = parser

unix {
    TEMPDIR      = $$PRJDIR/tmp/unix/$$TARGET
    macx:TEMPDIR = $$PRJDIR/tmp/osx/$$TARGET
}

win32 {
    TEMPDIR  = $$PRJDIR/tmp/win32/$$TARGET
    DEFINES += _WINDOWS WIN32_LEAN_AND_MEAN NOMINMAX
}

DESTDIR      = $$PRJDIR/lib
MOC_DIR      = $$TEMPDIR
OBJECTS_DIR  = $$TEMPDIR
RCC_DIR      = $$TEMPDIR
UI_DIR       = $$TEMPDIR/Ui
LIBS        += -L$$PRJDIR/lib

