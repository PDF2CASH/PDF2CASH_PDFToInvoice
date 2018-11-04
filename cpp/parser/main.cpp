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

// debug purpose
#include <time.h>

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

    clock_t startTime = clock();

    ReadInvoiceXML();
    printf("\n%ld seconds have passed\n", (clock() - startTime) / CLOCKS_PER_SEC);

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
}

////////////////////////////////////////////////////////////////////////
/// --------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////

#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>

#include <QMap>
#include <QList>

#include <QRect>
#include <QPoint>
#include <QSize>

const int SPACE_HEIGHT_BETWEEN_HEADER_VALUE = 14;

enum eHEADER_NFE                            /// CABEÇALHO
{
    H_ACCESS_KEY = 0,                         // CHAVE DE ACESSO
    H_NATURE_OPERATION,                       // NATUREZA DA OPERAÇÃO
    H_PROTOCOL_AUTHORIZATION_USE,             // PROTOCOLO DE AUTORIZAÇÃO DE USO
    H_STATE_REGISTRATION,                     // INSCRIÇÃO ESTADUAL
    H_STATE_REGISTRATION_SUB_TAXATION,        // INSCRIÇÃO ESTADUAL SUB. TRIBUTARIA
    H_CNPJ,                                   // CNPJ

    H_MAX,
};

enum eADDRESSEE_SENDER                      /// DESTINATÁRIO/REMETENTE
{
    A_S_NAME_SOCIAL_REASON = 0,             // NOME/RAZÃO SOCIAL
    A_S_CPNJ_CPF,                           // CNPJ/CPF
    A_S_EMISSION_DATE,                      // DATA DA EMISSÃO
    A_S_ADDRESS,                            // ENDEREÇO
    A_S_NEIGHBORHOOD_DISTRICT,              // BAIRRO/DISTRITO
    A_S_CEP,                                // CEP
    A_S_OUTPUT_INPUT_DATE,                  // DATA DE SAÍDA/ENTRADA
    A_S_COUNTY,                             // MUNICÍPIO
    A_S_PHONE_FAX,                          // TELEFONE/FAX
    A_S_UF,                                 // UF
    A_S_STATE_REGISTRATION,                 // INSCRIÇÃO ESTADUAL
    A_S_EXIT_TIME,                          // HORA DE SAÍDA

    A_S_MAX,
};

enum eTAX_CALCULATION                       /// CÁLCULO DO IMPOSTO
{
    T_C_ICMS_CALCULATION_BASIS = 0,             // BASE DE CÁLCULO DE ICMS
    T_C_COST_ICMS,                          // VALOR DO ICMS
    T_C_CALCULATION_BASIS_ICMS_ST,          // BASE DE CÁLCULO ICMS ST
    T_C_VALUE_ICMS_REPLACEMENT,             // VALOR DO ICMS SUBSTITUIÇÃO
    T_C_TOTAL_VALUE_PRODUCTS,               // VALOR TOTAL DOS PRODUTOS
    T_C_COST_FREIGHT,                       // VALOR DO FRETE
    T_C_COST_INSURANCE,                     // VALOR DO SEGURO
    T_C_DISCOUNT,                           // DESCONTO
    T_C_OTHER_EXPENDITURE,                  // OUTRAS DESPESAS ACESSÓRIAS
    T_C_COST_IPI,                           // VALOR DO IPI
    T_C_APPROXIMATE_COST_TAXES,             // VALOR APROX. DOS TRIBUTOS
    T_C_COST_TOTAL_NOTE,                    // VALOR TOTAL DA NOTA

    T_C_MAX,
};

enum eCONVEYOR_VOLUMES                      /// TRANSPORTADOR/VOLUMES TRANSPORTADOS
{
    C_V_SOCIAL_REASON = 0,                      // RAZÃO SOCIAL
    C_V_FREIGHT_ACCOUNT,                    // FRETE POR CONTA
    C_V_CODE_ANTT,                          // CÓDIGO ANTT
    C_V_VEHICLE_PLATE,                      // PLACA DO VEÍCULO
    C_V_UF_1,                               // UF
    C_V_CNPJ_CPF,                           // CNPJ/CPF
    C_V_ADDRESS,                            // ENDEREÇO
    C_V_COUNTY,                             // MUNICÍPIO
    C_V_UF_2,                               // UF
    C_V_STATE_REGISTRATION,                 // INSCRIÇÃO ESTADUAL
    C_V_QUANTITY,                           // QUANTIDADE
    C_V_SPECIES,                            // ESPÉCIE
    C_V_MARK,                               // MARCA
    C_V_NUMBERING,                          // NUMERAÇÃO
    C_V_GROSS_WEIGHT,                       // PESO BRUTO
    C_V_NET_WEIGHT,                         // PESO LIQUIDO

    C_V_MAX,
};

enum eISSQN_CALCULATION                     /// CÁLCULO DO ISSQN
{
    I_C_MUNICIPAL_REGISTRATION = 0,             // INSCRIÇÃO MUNICIPAL
    I_C_TOTAL_COST_SERVICES,                // VALOR TOTAL DOS SERVIÇOS
    I_C_ISSQN_CALCULATION_BASE,             // BASE DE CALCULO DO ISSQN
    I_C_COST_ISSQN,                         // VALOR DO ISSQN

    I_C_MAX,
};

enum eINVOICE_HEADER
{
    MAIN = 0,
    ADDRESSEE_SENDER,                       // DESTINATÁRIO/REMETENTE
    TAX_CALCULATION,                        // CÁLCULO DO IMPOSTO
    CONVEYOR_VOLUMES,                       // TRANSPORTADOR/VOLUMES TRANSPORTADOS
    PRODUCT_SERVICE_DATA,                   // DADOS DO PRODUTO/SERVIÇO
    ISSQN_CALCULATION,                      // CÁLCULO DO ISSQN
    ADDITIONAL_DATA,                        // DADOS ADICIONAIS

    MAX,
};

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

struct sINVOICEHEADER
{
    eINVOICE_HEADER header;
    QRect rect;
};

struct sINVOICEDATA
{
    QString header;
    QString value;

    int headerID;
    int subHeaderID;
};

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
        qDebug() << "This shouldn't to happen.";
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

    // Optimize list, so you will not have to go through the entire list again
    // but only for possible values.
    QList<sTEXTDATA*> tmpPossibleValues;
    bool sameInRange = false;

    for(auto it = possibleValues->begin(); it != possibleValues->end(); ++it)
    {
        tmpData = (*it);

        if(tmpData->top < originalHeaderRect.top() ||
           tmpData->top > ((originalHeaderRect.top() + originalHeaderRect.height()) + (SPACE_HEIGHT_BETWEEN_HEADER_VALUE * 2)))
            continue;

        sameInRange = false;
        for(auto rangeIt = inRange.begin(); rangeIt != inRange.end(); ++rangeIt)
        {
            if((*rangeIt)->left == tmpData->left &&
               (*rangeIt)->top == tmpData->top)
            {
                sameInRange = true;
                break;
            }
        }

        if(sameInRange)
            continue;

        tmpPossibleValues.push_back(tmpData);
    }

    if(tmpPossibleValues.length() > 0)
    {
        possibleValues = &tmpPossibleValues;
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
                qDebug() << "This shouldn't to happen!";
                return headerRect;
            }
            else
            {
                // Calcule maximium left and check if this overflow with headerRect left.
                int newLeft = (tmpData->left + tmpData->width) + 1;

                int newSizeWidth = (originalHeaderRect.left() - newLeft) + originalHeaderRect.width();
                headerRect = QRect(QPoint(newLeft, originalHeaderRect.top()),
                                   QSize(newSizeWidth, originalHeaderRect.height() + SPACE_HEIGHT_BETWEEN_HEADER_VALUE));

                leftData = tmpData;
            }
        }
    }

    // Update distance.
    distance = maxPageWidth;
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
        // Temporary variable used for intersects.
        QRect tmpRect;

        QList<sTEXTDATA*> values;

        QRect rightRect(QPoint(rightData->left, rightData->top),
                        QSize(rightData->width, rightData->height));

        QRect testQuantityRect(QPoint(originalHeaderRect.left(), originalHeaderRect.top()),
                               QSize(originalHeaderRect.width() + (rightData->left - originalHeaderRect.left() - 1),
                                     originalHeaderRect.height() + SPACE_HEIGHT_BETWEEN_HEADER_VALUE));

        for(auto it = possibleValues->begin(); it != possibleValues->end(); ++it)
        {
            tmpData = (*it);

            // Check if is in same position, if yes, let ignore, because is our current header.
            if((tmpData->top == rightRect.y() && tmpData->left == rightRect.x()) ||
               (tmpData->top == originalHeaderRect.top() && tmpData->left == originalHeaderRect.left()))
                continue;

            tmpRect = QRect(QPoint(tmpData->left, tmpData->top),
                            QSize(tmpData->width, tmpData->height));

            if(testQuantityRect.intersects(tmpRect))
            {
                values.push_back(tmpData);
            }
        }

        if(values.length() == 1)
        {
            values.clear();
        }
        else
        {
            values.clear();

            int diffLeft = (rightData->left - (originalHeaderRect.left() + originalHeaderRect.width())) - 1;
            int newWidth = (originalHeaderRect.left() + originalHeaderRect.width()) + 1;

            // Update rect of the right.

            // TODO : find a better way to increase height than to use defined value 10.
            QRect newRightRect(QPoint(newWidth, rightData->top),
                               QSize(diffLeft, rightData->height + SPACE_HEIGHT_BETWEEN_HEADER_VALUE));

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
        }

        // -----------------------------------
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
            // TODO : Necessary to check some case than can happen here.
            qDebug() << "This shouldn't to happen!";
            return headerRect;
        }
    }

    return headerRect;
}

///
/// \brief TryGetValue
/// \param header
/// \param possibleValues
/// \param value
/// \param maxPageHeight
/// \param maxPageWidth
/// \return
///
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
        return true;
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

bool FindValueData(QString value, QList<sTEXTDATA*> list, QRect* rect)
{
    sTEXTDATA* data;

    QList<sTEXTDATA*> find;

    for(auto it = list.begin(); it != list.end(); ++it)
    {
        data = (*it);
        if(data != nullptr && data->text == value)
        {
            find.push_back(data);
        }
    }

    if(find.length() == 0)
    {
        *rect = QRect(QPoint(-1, -1), QSize(-1, -1));
        return true;
    }
    else if(find.length() == 1)
    {
        data = find.first();

        *rect = QRect(QPoint(data->left, data->top), QSize(data->width, data->height));
        return true;
    }
    else
    {
        // TODO:
    }

    return false;
}

QList<QString> ConvertEnumToText(eINVOICE_HEADER header, int value = -1)
{
    QList<QString> list;

    switch(header)
    {
    case MAIN:
    {
        switch(value)
        {
            case H_ACCESS_KEY:
            {
                list.push_back("CHAVE DE ACESSO");
                return list;
            }
            case H_NATURE_OPERATION:
            {
                list.push_back("NATUREZA DA OPERAÇÃO");
                return list;
            }
            case H_PROTOCOL_AUTHORIZATION_USE:
            {
                list.push_back("PROTOCOLO DE AUTORIZAÇÃO DE USO");
                return list;
            }
            case H_STATE_REGISTRATION:
            {
                list.push_back("INSCRIÇÃO ESTADUAL");
                return list;
            }
            case H_STATE_REGISTRATION_SUB_TAXATION:
            {
                list.push_back("INSCRIÇÃO ESTADUAL SUB. TRIBUTARIA");
                return list;
            }
            case H_CNPJ:
            {
                list.push_back("CNPJ");
                return list;
            }
        }
    }
    break;
    case ADDRESSEE_SENDER:
    {
        switch(value)
        {
        case A_S_NAME_SOCIAL_REASON:
        {
            list.push_back("NOME/RAZÃO SOCIAL");
            return list;
        }
        case A_S_CPNJ_CPF:
        {
            list.push_back("CNPJ/CPF");
            return list;
        }
        case A_S_EMISSION_DATE:
        {
            list.push_back("DATA DA EMISSÃO");
            return list;
        }
        case A_S_ADDRESS:
        {
            list.push_back("ENDEREÇO");
            return list;
        }
        case A_S_NEIGHBORHOOD_DISTRICT:
        {
            list.push_back("BAIRRO/DISTRITO");
            return list;
        }
        case A_S_CEP:
        {
            list.push_back("CEP");
            return list;
        }
        case A_S_OUTPUT_INPUT_DATE:
        {
            list.push_back("DATA DE SAÍDA/ENTRADA");
            return list;
        }
        case A_S_COUNTY:
        {
            list.push_back("MUNICÍPIO");
            return list;
        }
        case A_S_PHONE_FAX:
        {
            list.push_back("TELEFONE/FAX");
            return list;
        }
        case A_S_UF:
        {
            list.push_back("UF");
            return list;
        }
        case A_S_STATE_REGISTRATION:
        {
            list.push_back("INSCRIÇÃO ESTADUAL");
            return list;
        }
        case A_S_EXIT_TIME:
        {
            list.push_back("HORA DE SAÍDA");
            return list;
        }
        default:
        {
            list.push_back("DESTINATÁRIO/REMETENTE");
            list.push_back("DESTINATÁRIO / REMETENTE");
            return list;
        }
        }
    }
    //break;
    case TAX_CALCULATION:
    {
        switch(value)
        {
        case T_C_ICMS_CALCULATION_BASIS:
        {
            list.push_back("BASE DE CÁLCULO DE ICMS");
            return list;
        }
        case T_C_COST_ICMS:
        {
            list.push_back("VALOR DO ICMS");
            return list;
        }
        case T_C_CALCULATION_BASIS_ICMS_ST:
        {
            list.push_back("BASE DE CÁLCULO ICMS ST");
            return list;
        }
        case T_C_VALUE_ICMS_REPLACEMENT:
        {
            list.push_back("VALOR DO ICMS SUBSTITUIÇÃO");
            return list;
        }
        case T_C_TOTAL_VALUE_PRODUCTS:
        {
            list.push_back("VALOR TOTAL DOS PRODUTOS");
            return list;
        }
        case T_C_COST_FREIGHT:
        {
            list.push_back("VALOR DO FRETE");
            return list;
        }
        case T_C_COST_INSURANCE:
        {
            list.push_back("VALOR DO SEGURO");
            return list;
        }
        case T_C_DISCOUNT:
        {
            list.push_back("DESCONTO");
            return list;
        }
        case T_C_OTHER_EXPENDITURE:
        {
            list.push_back("OUTRAS DESPESAS ACESSÓRIAS");
            return list;
        }
        case T_C_COST_IPI:
        {
            list.push_back("VALOR DO IPI");
            return list;
        }
        case T_C_APPROXIMATE_COST_TAXES:
        {
            list.push_back("VALOR APROX. DOS TRIBUTOS");
            return list;
        }
        case T_C_COST_TOTAL_NOTE:
        {
            list.push_back("VALOR TOTAL DA NOTA");
            return list;
        }
        default:
        {
            list.push_back("CÁLCULO DO IMPOSTO");
            return list;
        }
        }
    }
    //break;
    case CONVEYOR_VOLUMES:
    {
        switch(value)
        {
        case C_V_SOCIAL_REASON:
        {
            list.push_back("RAZÃO SOCIAL");
            return list;
        }
        case C_V_FREIGHT_ACCOUNT:
        {
            list.push_back("FRETE POR CONTA");
            return list;
        }
        case C_V_CODE_ANTT:
        {
            list.push_back("CÓDIGO ANTT");
            return list;
        }
        case C_V_VEHICLE_PLATE:
        {
            list.push_back("PLACA DO VEÍCULO");
            return list;
        }
        case C_V_UF_1:
        {
            list.push_back("UF");
            return list;
        }
        case C_V_CNPJ_CPF:
        {
            list.push_back("CNPJ/CPF");
            return list;
        }
        case C_V_ADDRESS:
        {
            list.push_back("ENDEREÇO");
            return list;
        }
        case C_V_COUNTY:
        {
            list.push_back("MUNICÍPIO");
            return list;
        }
        case C_V_UF_2:
        {
            list.push_back("UF");
            return list;
        }
        case C_V_STATE_REGISTRATION:
        {
            list.push_back("INSCRIÇÃO ESTADUAL");
            return list;
        }
        case C_V_QUANTITY:
        {
            list.push_back("QUANTIDADE");
            return list;
        }
        case C_V_SPECIES:
        {
            list.push_back("ESPÉCIE");
            return list;
        }
        case C_V_MARK:
        {
            list.push_back("MARCA");
            return list;
        }
        case C_V_NUMBERING:
        {
            list.push_back("NUMERAÇÃO");
            return list;
        }
        case C_V_GROSS_WEIGHT:
        {
            list.push_back("PESO BRUTO");
            return list;
        }
        case C_V_NET_WEIGHT:
        {
            list.push_back("PESO LIQUIDO");
            return list;
        }
        default:
        {
            list.push_back("TRANSPORTADOR/VOLUMES TRANSPORTADOS");
            return list;
        }
        }
    }
    //break;
    case PRODUCT_SERVICE_DATA:
    {
        list.push_back("DADOS DO PRODUTO/SERVIÇO");
        return list;
    }
    //break;
    case ISSQN_CALCULATION:
    {
        switch(value)
        {
        case I_C_MUNICIPAL_REGISTRATION:
        {
            list.push_back("INSCRIÇÃO MUNICIPAL");
            return list;
        }
        case I_C_TOTAL_COST_SERVICES:
        {
            list.push_back("VALOR TOTAL DOS SERVIÇOS");
            return list;
        }
        case I_C_ISSQN_CALCULATION_BASE:
        {
            list.push_back("BASE DE CALCULO DO ISSQN");
            return list;
        }
        case I_C_COST_ISSQN:
        {
            list.push_back("VALOR DO ISSQN");
            return list;
        }
        default:
        {
            list.push_back("CÁLCULO DO ISSQN");
            return list;
        }
        }
    }
    //break;
    case ADDITIONAL_DATA:
    {
        list.push_back("DADOS ADICIONAIS");
        return list;
    }
    }

    return list;
}

sINVOICEHEADER* GetInvoiceHeader(QList<sINVOICEHEADER*> list, int value)
{
    for(auto it = list.begin(); it != list.end(); ++it)
    {
        if((*it) && (*it)->header == value)
            return (*it);
    }

    return nullptr;
}

QList<sINVOICEHEADER*> ProcessInvoiceHeader(QList<sTEXTDATA*> possibleHeaders, int maxWidth, int maxHeight)
{
    QList<sINVOICEHEADER*> invoiceHeaderList;

    sINVOICEHEADER* invoiceHeader = nullptr;
    sINVOICEHEADER* tmpHeader = nullptr;

    QRect tmpRect;

    // Process header rects.
    for(int header = MAX - 1; header >= MAIN; header--)
    {
        switch(header)
        {
            case ADDITIONAL_DATA:
            {
                invoiceHeader = new sINVOICEHEADER();
                invoiceHeader->header = ADDITIONAL_DATA;

                if(FindValueData(ConvertEnumToText(invoiceHeader->header)[0], possibleHeaders, &tmpRect))
                {
                    tmpRect = QRect(QPoint(0 /*tmpRect.left()*/, tmpRect.top()),
                                    QSize(maxWidth, maxHeight - tmpRect.top()));

                    invoiceHeader->rect = tmpRect;

                    invoiceHeaderList.push_back(invoiceHeader);
                }
            }
            break;
            case ISSQN_CALCULATION:
            {
                tmpHeader = GetInvoiceHeader(invoiceHeaderList, ADDITIONAL_DATA);
                if(tmpHeader == nullptr)
                    continue;

                invoiceHeader = new sINVOICEHEADER();
                invoiceHeader->header = ISSQN_CALCULATION;

                if(FindValueData(ConvertEnumToText(invoiceHeader->header)[0], possibleHeaders, &tmpRect))
                {
                    tmpRect = QRect(QPoint(0 /*tmpRect.left()*/, tmpRect.top()),
                                    QSize(maxWidth, (tmpHeader->rect.top() - 1) - tmpRect.top()));

                    invoiceHeader->rect = tmpRect;

                    invoiceHeaderList.push_back(invoiceHeader);
                }
            }
            break;
            case PRODUCT_SERVICE_DATA:
            {
                tmpHeader = GetInvoiceHeader(invoiceHeaderList, ISSQN_CALCULATION);
                if(tmpHeader == nullptr)
                    continue;

                invoiceHeader = new sINVOICEHEADER();
                invoiceHeader->header = PRODUCT_SERVICE_DATA;

                if(FindValueData(ConvertEnumToText(invoiceHeader->header)[0], possibleHeaders, &tmpRect))
                {
                    tmpRect = QRect(QPoint(0 /*tmpRect.left()*/, tmpRect.top()),
                                    QSize(maxWidth, (tmpHeader->rect.top() - 1) - tmpRect.top()));

                    invoiceHeader->rect = tmpRect;

                    invoiceHeaderList.push_back(invoiceHeader);
                }
            }
            break;
            case CONVEYOR_VOLUMES:
            {
                tmpHeader = GetInvoiceHeader(invoiceHeaderList, PRODUCT_SERVICE_DATA);
                if(tmpHeader == nullptr)
                    continue;

                invoiceHeader = new sINVOICEHEADER();
                invoiceHeader->header = CONVEYOR_VOLUMES;

                if(FindValueData(ConvertEnumToText(invoiceHeader->header)[0], possibleHeaders, &tmpRect))
                {
                    tmpRect = QRect(QPoint(0 /*tmpRect.left()*/, tmpRect.top()),
                                    QSize(maxWidth, (tmpHeader->rect.top() - 1) - tmpRect.top()));

                    invoiceHeader->rect = tmpRect;

                    invoiceHeaderList.push_back(invoiceHeader);
                }
            }
            break;
            case TAX_CALCULATION:
            {
                tmpHeader = GetInvoiceHeader(invoiceHeaderList, CONVEYOR_VOLUMES);
                if(tmpHeader == nullptr)
                    continue;

                invoiceHeader = new sINVOICEHEADER();
                invoiceHeader->header = TAX_CALCULATION;

                if(FindValueData(ConvertEnumToText(invoiceHeader->header)[0], possibleHeaders, &tmpRect))
                {
                    tmpRect = QRect(QPoint(0 /*tmpRect.left()*/, tmpRect.top()),
                                    QSize(maxWidth, (tmpHeader->rect.top() - 1) - tmpRect.top()));

                    invoiceHeader->rect = tmpRect;

                    invoiceHeaderList.push_back(invoiceHeader);
                }
            }
            break;
            case ADDRESSEE_SENDER:
            {
                tmpHeader = GetInvoiceHeader(invoiceHeaderList, TAX_CALCULATION);
                if(tmpHeader == nullptr)
                    continue;

                invoiceHeader = new sINVOICEHEADER();
                invoiceHeader->header = ADDRESSEE_SENDER;

                if(FindValueData(ConvertEnumToText(invoiceHeader->header)[0], possibleHeaders, &tmpRect))
                {
                    tmpRect = QRect(QPoint(0 /*tmpRect.left()*/, tmpRect.top()),
                                    QSize(maxWidth, (tmpHeader->rect.top() - 1) - tmpRect.top()));

                    invoiceHeader->rect = tmpRect;

                    invoiceHeaderList.push_back(invoiceHeader);
                }
            }
            break;
            case MAIN:
            {
                tmpHeader = GetInvoiceHeader(invoiceHeaderList, ADDRESSEE_SENDER);
                if(tmpHeader == nullptr)
                    continue;

                invoiceHeader = new sINVOICEHEADER();
                invoiceHeader->header = MAIN;

                tmpRect = QRect(QPoint(0 /*tmpRect.left()*/, 0 /*tmpRect.top()*/),
                                QSize(maxWidth, (tmpHeader->rect.top() - 1)/* - tmpRect.top()*/));

                invoiceHeader->rect = tmpRect;

                invoiceHeaderList.push_back(invoiceHeader);
            }
            break;
        }
    }

    return invoiceHeaderList;
}

sTEXTDATA* GetTextData(QString header, QList<sTEXTDATA*> possibleValues)
{
    sTEXTDATA* tmpData = nullptr;
    for(auto it = possibleValues.begin(); it != possibleValues.end(); ++it)
    {
        tmpData = (*it);
        if(tmpData != nullptr)
        {
            if(tmpData->text == header)
            {
                return tmpData;
            }
        }
    }

    return nullptr;
}

int GetMaxDataHeader(int header)
{
    switch(header)
    {
        case MAIN: return H_MAX;
        case ADDRESSEE_SENDER: return A_S_MAX;
        case TAX_CALCULATION: return T_C_MAX;
        case CONVEYOR_VOLUMES: return C_V_MAX;
        case PRODUCT_SERVICE_DATA: return 0;
        case ISSQN_CALCULATION: return I_C_MAX;
        default: return 0;
    }
}

void DebugShow(QMap<int, QList<sINVOICEDATA*>> map)
{
    QString tmpStr;
    sINVOICEDATA* tmpInvoice = nullptr;

    for(auto it = map.begin(); it != map.end(); ++it)
    {
        switch(it.key())
        {
            case MAIN: tmpStr = "NF-E"; break;
            case ADDRESSEE_SENDER: tmpStr = "DESTINATÁRIO/REMETENTE"; break;
            case TAX_CALCULATION: tmpStr = "CÁLCULO DO IMPOSTO"; break;
            case CONVEYOR_VOLUMES: tmpStr = "TRANSPORTADOR/VOLUMES TRANSPORTADOS"; break;
            case PRODUCT_SERVICE_DATA: tmpStr = "DADOS DO PRODUTO/SERVIÇO"; break;
            case ISSQN_CALCULATION: tmpStr = "CÁLCULO DO ISSQN"; break;
        }

        printf("=============================================================================\n");
        printf("%s\n", tmpStr.toStdString().c_str());
        printf("=============================================================================\n");

        for(auto it2 = (*it).begin(); it2 != (*it).end(); ++it2)
        {
            tmpInvoice = (*it2);
            printf("[%s]\n", tmpInvoice->header.toStdString().c_str());
            printf("%s\n", tmpInvoice->value.toStdString().c_str());
        }

        printf("\n");
    }
}

QString ConvertToJsonHeader(int header, int value)
{
    QString tmpStr;

    switch(header)
    {
    case MAIN:
    {
        switch(value)
        {
            case H_ACCESS_KEY: return "main_access_key";
            case H_NATURE_OPERATION: return "main_nature_operation";
            case H_PROTOCOL_AUTHORIZATION_USE: return "main_protocol_authorization_use";
            case H_STATE_REGISTRATION: return "main_state_registration";
            case H_STATE_REGISTRATION_SUB_TAXATION: return "main_state_registration_sub_tax";
            case H_CNPJ: return "main_cnpj";
        }
    }
    break;
    case ADDRESSEE_SENDER:
    {
        switch(value)
        {
        case A_S_NAME_SOCIAL_REASON: return "sender_name_social";
        case A_S_CPNJ_CPF: return "sender_cnpj_cpf";
        case A_S_EMISSION_DATE: return "sender_emission_date";
        case A_S_ADDRESS: return "sender_address";
        case A_S_NEIGHBORHOOD_DISTRICT: return "sender_neighborhood_district";
        case A_S_CEP: return "sender_cep";
        case A_S_OUTPUT_INPUT_DATE: return "sender_out_input_date";
        case A_S_COUNTY: return "sender_county";
        case A_S_PHONE_FAX: return "sender_phone_fax";
        case A_S_UF: return "sender_uf";
        case A_S_STATE_REGISTRATION: return "sender_state_registration";
        case A_S_EXIT_TIME: return "sender_output_time";
        }
    }
    break;
    case TAX_CALCULATION:
    {
        switch(value)
        {
        case T_C_ICMS_CALCULATION_BASIS: return "tax_icms_basis";
        case T_C_COST_ICMS: return "tax_cost_icms";
        case T_C_CALCULATION_BASIS_ICMS_ST: return "tax_icms_basis_st";
        case T_C_VALUE_ICMS_REPLACEMENT: return "tax_cost_icms_replacement";
        case T_C_TOTAL_VALUE_PRODUCTS: return "tax_total_cost_products";
        case T_C_COST_FREIGHT: return "tax_cost_freight";
        case T_C_COST_INSURANCE: return "tax_cost_insurance";
        case T_C_DISCOUNT: return "tax_discount";
        case T_C_OTHER_EXPENDITURE: return "tax_other_expenditure";
        case T_C_COST_IPI: return "tax_cost_ipi";
        case T_C_APPROXIMATE_COST_TAXES: return "tax_approx_cost_taxes";
        case T_C_COST_TOTAL_NOTE: return "tax_cost_total_note";
        }
    }
    break;
    case CONVEYOR_VOLUMES:
    {
        switch(value)
        {
        case C_V_SOCIAL_REASON: return "conveyor_social_reason";
        case C_V_FREIGHT_ACCOUNT: return "conveyor_freight_account";
        case C_V_CODE_ANTT: return "conveyor_code_antt";
        case C_V_VEHICLE_PLATE: return "conveyor_vehicle_plate";
        case C_V_UF_1: return "conveyor_uf_1";
        case C_V_CNPJ_CPF: return "conveyor_cnpj_cpf";
        case C_V_ADDRESS: return "conveyor_address";
        case C_V_COUNTY: return "conveyor_county";
        case C_V_UF_2: return "conveyor_uf_2";
        case C_V_STATE_REGISTRATION: return "conveyor_state_registration";
        case C_V_QUANTITY: return "conveyor_quantity";
        case C_V_SPECIES: return "conveyor_species";
        case C_V_MARK: return "conveyor_mark";
        case C_V_NUMBERING: return "conveyor_numering";
        case C_V_GROSS_WEIGHT: return "conveyor_gross_weight";
        case C_V_NET_WEIGHT: return "conveyor_net_weight";
        }
    }
    break;
    case PRODUCT_SERVICE_DATA:
    {
        return "product_service_data";
    }
    break;
    case ISSQN_CALCULATION:
    {
        switch(value)
        {
        case I_C_MUNICIPAL_REGISTRATION: return "issqn_municial_registration";
        case I_C_TOTAL_COST_SERVICES: return "issqn_total_cost_services";
        case I_C_ISSQN_CALCULATION_BASE: return "issqn_calculation_base";
        case I_C_COST_ISSQN: return "issqn_cost";
        }
    }
    break;
    case ADDITIONAL_DATA:
    {
        return "additional_data";
    }
    break;
    }

    return "";
}

QString GenerateJson(QMap<int, QList<sINVOICEDATA*>> map)
{
    QString json("");

    QString tmpStr;
    sINVOICEDATA* tmpInvoice = nullptr;

    json += "{\n";

    for(auto it = map.begin(); it != map.end(); ++it)
    {
        for(auto it2 = (*it).begin(); it2 != (*it).end(); ++it2)
        {
            tmpInvoice = (*it2);
            if(tmpInvoice != nullptr)
            {
                tmpStr = ConvertToJsonHeader(tmpInvoice->headerID, tmpInvoice->subHeaderID);
                if(tmpStr.isEmpty())
                    continue;

                json += "\t";
                json += "\"";
                json += tmpStr;
                json += "\"";
                json += ": ";

                if(tmpInvoice->value.isEmpty())
                {
                    json += "null";
                    json += ",\n";
                }
                else
                {
                    if(IsNumber(tmpInvoice->value))
                    {
                        json += tmpInvoice->value;
                        json += ",\n";
                    }
                    else
                    {
                        json += "\"";
                        json += tmpInvoice->value;
                        json += "\"";
                        json += ",\n";
                    }
                }
            }
        }
    }

    json += "}";

    // TODO : Debug purpose.
    printf("%s\n", json.toStdString().c_str());

    return json;
}

///
/// \brief GetInvoiceData
/// \param pageMap
/// \return
///
bool GetInvoiceData(QMap<int, sPAGE*>* pageMap)
{
    // List with possibles headers.
    QList<sTEXTDATA*> headers;
    QList<sTEXTDATA*> values;
    QMap<int, QList<sTEXTDATA*>> valuesMap;
    QMap<int, QList<sINVOICEDATA*>> invoicesMap;
    QList<sINVOICEHEADER*> invoiceHeaderList;

    QList<sTEXTDATA*> failed;

    // Temporary variables.
    sPAGE* page = nullptr;
    sTEXTDATA* textData = nullptr;
    sINVOICEDATA* invoiceData = nullptr;
    QString currentData;
    QString tmpStr;

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

            // Process all the data obtained in the XML and add only
            // data that has a number of characters larger than numbers.
            if(CheckPossibleHeader(textData->text))
            {
                headers.push_back(textData);
            }

            values.push_back(textData);
        }

        // Check if we got possibles header, then let to process.
        if(headers.size() <= 0)
        {
            qDebug() << "Maybe error?";
            continue;
        }

        invoiceHeaderList = ProcessInvoiceHeader(values, page->width, page->height);

        // Check if we got possibles header.
        if(invoiceHeaderList.size() <= 0)
        {
            qDebug() << "Maybe error?";
            continue;
        }

        QList<sTEXTDATA*> tmpTxtList;
        QList<sINVOICEDATA*> tmpInvoiceList;
        sINVOICEHEADER* tmpHeader = nullptr;
        QRect tmpRect;
        QList<QString> tmpStringList;
        int forMax = 0;

        for(int i = MAIN; i < MAX; i++)
        {
            tmpTxtList.clear();
            tmpHeader = nullptr;

            for(auto it = invoiceHeaderList.begin(); it != invoiceHeaderList.end(); ++it)
            {
                if((*it) && (*it)->header == i)
                {
                    tmpHeader = (*it);
                    break;
                }
            }

            if(tmpHeader == nullptr)
                continue;

            for(auto it = values.begin(); it != values.end(); ++it)
            {
                textData = (*it);

                tmpRect = QRect(QPoint(textData->left, textData->top),
                                QSize(textData->width, textData->height));

                if(tmpHeader->rect.intersects(tmpRect))
                {
                    tmpTxtList.push_back(textData);
                }
            }

            valuesMap.insert(i, tmpTxtList);
        }

        if(valuesMap.size() <= 0)
        {
            qDebug() << "Maybe error?";
            continue;
        }

        // Let to find values from headers.
        for(int i = MAIN; i < MAX; i++)
        {
            tmpInvoiceList.clear();

            if(i == PRODUCT_SERVICE_DATA)
                continue;

            forMax = GetMaxDataHeader(i);
            if(forMax == 0)
                continue;

            for(int k = 0; k < forMax; k++)
            {
                tmpStringList = ConvertEnumToText(static_cast<eINVOICE_HEADER>(i), k);
                if(tmpStringList.length() <= 0)
                {
                    qDebug() << "Maybe error?";
                    continue;
                }

                tmpTxtList = valuesMap[i];
                for(auto it = tmpStringList.begin(); it != tmpStringList.end(); ++it)
                {
                    tmpStr = (*it);
                    textData = GetTextData(tmpStr, tmpTxtList);

                    if(textData != nullptr)
                    {
                        if(TryGetValue(textData, &tmpTxtList, &currentData, page->height, page->width))
                        {
                            invoiceData = new sINVOICEDATA();
                            invoiceData->header = textData->text;
                            invoiceData->value = currentData;
                            invoiceData->headerID = i;
                            invoiceData->subHeaderID = k;

                            tmpInvoiceList.push_back(invoiceData);
                            break;
                        }
                        else
                        {
                            failed.push_back(textData);
                        }
                    }
                }
            }

            invoicesMap.insert(i, tmpInvoiceList);
        }
    }

    DebugShow(invoicesMap);
    //GenerateJson(invoicesMap);

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

