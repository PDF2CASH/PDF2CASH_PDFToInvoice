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

    ////////////////////////////////////////////

    ReadInvoiceXML();
    return a.exec();

    ////////////////////////////////////////////

    GooString* fileName = nullptr;

    fileName = new GooString("/home/litwin/MDS/PDF2CASH_PDFToInvoice/cpp/parser/nfe1.pdf");

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

    sTEXTDATA() {}

    sTEXTDATA(int t, int l, int h, int w, long f, QString txt)
    {
        top = t;
        left = l;
        height = h;
        width = w;
        fontIndex = f;
        text = txt;
    }

    sTEXTDATA(sTEXTDATA* data)
    {
        top = data->top;
        left = data->left;
        height = data->height;
        width = data->width;
        fontIndex = data->fontIndex;
        text = data->text;
    }
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

struct sINVOICEDATA
{
    QString header;
    QString value;
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

#include <QRect>
#include <QPoint>
#include <QSize>

const int SPACE_HEIGHT_BETWEEN_HEADER_VALUE = 14;

///
/// \brief TrySimulateRectHeader
/// \param headerRect
/// \param possibleValues
/// \param maxPageHeight
/// \param maxPageWidth
/// \return
///
QRect TrySimulateRectHeader(QRect headerRect, QList<sTEXTDATA*>* possibleValues, int maxPageHeight, int maxPageWidth)
{
    QList<sTEXTDATA*> inRange;
    sTEXTDATA* tmpData = nullptr;

    // Keep headerRect without edits.
    const QRect originalHeaderRect = headerRect;

    // Variables used for calculation of distance.
    int distance = 0;
    int distanceClose = originalHeaderRect.height() / 2;

    // When only one neighbor was found in the same column
    // then there is only one possibility: left or right side.
    bool oneColumn = false;

    // Get all the values that are close to the header in Y.
    for(auto it = possibleValues->begin(); it != possibleValues->end(); ++it)
    {
        tmpData = (*it);

        // Check if is in same position, if yes, let ignore, because is our current header.
        if(tmpData->top == originalHeaderRect.y() &&
           tmpData->left == originalHeaderRect.x())
            continue;

        // Calcule distance between header and tmpData.
        distance = (tmpData->top > originalHeaderRect.y()) ?
                    (tmpData->top - originalHeaderRect.y()) :
                    (originalHeaderRect.y() - tmpData->top);

        // Check if distance calculed is less than lastest distance close.
        if(distance <= distanceClose)
        {
            inRange.push_back(tmpData);
        }
    }

    // Check if not found any value.
    if(inRange.size() == 0)
    {
        qDebug() << "This shoundn't to happen.";
        return headerRect;
    }

    // Let to process values.
    sTEXTDATA* leftData = nullptr;
    sTEXTDATA* rightData = nullptr;

    // Check if found only 1 value in current row
    // Then check if value is in right or left side of our header data.
    if(inRange.size() == 1)
    {
        // Get first value from list.
        tmpData = inRange.first();

        // Set oneColumn in true, because don't is more necessary to search new values.
        oneColumn = true;

        // Calcule max width value, if value is greater than left variable
        // Then is data in right side, else is left side.
        int sizeTotal = originalHeaderRect.left() + originalHeaderRect.width();
        if(tmpData->left > sizeTotal)
        {
            rightData = tmpData;
        }
        else
        {
            leftData = tmpData;
        }
    }

    distanceClose = 0;

    /********************************************************/
    // 1. Get left side more close.
    if(oneColumn == false)
    {
        for(auto it = inRange.begin(); it != inRange.end(); ++it)
        {
            tmpData = (*it);

            if(tmpData->left > originalHeaderRect.left())
                continue;

            distanceClose = originalHeaderRect.left() - tmpData->left;
            if(distance > distanceClose)
            {
                distance = distanceClose;
                leftData = tmpData;
            }
        }
    }

    // Check if we got left data for be processed.
    if(leftData != nullptr)
    {
        QList<sTEXTDATA*> values;

        QRect leftRect(QPoint(leftData->left, leftData->top),
                       QSize(leftData->width, leftData->height));

        int diffLeft = (originalHeaderRect.left() - leftRect.left()) - 1;

        // Update rect of the right.

        // TODO : find a better way to increase height than to use defined value 10.
        QRect newLeftRect(QPoint(leftData->left, leftData->top),
                          QSize(diffLeft, leftData->height + SPACE_HEIGHT_BETWEEN_HEADER_VALUE));

        QRect tmpRect;

        for(auto it = possibleValues->begin(); it != possibleValues->end(); ++it)
        {
            tmpData = (*it);

            // Check if is in same position, if yes, let ignore, because is our current header.
            if(tmpData->top == leftRect.y() &&
               tmpData->left == leftRect.x())
                continue;

            // Sometimes the position of the left of the value is smaller than that of the header
            // so we have to ignore it, otherwise the algorithm will think that this value belongs
            // to another header.
            if((tmpData->left + tmpData->width) > originalHeaderRect.left())
                continue;

            tmpRect = QRect(QPoint(tmpData->left, tmpData->top),
                            QSize(tmpData->width, tmpData->height));

            // Check if current data intersects with our header rect.
            if(newLeftRect.intersects(tmpRect) &&
              (tmpData->left + tmpData->width) > (leftData->left + leftData->width))
            {
                values.push_back(tmpData);
            }
        }

        // Check if got some value for be processed.
        if(values.size() == 0)
        {
            // There is no value below the right header.
            // So only rightData coordinates will be used to update.
            int newLeft = (leftRect.left() + leftRect.width()) + 1;
            int newSizeWidth = (originalHeaderRect.left() - newLeft) + originalHeaderRect.width();

            headerRect = QRect(QPoint(newLeft, originalHeaderRect.top()),
                               QSize(newSizeWidth, originalHeaderRect.height() + SPACE_HEIGHT_BETWEEN_HEADER_VALUE));

            rightData = nullptr;
        }
        else if(values.size() == 1)
        {
            tmpData = values.first();

            // Calcule maximium left and check if this overflow with headerRect left.
            int newLeft = (tmpData->left + tmpData->width) + 1;
            if(newLeft > originalHeaderRect.left())
            {
                // In this case, the poppler processed some information incorrectly
                // it included two values in a single element.

                // TODO : ...
                int b = 3;

                // This problem happen with text: "DATA DA EMISSÃO".
            }

            int newSizeWidth = (originalHeaderRect.left() - newLeft) + originalHeaderRect.width();
            headerRect = QRect(QPoint(newLeft, originalHeaderRect.top()),
                               QSize(newSizeWidth, originalHeaderRect.height() + SPACE_HEIGHT_BETWEEN_HEADER_VALUE));

            leftData = tmpData;
        }
        else
        {
            distance = 0;
            distanceClose = 0;

            // Reset tmpData.
            tmpData = nullptr;

            // In this case, we got more than 1 values closers.
            for(auto it = values.begin(); it != values.end(); ++it)
            {
                rightData = (*it);

                distanceClose = originalHeaderRect.left() - rightData->left;
                if(distanceClose > distance)
                {
                    distance = distanceClose;
                    tmpData = rightData;
                }
            }

            if(tmpData == nullptr)
            {
                // TODO : Necessary to check some case than happen here.
                qDebug() << "This shouln't to happen!";
                return headerRect;
            }
            else
            {
                // Calcule maximium left and check if this overflow with headerRect left.
                int newLeft = (tmpData->left + tmpData->width) + 1;
                if(newLeft > originalHeaderRect.left())
                {
                    // In this case, the poppler processed some information incorrectly
                    // it included two values in a single element.

                    // TODO : ...
                    int b = 3;

                    // This problem happen with text: "DATA DA EMISSÃO".
                }

                int newSizeWidth = (originalHeaderRect.left() - newLeft) + originalHeaderRect.width();
                headerRect = QRect(QPoint(newLeft, originalHeaderRect.top()),
                                   QSize(newSizeWidth, originalHeaderRect.height() + SPACE_HEIGHT_BETWEEN_HEADER_VALUE));

                leftData = tmpData;
            }
        }
    }

    // Update distance.
    distance = maxPageWidth;//originalHeaderRect.left() + originalHeaderRect.width();
    distanceClose = 0;

    bool isLastColumn = true;

    /********************************************************/
    // 2. Get right side more close.
    if(oneColumn == false)
    {
        int totalWidth = originalHeaderRect.left() + originalHeaderRect.width();
        for(auto it = inRange.begin(); it != inRange.end(); ++it)
        {
            tmpData = (*it);

            if(isLastColumn &&
               (tmpData->left + tmpData->width) > totalWidth)
            {
                isLastColumn = false;
            }

            if(tmpData->left < totalWidth)
                continue;

            distanceClose = tmpData->left - totalWidth;
            if(distanceClose < distance)
            {
                distance = distanceClose;
                rightData = tmpData;
            }
        }
    }

    // Check if we found some value in right side.
    if(rightData == nullptr && isLastColumn)
    {
        // There is no value below the right header.
        // So only rightData coordinates will be used to update.
        int newLeft = (originalHeaderRect.left() != headerRect.left()) ?
                       headerRect.left() :
                       originalHeaderRect.left();

        int newSizeWidth = (originalHeaderRect.width() != headerRect.width()) ?
                    headerRect.width() + (maxPageWidth - (originalHeaderRect.left() + originalHeaderRect.width())) - 1 :
                    originalHeaderRect.width() + (maxPageWidth - (originalHeaderRect.left() + originalHeaderRect.width())) - 1;

        headerRect = QRect(QPoint(newLeft, originalHeaderRect.top()),
                           QSize(newSizeWidth, originalHeaderRect.height() + SPACE_HEIGHT_BETWEEN_HEADER_VALUE));
    }
    else if(rightData != nullptr)
    {
        QList<sTEXTDATA*> values;

        QRect rightRect(QPoint(rightData->left, rightData->top),
                        QSize(rightData->width, rightData->height));

        int diffLeft = (rightData->left - (originalHeaderRect.left() + originalHeaderRect.width())) - 1;
        int newWidth = (originalHeaderRect.left() + originalHeaderRect.width()) + 1;

        // Update rect of the right.

        // TODO : find a better way to increase height than to use defined value 10.
        QRect newRightRect(QPoint(newWidth, rightData->top),
                           QSize(diffLeft, rightData->height + SPACE_HEIGHT_BETWEEN_HEADER_VALUE));

        // Temporary variable used for intersects.
        QRect tmpRect;

        // TODO : find a better way to increase height than to use defined value 10.
        QRect tmpRectHeader(QPoint(headerRect.left(), headerRect.top()),
                            QSize(headerRect.width(), headerRect.height() + SPACE_HEIGHT_BETWEEN_HEADER_VALUE));

        for(auto it = possibleValues->begin(); it != possibleValues->end(); ++it)
        {
            tmpData = (*it);

            // Check if is in same position, if yes, let ignore, because is our current header.
            if(tmpData->top == rightRect.y() &&
               tmpData->left == rightRect.x())
                continue;

            tmpRect = QRect(QPoint(tmpData->left, tmpData->top),
                            QSize(tmpData->width, tmpData->height));

            // Check if current data intersects with our header rect.
            if(newRightRect.intersects(tmpRect))
            {
                // Check if it intercepts with the original header
                // It's own value we're looking for, but we'll ignore it for now.
                if(tmpRectHeader.intersects(tmpRect))
                    continue;

                values.push_back(tmpData);
            }
        }

        if(values.size() == 0)
        {
            // There is no value below the right header.
            // So only rightData coordinates will be used to update.
            int newLeft = (originalHeaderRect.left() != headerRect.left()) ?
                           headerRect.left() :
                           originalHeaderRect.left();

            int newSizeWidth = (originalHeaderRect.width() != headerRect.width()) ?
                        headerRect.width() + (rightData->left - (originalHeaderRect.left() + originalHeaderRect.width())) - 1 :
                        originalHeaderRect.width() + (rightData->left - (originalHeaderRect.left() + originalHeaderRect.width())) - 1;

            headerRect = QRect(QPoint(newLeft, originalHeaderRect.top()),
                               QSize(newSizeWidth, originalHeaderRect.height() + SPACE_HEIGHT_BETWEEN_HEADER_VALUE));

            rightData = nullptr;
        }
        else if(values.size() == 1)
        {
            tmpData = values.first();

            // There is no value below the right header.
            // So only rightData coordinates will be used to update.
            int newLeft = (originalHeaderRect.left() != headerRect.left()) ?
                           headerRect.left() :
                           originalHeaderRect.left();

            int newSizeWidth = (originalHeaderRect.width() != headerRect.width()) ?
                        headerRect.width() + (tmpData->left - (originalHeaderRect.left() + originalHeaderRect.width())) - 1 :
                        originalHeaderRect.width() + (tmpData->left - (originalHeaderRect.left() + originalHeaderRect.width())) - 1;

            headerRect = QRect(QPoint(newLeft, originalHeaderRect.top()),
                               QSize(newSizeWidth, originalHeaderRect.height() + SPACE_HEIGHT_BETWEEN_HEADER_VALUE));

            rightData = tmpData;
        }
        else
        {
            // TODO : finish it!
            for(auto it = values.begin(); it != values.end(); ++it)
            {

            }

            int b = 3;
        }
    }

    return headerRect;
}

bool TryGetValue(sTEXTDATA* header, QList<sTEXTDATA*>* possibleValues, QString* value, int maxPageHeight, int maxPageWidth)
{
    QList<sTEXTDATA*> values;
    sTEXTDATA* tmpData = nullptr;

    QRect headerRect(QPoint(header->left, header->top),
                     QSize(header->width, header->height));

    headerRect = TrySimulateRectHeader(headerRect, possibleValues, maxPageHeight, maxPageWidth);

    QRect tmpRect;

    for(auto it = possibleValues->begin(); it != possibleValues->end(); ++it)
    {
        tmpData = (*it);

        // Ignore if is same header.
        if(tmpData == header || tmpData->text == header->text)
            continue;

        tmpRect = QRect(QPoint(tmpData->left, tmpData->top), QSize(tmpData->width, tmpData->height));

        if(headerRect.intersects(tmpRect))
        {
            values.push_back(tmpData);
        }
    }

    if(values.size() == 0)
    {
        *value = "";
        return false;
    }
    else if(values.size() == 1)
    {
        tmpData = values.first();
        *value = tmpData->text;

        return true;
    }
    else
    {
        sTEXTDATA* txtData = nullptr;
        int distanceTop = header->height / 2;

        int distance = -1;
        int tmpDistance = 0;

        for(auto it = values.begin(); it != values.end(); ++it)
        {
            tmpData = (*it);

            if(tmpData->top <= (header->top + distanceTop))
                continue;

            tmpDistance = (tmpData->left > header->left) ?
                        tmpData->left - header->left :
                        header->left - tmpData->left;

            if(distance == -1)
            {
                distance = tmpDistance;
                txtData = tmpData;
            }
            else if(tmpDistance < distance)
            {
                distance = tmpDistance;
                txtData = tmpData;
            }
        }

        if(txtData == nullptr)
        {
            *value = "";
            return false;
        }
        else
        {
            *value = txtData->text;
            return true;
        }
    }
}

///
/// \brief GetInvoiceData
/// \param pageMap
/// \return
///
bool GetInvoiceData(QMap<int, sPAGE*>* pageMap)
{
    // List with possibles headers.
    QList<sTEXTDATA*> possibleHeaders;
    QList<sTEXTDATA*> possibleValues;

    QList<sINVOICEDATA*> invoiceList;

    QList<sTEXTDATA*> failed;

    // Temporary variables.
    sPAGE* page = nullptr;
    sTEXTDATA* textData = nullptr;
    sINVOICEDATA* invoiceData = nullptr;
    QString currentData;

    // Iterator of the page map.
    QMap<int, sPAGE*>::iterator itPage;

    // 0. Process each page.
    for(itPage = pageMap->begin(); itPage != pageMap->end(); ++itPage)
    {
        page = itPage.value();

        // 1. Let to get possibles headers.
        // We will discard texts that only have values.
        // Because it is more likely to get a possible header and try to read the value below the header.
        for(auto it = page->txtDataList.begin(); it != page->txtDataList.end(); ++it)
        {
            textData = (*it);

            if(CheckPossibleHeader(textData->text))
                possibleHeaders.push_back(textData);

            possibleValues.push_back(textData);
        }

        // Check if we got possibles header, then let to process.
        if(possibleHeaders.size() > 0)
        {
            // Now based on the possible header
            // We will try to get their values from the coordinates (top and left).
            for(auto it = possibleHeaders.begin(); it != possibleHeaders.end(); ++it)
            {
                textData = (*it);
                if(!textData || textData->text == "")
                    continue;

                // debug purpose.
                if(textData->text != "INSCRIÇÃO ESTADUAL")
                    continue;

                if(TryGetValue(textData, &possibleValues, &currentData, page->height, page->width))
                {
                    invoiceData = new sINVOICEDATA();
                    invoiceData->header = textData->text;
                    invoiceData->value = currentData;

                    invoiceList.push_back(invoiceData);
                }
                else
                {
                    failed.push_back(textData);
                }
            }
        }
        else
        {
            qDebug() << "Maybe error?";
            return false;
        }
    }

    return true;
}

///
/// \brief ReadInvoiceXML
/// \return
///
bool ReadInvoiceXML()
{
    QFile file("/home/litwin/MDS/PDF2CASH_PDFToInvoice/cpp/parser/nfe1.xml");
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "Cannot read file" << file.errorString();
        return false;
    }

    QMap<int, sPAGE*>* pageMap = new QMap<int, sPAGE*>();

    QXmlStreamReader reader(&file);

    // Temporary pointers.
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
                pageMap->insert(static_cast<int>(pageData->number), pageData);
            }
        }
    }

    // If loaded with successfuly, let to process invoice data.
    if(pageMap->size() > 0)
    {
        return GetInvoiceData(pageMap);
    }
    else
    {
        // Failed to load.
        qDebug() << "page map is null.";
        return false;
    }
}

