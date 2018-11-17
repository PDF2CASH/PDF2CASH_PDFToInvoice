#include <QCoreApplication>

#include "Utils.h"
#include "Parser.h"

#include <QElapsedTimer>

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

bool ProcessPDF(QString pdfFileName)
{
    GooString* fileName = nullptr;

    fileName = new GooString(pdfFileName.toStdString().c_str());

    //printf("Filename: %s\n", fileName->toStr().c_str());

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
        return false;
    }

    bool xml = true;

    //std::string s = getFileName(fileName->toStr());
    //s.erase(s.find_last_of("."), std::string::npos);

    htmlFileName = new GooString(pdfFileName.replace(".pdf", "").toStdString());

    //printf("test: %s\n", htmlFileName->toStr().c_str());

    // read config file
    globalParams = new GlobalParams();

    // convert from user-friendly percents into a coefficient
    wordBreakThreshold /= 100.0;

    if (scale>3.0) scale = 3.0;
    if (scale<0.5) scale = 0.5;

    if (complexMode || singleHtml)
    {
        //noframes=gFalse;
        stout = gFalse;
    }

    if (stout)
    {
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
    if (lastPage < firstPage)
    {
        error(errCommandLine, -1,
            "Wrong page range given: the first page ({0:d}) can not be after the last page ({1:d}).",
            firstPage, lastPage);
        //goto error;
    }

    info = doc->getDocInfo();
    if (info.isDict())
    {
        docTitle = Utils::getInfoString(info.getDict(), "Title");
        author = Utils::getInfoString(info.getDict(), "Author");
        keywords = Utils::getInfoString(info.getDict(), "Keywords");
        subject = Utils::getInfoString(info.getDict(), "Subject");
        date = Utils::getInfoDate(info.getDict(), "ModDate");
        if (!date)
            date = Utils::getInfoDate(info.getDict(), "CreationDate");
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
    else
    {
        printf("Some problem here.\n");
        return false;
    }

    delete htmlOut;

    if(doc) delete doc;

    return true;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

//    // TODO : DEBUG PURPOSE !
//    argc = 2;
//    argv = new char*[2];
//
//    //argv[1] = "/home/litwin/MDS/PDF2CASH_PDFToInvoice/cpp/parser/test.pdf";
//    //argv[1] = "/home/litwin/MDS/PDF2CASH_PDFToInvoice/cpp/build-parser-Desktop_Qt_5_11_2_GCC_64bit-Debug/pnc1.pdf";
//    argv[1] = "/home/litwin/MDS/PDF2CASH_PDFToInvoice/cpp/test.pdf";
//    // END -------------------------

    if(argc <= 1 || argc > 2)
    {
        printf("Incorrect way to execute program.\n");
        printf("PARAM: [program] [path pdf]\n.");

        return -1;
    }

    QString fileName(argv[1]);

    if(ProcessPDF(fileName))
    {
        QElapsedTimer timer;
        timer.start();
        int timeOutMS = 1000;
        long remainingTime = 0;

        // Just for give some delay in program for poppler library finish everything.
        for (;;)
        {
            remainingTime = timeOutMS - timer.elapsed();
            if(remainingTime <= 0)
                break;
        }

        Parser* parser = new Parser();
        if(parser == nullptr)
        {
            printf("Some problem happened.\n");
            return -1;
        }

        if(parser->ReadInvoiceXML(fileName) && parser->GetInvoiceData())
        {
            if(!parser->ConvertToJson())
            {
                printf("Failed to convert pdf to json.\n");
                return -1;
            }
        }
        else
        {
            printf("Failed to read xml or get invoice data.\n");
            return -1;
        }
    }
    else
    {
        printf("Error to process PDF.\n");
        return -1;
    }

    printf("Finished.\n");
    return 0 /*a.exec()*/;
}
