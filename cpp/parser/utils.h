#ifndef UTILS_H
#define UTILS_H

#include <qstring.h>

#include <PDFDoc.h>
#include <PDFDocFactory.h>
#include <PDFDocEncoding.h>
#include <DateInfo.h>
#include <Outline.h>
#include <GlobalParams.h>

#include <html/HtmlOutputDev.h>

namespace utils
{
bool CheckPossibleHeader(QString text);
bool IsNumber(QString text);

GooString* getInfoDate(Dict *infoDict, const char *key);
GooString* getInfoString(Dict *infoDict, const char *key);
}

#endif // UTILS_H
