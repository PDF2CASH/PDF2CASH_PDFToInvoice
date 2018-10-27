#include <QCoreApplication>

#include <goo/GooString.h>
#include <goo/gmem.h>

#include <PDFDoc.h>
#include <PDFDocFactory.h>
#include <PDFDocEncoding.h>
#include <DateInfo.h>
#include <Outline.h>
#include <GlobalParams.h>

#include <html/HtmlOutputDev.h>

static int firstPage = 1;
static int lastPage = 0;
static GBool rawOrder = gTrue;
GBool printCommands = gTrue;
static GBool printHelp = gFalse;
GBool printHtml = gFalse;
GBool complexMode=gFalse;
GBool singleHtml=gFalse; // singleHtml
GBool isIgnore=gFalse;
static char extension[5]="png";
static double scale=1.5;
GBool noframes=gFalse;
GBool stout=gFalse;
GBool xml=gFalse;
GBool noRoundedCoordinates = gFalse;
static GBool errQuiet=gFalse;
static GBool noDrm=gFalse;
double wordBreakThreshold=10;  // 10%, below converted into a coefficient - 0.1

GBool showHidden = gFalse;
GBool noMerge = gFalse;
GBool fontFullName = gFalse;
static char ownerPassword[33] = "";
static char userPassword[33] = "";
static GBool printVersion = gFalse;

bool ReadInvoiceXML();

std::string getFileName(const std::string& s)
{
    char sep = '/';

#ifdef _WIN32
    sep = '\\';
#endif

    size_t i = s.rfind(sep, s.length());
    if (i != std::string::npos) {
        return(s.substr(i + 1, s.length() - i));
    }

    return("");
}

static GooString* getInfoDate(Dict *infoDict, const char *key) {
    Object obj;
    const char *s;
    int year, mon, day, hour, min, sec, tz_hour, tz_minute;
    char tz;
    struct tm tmStruct;
    GooString *result = nullptr;
    char buf[256];

    obj = infoDict->lookup(key);
    if (obj.isString()) {
        s = obj.getString()->getCString();
        // TODO do something with the timezone info
        if (parseDateString(s, &year, &mon, &day, &hour, &min, &sec, &tz, &tz_hour, &tz_minute)) {
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
            if (strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S+00:00", &tmStruct)) {
                result = new GooString(buf);
            }
            else {
                result = new GooString(s);
            }
        }
        else {
            result = new GooString(s);
        }
    }
    return result;
}

static GooString* getInfoString(Dict *infoDict, const char *key) {
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
    if (obj.isString()) {
        rawString = obj.getString();

        // Convert rawString to unicode
        if (rawString->hasUnicodeMarker()) {
            isUnicode = gTrue;
            unicodeLength = (obj.getString()->getLength() - 2) / 2;
        }
        else {
            isUnicode = gFalse;
            unicodeLength = obj.getString()->getLength();
        }
        unicodeString = new Unicode[unicodeLength];

        for (int i = 0; i<unicodeLength; i++) {
            if (isUnicode) {
                unicodeString[i] = ((rawString->getChar((i + 1) * 2) & 0xff) << 8) |
                    (rawString->getChar(((i + 1) * 2) + 1) & 0xff);
            }
            else {
                unicodeString[i] = pdfDocEncoding[rawString->getChar(i) & 0xff];
            }
        }

        // HTML escape and encode unicode
        encodedString = HtmlFont::HtmlFilter(unicodeString, unicodeLength);
        delete[] unicodeString;
    }

    return encodedString;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ReadInvoiceXML();
    return a.exec();

    GooString* fileName = nullptr;

    fileName = new GooString("/home/litwin/PDFParserTest/test.pdf");

    GooString *docTitle = nullptr;
    GooString *author = nullptr, *keywords = nullptr, *subject = nullptr, *date = nullptr;
    GooString *htmlFileName = nullptr;
    HtmlOutputDev *htmlOut = nullptr;
#ifdef HAVE_SPLASH
    SplashOutputDev *splashOut = nullptr;
#endif
    GBool doOutline;
    GBool ok;
    GooString *ownerPW, *userPW;
    Object info;
    int exit_status = EXIT_FAILURE;

    PDFDoc* doc = nullptr;

    doc = PDFDocFactory().createPDFDoc(*fileName);
    if (doc == nullptr || !doc->isOk())
    {
        printf("Failed to read PDF file.\n");
        return -1;
    }

    bool xml = true;

    std::string s = getFileName(fileName->toStr());
    s.erase(s.find_last_of("."), std::string::npos);

    htmlFileName = new GooString(s);

    // read config file
    globalParams = new GlobalParams();

    // convert from user-friendly percents into a coefficient
    wordBreakThreshold /= 100.0;

    if (scale>3.0) scale = 3.0;
    if (scale<0.5) scale = 0.5;

    if (complexMode || singleHtml) {
        //noframes=gFalse;
        stout = gFalse;
    }

    if (stout) {
        noframes = gTrue;
        complexMode = gFalse;
        singleHtml = gFalse;
    }

    if (xml)
    {
        complexMode = gTrue;
        singleHtml = gFalse;
        noframes = gTrue;
        noMerge = gTrue;
    }

    // get page range
    if (firstPage < 1)
        firstPage = 1;
    if (lastPage < 1 || lastPage > doc->getNumPages())
        lastPage = doc->getNumPages();
    if (lastPage < firstPage) {
        error(errCommandLine, -1,
            "Wrong page range given: the first page ({0:d}) can not be after the last page ({1:d}).",
            firstPage, lastPage);
        //goto error;
    }

    info = doc->getDocInfo();
    if (info.isDict()) {
        docTitle = getInfoString(info.getDict(), "Title");
        author = getInfoString(info.getDict(), "Author");
        keywords = getInfoString(info.getDict(), "Keywords");
        subject = getInfoString(info.getDict(), "Subject");
        date = getInfoDate(info.getDict(), "ModDate");
        if (!date)
            date = getInfoDate(info.getDict(), "CreationDate");
    }
    if (!docTitle) docTitle = new GooString(htmlFileName);

    if (!singleHtml)
        rawOrder = complexMode; // todo: figure out what exactly rawOrder do :)
    else
        rawOrder = singleHtml;

    doOutline = doc->getOutline()->getItems() != nullptr;

    // write text file
    htmlOut = new HtmlOutputDev(doc->getCatalog(), htmlFileName->getCString(),
        docTitle->getCString(),
        author ? author->getCString() : nullptr,
        keywords ? keywords->getCString() : nullptr,
        subject ? subject->getCString() : nullptr,
        date ? date->getCString() : nullptr,
        extension,
        rawOrder,
        firstPage,
        doOutline,
        xml);
    delete docTitle;
    if (author)
    {
        delete author;
    }
    if (keywords)
    {
        delete keywords;
    }
    if (subject)
    {
        delete subject;
    }
    if (date)
    {
        delete date;
    }

    if (htmlOut->isOk())
    {
        doc->displayPages(htmlOut, firstPage, lastPage, 72 * scale, 72 * scale, 0,
            gTrue, gFalse, gFalse);
        htmlOut->dumpDocOutline(doc);
    }

    //ProcessXML();
}

#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>

#include <QMap>
#include <QList>

struct sTEXTDATA
{
    int top;
    int left;
    int height;
    int width;

    long fontIndex;

    QString text;
};

struct sFONTDESCRIPTION
{
    int index; // it's necessary to define?
    int size;
    QString family;
    QString color; // hex value
};

struct sPAGE
{
    long number;
    QString position;

    int top;
    int left;
    int height;
    int width;

    QMap<int, sFONTDESCRIPTION*> fontMap;
    QList<sTEXTDATA*> txtDataList;
};

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

bool GetInvoiceData(QMap<int, sPAGE*>* pageMap)
{
    // List with possibles headers.
    QList<sTEXTDATA*> possibleHeaders;

    // Temporary variables.
    sPAGE* page = nullptr;
    sTEXTDATA* textData = nullptr;

    // Iterator of the page map.
    QMap<int, sPAGE*>::iterator it;

    // 0. Process each page.
    for(it = pageMap->begin(); it != pageMap->end(); ++it)
    {
        page = it.value();
        if(!page) continue;

        // 1. Let to get possibles headers.
        // We will discard texts that only have values.
        // Because it is more likely to get a possible header and try to read the value below the header.
        for(auto itTxt = page->txtDataList.begin(); itTxt != page->txtDataList.end(); ++itTxt)
        {
            textData = *itTxt;
            if(!textData) continue;

            if(CheckPossibleHeader(textData->text))
                possibleHeaders.push_back(textData);
        }

        // Check if we got possibles header, then let to process.
        if(possibleHeaders.size() > 0)
        {
            // Now based on the possible header
            // We will try to get their values from the coordinates (top and left).
        }
        else
        {
            qDebug() << "Maybe error?";
            return false;
        }
    }

    return true;
}

bool ReadInvoiceXML()
{
    QFile file("/home/litwin/MDS/PDF2CASH_PDFToInvoice/cpp/parser/test.xml");
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "Cannot read file" << file.errorString();
        exit(0);
    }

    QMap<int, sPAGE*>* pageMap = new QMap<int, sPAGE*>();

    QXmlStreamReader reader(&file);

    sFONTDESCRIPTION* fontDesc = nullptr;
    sTEXTDATA* txtData = nullptr;

    while(!reader.atEnd() && !reader.hasError())
    {
        QXmlStreamReader::TokenType token = reader.readNext();
        if(token == QXmlStreamReader::StartDocument)
            continue;

        if(token == QXmlStreamReader::StartElement)
        {
            if(reader.name() == "pdf2xml")
                continue;

            if(reader.name() == "page")
            {
                int number = reader.attributes().value("number").toInt();
                QString position = reader.attributes().value("position").toString();
                int top = reader.attributes().value("top").toInt();
                int left = reader.attributes().value("left").toInt();
                int height = reader.attributes().value("height").toInt();
                int width = reader.attributes().value("width").toInt();

                sPAGE* pageData = new sPAGE();
                pageData->number = number;
                pageData->position = position;
                pageData->top = top;
                pageData->left = left;
                pageData->height = height;
                pageData->width = width;

                pageData->fontMap.clear();
                pageData->txtDataList.clear();

                reader.readNext();

                while(!(reader.tokenType() == QXmlStreamReader::EndElement && reader.name() == "page"))
                {
                    if(reader.tokenType() == QXmlStreamReader::StartElement)
                    {
                        if(reader.name() == "fontspec")
                        {
                            if(reader.tokenType() != QXmlStreamReader::StartElement)
                                continue;

                            int id = reader.attributes().value("id").toInt();
                            int size = reader.attributes().value("size").toInt();
                            QString family = reader.attributes().value("family").toString();
                            QString color = reader.attributes().value("color").toString();

                            fontDesc = new sFONTDESCRIPTION();
                            fontDesc->index = id;
                            fontDesc->size = size;
                            fontDesc->color = color;
                            fontDesc->family = family;

                            if(pageData->fontMap.contains(id))
                            {
                                qDebug() << "already has font added with same index!";
                                continue;
                            }

                            pageData->fontMap.insert(id, fontDesc);
                        }

                        if(reader.name() == "text")
                        {
                            if(reader.tokenType() != QXmlStreamReader::StartElement)
                                continue;

                            int topText = reader.attributes().value("top").toInt();
                            int leftText = reader.attributes().value("left").toInt();
                            int widthText = reader.attributes().value("width").toInt();
                            int heightText = reader.attributes().value("height").toInt();
                            int font = reader.attributes().value("font").toInt();

                            QString text = reader.readElementText(QXmlStreamReader::IncludeChildElements);
                            if(text == "")
                            {
                                qDebug() << "text is null.";
                                continue;
                            }

                            txtData = new sTEXTDATA();
                            txtData->top = topText;
                            txtData->left = leftText;
                            txtData->width = widthText;
                            txtData->height = heightText;
                            txtData->fontIndex = font;
                            txtData->text = text;

                            pageData->txtDataList.push_back(txtData);
                        }
                    }
                    reader.readNext();
                }

                // loaded everything from current page node,
                // now let to save in map.
                pageMap->insert(pageData->number, pageData);
            }
        }
    }

    if(pageMap->size() > 0)
    {
        return GetInvoiceData(pageMap);
    }
    else
    {
        qDebug() << "page map is null!";
        return false;
    }
}

