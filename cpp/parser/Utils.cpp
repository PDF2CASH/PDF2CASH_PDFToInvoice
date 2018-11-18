#include "Utils.h"

#include <QFileInfo>

namespace Utils
{

bool IsNumber(QString text)
{
    int len = text.size();

    for(int i = 0; i < len; i++)
    {
        if(text[i].isNumber() == false)
            return false;
    }

    return true;
}

///
/// \brief CheckPossibleHeader
/// \param text
/// \return
///
bool CheckPossibleHeader(QString text)
{
    int len = text.size();
    int lenNumber = 0;
    int lenLetter = 0;

    for(int i = 0; i < len; i++)
    {
        if(text[i].isDigit())
            lenNumber++;
        else if(text[i].isLetter())
            lenLetter++;
    }

    return (lenNumber == 0) ? true : (lenNumber > lenLetter) ? false : true;
}

std::string getFileName(const std::string& s)
{
    char sep = '/';

#ifdef _WIN32
    sep = '\\';
#endif

    size_t i = s.rfind(sep, s.length());
    if (i != std::string::npos)
    {
        return(s.substr(i + 1, s.length() - i));
    }

    return("");
}

GooString* getInfoDate(Dict *infoDict, const char *key)
{
    Object obj;
    const char *s;
    int year, mon, day, hour, min, sec, tz_hour, tz_minute;
    char tz;
    struct tm tmStruct;
    GooString *result = nullptr;
    char buf[256];

    obj = infoDict->lookup(key);
    if (obj.isString())
    {
        s = obj.getString()->getCString();
        // TODO do something with the timezone info
        if (parseDateString(s, &year, &mon, &day, &hour, &min, &sec, &tz, &tz_hour, &tz_minute))
        {
            tmStruct.tm_year = year - 1900;
            tmStruct.tm_mon = mon - 1;
            tmStruct.tm_mday = day;
            tmStruct.tm_hour = hour;
            tmStruct.tm_min = min;
            tmStruct.tm_sec = sec;
            tmStruct.tm_wday = -1;
            tmStruct.tm_yday = -1;
            tmStruct.tm_isdst = -1;
            mktime(&tmStruct); // compute the tm_wday and tm_yday fields
            if (strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S+00:00", &tmStruct))
            {
                result = new GooString(buf);
            }
            else
            {
                result = new GooString(s);
            }
        }
        else
        {
            result = new GooString(s);
        }
    }

    return result;
}

GooString* getInfoString(Dict *infoDict, const char *key)
{
    Object obj;
    // Raw value as read from PDF (may be in pdfDocEncoding or UCS2)
    const GooString *rawString;
    // Value converted to unicode
    Unicode *unicodeString;
    int unicodeLength;
    // Value HTML escaped and converted to desired encoding
    GooString *encodedString = nullptr;
    // Is rawString UCS2 (as opposed to pdfDocEncoding)
    GBool isUnicode;

    obj = infoDict->lookup(key);
    if (obj.isString())
    {
        rawString = obj.getString();

        // Convert rawString to unicode
        if (rawString->hasUnicodeMarker())
        {
            isUnicode = gTrue;
            unicodeLength = (obj.getString()->getLength() - 2) / 2;
        }
        else
        {
            isUnicode = gFalse;
            unicodeLength = obj.getString()->getLength();
        }

        unicodeString = new Unicode[unicodeLength];

        for (int i = 0; i<unicodeLength; i++)
        {
            if (isUnicode)
            {
                unicodeString[i] = ((rawString->getChar((i + 1) * 2) & 0xff) << 8) |
                    (rawString->getChar(((i + 1) * 2) + 1) & 0xff);
            }
            else
            {
                unicodeString[i] = pdfDocEncoding[rawString->getChar(i) & 0xff];
            }
        }

        // HTML escape and encode unicode
        encodedString = HtmlFont::HtmlFilter(unicodeString, unicodeLength);

        delete[] unicodeString;
    }

    return encodedString;
}

bool FileExists(QString path)
{
    QFileInfo check_file(path);
    // check if file exists and if yes: Is it really a file and no directory?
    if (check_file.exists() && check_file.isFile())
    {
        return true;
    }
    else
    {
        return false;
    }
}

//QTextStream& qStdOut()
//{
//    static QTextStream ts( stdout );
//    return ts;
//}

}


