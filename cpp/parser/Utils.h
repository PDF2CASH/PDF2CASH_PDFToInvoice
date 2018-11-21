#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QTextStream>

#include <PDFDoc.h>
#include <PDFDocFactory.h>
#include <PDFDocEncoding.h>
#include <DateInfo.h>
#include <Outline.h>
#include <GlobalParams.h>

#include <html/HtmlOutputDev.h>

#include <QTextStream>

namespace Utils
{

bool CheckPossibleHeader(QString text);
bool IsNumber(QString text);

GooString* getInfoDate(Dict *infoDict, const char *key);
GooString* getInfoString(Dict *infoDict, const char *key);

bool FileExists(QString path);

inline QTextStream& qStdout()
{
    static QTextStream r{stdout};
    return r;
}

}

#endif // UTILS_H