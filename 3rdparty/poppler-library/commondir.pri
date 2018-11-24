# specifying common dirs

unix {
    #TEMPDIR      = $$PRJDIR/build/tmp/unix/$$TARGET
    #macx:TEMPDIR = $$PRJDIR/build/tmp/osx/$$TARGET

    TEMPDIR      = $$PRJDIR/build/lib/$$TARGET
    macx:TEMPDIR = $$PRJDIR/build/lib/$$TARGET
}

win32 {
    #TEMPDIR  = $$PRJDIR/build/tmp/win32/$$TARGET
    TEMPDIR  = $$PRJDIR/build/lib/$$TARGET
    DEFINES += _WINDOWS WIN32_LEAN_AND_MEAN NOMINMAX
}


DESTDIR      = $$PRJDIR/build/lib
MOC_DIR      = $$TEMPDIR
OBJECTS_DIR  = $$TEMPDIR
RCC_DIR      = $$TEMPDIR
UI_DIR       = $$TEMPDIR/Ui
LIBS        += -L$$PRJDIR/build/lib
