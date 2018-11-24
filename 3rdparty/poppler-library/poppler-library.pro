#-------------------------------------------------
#
# Project created by QtCreator 2018-10-26T15:46:22
#
#-------------------------------------------------

QT       -= gui

TARGET = poppler-library
TEMPLATE = lib
CONFIG += staticlib

# Qt5.5.1 on OSX needs both c++11 and c++14!! the c++14 is not enough
CONFIG  += c++11 c++14

PRJDIR       = ./
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
    fofi/FoFiBase.cc \
    fofi/FoFiEncodings.cc \
    fofi/FoFiIdentifier.cc \
    fofi/FoFiTrueType.cc \
    fofi/FoFiType1.cc \
    fofi/FoFiType1C.cc \
    goo/FixedPoint.cc \
    goo/gfile.cc \
    goo/glibc.cc \
    goo/glibc_strtok_r.cc \
    goo/GooString.cc \
    goo/GooTimer.cc \
    goo/grandom.cc \
    goo/gstrtod.cc \
    goo/ImgWriter.cc \
    goo/JpegWriter.cc \
    goo/NetPBMWriter.cc \
    goo/PNGWriter.cc \
    goo/TiffWriter.cc \
    poppler/html/HtmlFonts.cc \
    poppler/html/HtmlLinks.cc \
    poppler/html/HtmlOutputDev.cc \
    poppler/Annot.cc \
    poppler/Array.cc \
    poppler/BuiltinFont.cc \
    poppler/BuiltinFontTables.cc \
    poppler/CachedFile.cc \
    poppler/Catalog.cc \
    poppler/CharCodeToUnicode.cc \
    poppler/CMap.cc \
    poppler/DateInfo.cc \
    poppler/Decrypt.cc \
    poppler/Dict.cc \
    poppler/Error.cc \
    poppler/FileSpec.cc \
    poppler/FontEncodingTables.cc \
    poppler/FontInfo.cc \
    poppler/Form.cc \
    poppler/Function.cc \
    poppler/Gfx.cc \
    poppler/GfxFont.cc \
    poppler/GfxState.cc \
    poppler/GlobalParams.cc \
    poppler/GlobalParamsWin.cc \
    poppler/Hints.cc \
    poppler/JArithmeticDecoder.cc \
    poppler/JBIG2Stream.cc \
    poppler/JPXStream.cc \
    poppler/Lexer.cc \
    poppler/Linearization.cc \
    poppler/Link.cc \
    poppler/LocalPDFDocBuilder.cc \
    poppler/MarkedContentOutputDev.cc \
    poppler/Movie.cc \
    poppler/NameToCharCode.cc \
    poppler/Object.cc \
    poppler/OptionalContent.cc \
    poppler/Outline.cc \
    poppler/OutputDev.cc \
    poppler/Page.cc \
    poppler/PageLabelInfo.cc \
    poppler/PageTransition.cc \
    poppler/Parser.cc \
    poppler/PDFDoc.cc \
    poppler/PDFDocEncoding.cc \
    poppler/PDFDocFactory.cc \
    poppler/PreScanOutputDev.cc \
    poppler/ProfileData.cc \
    poppler/PSOutputDev.cc \
    poppler/PSTokenizer.cc \
    poppler/Rendition.cc \
    poppler/SecurityHandler.cc \
    poppler/SignatureHandler.cc \
    poppler/SignatureInfo.cc \
    poppler/Sound.cc \
    poppler/StdinCachedFile.cc \
    poppler/StdinPDFDocBuilder.cc \
    poppler/Stream.cc \
    poppler/StructElement.cc \
    poppler/StructTreeRoot.cc \
    poppler/SysFontInfo.cpp \
    poppler/SysFontList.cpp \
    poppler/TextOutputDev.cc \
    poppler/UnicodeMap.cc \
    poppler/UnicodeMapFuncs.cc \
    poppler/UnicodeTypeTable.cc \
    poppler/UTF.cc \
    poppler/ViewerPreferences.cc \
    poppler/XRef.cc

HEADERS += \
    fofi/FoFiBase.h \
    fofi/FoFiEncodings.h \
    fofi/FoFiIdentifier.h \
    fofi/FoFiTrueType.h \
    fofi/FoFiType1.h \
    fofi/FoFiType1C.h \
    goo/FixedPoint.h \
    goo/gdir.h \
    goo/gfile.h \
    goo/glibc.h \
    goo/gmem.h \
    goo/GooCheckedOps.h \
    goo/GooLikely.h \
    goo/GooList.h \
    goo/GooString.h \
    goo/GooTimer.h \
    goo/grandom.h \
    goo/gstrtod.h \
    goo/gtypes.h \
    goo/ImgWriter.h \
    goo/JpegWriter.h \
    goo/NetPBMWriter.h \
    goo/PNGWriter.h \
    goo/TiffWriter.h \
    poppler/html/HtmlFonts.h \
    poppler/html/HtmlLinks.h \
    poppler/html/HtmlOutputDev.h \
    poppler/html/HtmlUtils.h \
    poppler/Annot.h \
    poppler/Array.h \
    poppler/BuiltinFont.h \
    poppler/BuiltinFontTables.h \
    poppler/CachedFile.h \
    poppler/Catalog.h \
    poppler/CharCodeToUnicode.h \
    poppler/CharTypes.h \
    poppler/CMap.h \
    poppler/DateInfo.h \
    poppler/Decrypt.h \
    poppler/Dict.h \
    poppler/Error.h \
    poppler/ErrorCodes.h \
    poppler/FileSpec.h \
    poppler/FontEncodingTables.h \
    poppler/FontInfo.h \
    poppler/Form.h \
    poppler/Function.h \
    poppler/Gfx.h \
    poppler/GfxFont.h \
    poppler/GfxState.h \
    poppler/GfxState_helpers.h \
    poppler/GlobalParams.h \
    poppler/Hints.h \
    poppler/JArithmeticDecoder.h \
    poppler/JBIG2Stream.h \
    poppler/JPXStream.h \
    poppler/Lexer.h \
    poppler/Linearization.h \
    poppler/Link.h \
    poppler/LocalPDFDocBuilder.h \
    poppler/MarkedContentOutputDev.h \
    poppler/Movie.h \
    poppler/NameToCharCode.h \
    poppler/NameToUnicodeTable.h \
    poppler/Object.h \
    poppler/OptionalContent.h \
    poppler/Outline.h \
    poppler/OutputDev.h \
    poppler/Page.h \
    poppler/PageLabelInfo.h \
    poppler/PageLabelInfo_p.h \
    poppler/PageTransition.h \
    poppler/Parser.h \
    poppler/PDFDoc.h \
    poppler/PDFDocBuilder.h \
    poppler/PDFDocEncoding.h \
    poppler/PDFDocFactory.h \
    poppler/poppler-config.h \
    poppler/PopplerCache.h \
    poppler/PreScanOutputDev.h \
    poppler/ProfileData.h \
    poppler/PSOutputDev.h \
    poppler/PSTokenizer.h \
    poppler/Rendition.h \
    poppler/SecurityHandler.h \
    poppler/SignatureHandler.h \
    poppler/SignatureInfo.h \
    poppler/Sound.h \
    poppler/StdinCachedFile.h \
    poppler/StdinPDFDocBuilder.h \
    poppler/Stream-CCITT.h \
    poppler/Stream.h \
    poppler/StructElement.h \
    poppler/StructTreeRoot.h \
    poppler/SysFontInfo.h \
    poppler/SysFontList.h \
    poppler/TextOutputDev.h \
    poppler/UnicodeCClassTables.h \
    poppler/UnicodeCompTables.h \
    poppler/UnicodeDecompTables.h \
    poppler/UnicodeMap.h \
    poppler/UnicodeMapFuncs.h \
    poppler/UnicodeMapTables.h \
    poppler/UnicodeTypeTable.h \
    poppler/UTF.h \
    poppler/ViewerPreferences.h \
    poppler/XRef.h \
    config.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
