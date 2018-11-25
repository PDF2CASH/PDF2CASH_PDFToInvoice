#include "CHandler.h"

#include <QDateTime>
#include <QLocale>

#include <QFile>
#include <QCryptographicHash>
#include <QDateTime>

using namespace qhttp::server;

QString generatePDFName(const QByteArray buffer)
{
    return "./" + QString(QCryptographicHash::hash((buffer),QCryptographicHash::Md5).toHex()) + ".pdf";
}

ClientHandler::ClientHandler(quint64 id, QHttpRequest* req, QHttpResponse* res, pdf2cash::Parser* parser)
    : QObject(req /* as parent*/), iconnectionId(id)
{
    req->collectData(/*8*1024*1024*/);

    //QByteArray prefix;
    //std::function<void(QByteArray)>lambda = [prefix] (QByteArray message) -> QByteArray
    //{
    //    printf("%s", message.toStdString().c_str());
    //    return message;
    //};

    // Test.
    //req->onData(lambda);

    //printf("Testing onData method: size received: %d.\n", chunk.size());
    //if(chunk.size() > 0)
    //{
    //    printf("Buffer from onData method:\n");
    //    printf("%s", chunk.toStdString().c_str());
    //}

    req->onEnd([req, res, parser]()
    {
        res->setStatusCode(qhttp::ESTATUS_OK);
        res->addHeader("connection", "close"); // optional header (added by default)

        int size = req->collectedData().size();

        // dump the incoming data into a file
        if ( size > 0 )
        {
            QString fileName = generatePDFName(req->collectedData());
            if(fileName == "")
            {
                fileName = QString(QDateTime::currentDateTime().toLocalTime().toTime_t()) + ".pdf";
            }

            QFile f(fileName);
            if ( f.open(QFile::WriteOnly) )
            {
                if(f.write(req->collectedData()))
                {
                    f.close();

                    if(parser->ProcessPDF(fileName))
                    {
                        if(parser->ReadInvoiceXML(fileName) && parser->GetInvoiceData())
                        {
                            // Function used for debug purpose.
                            //parser->DebugShow();

                            QString message = parser->ConvertToJsonBuffer();

                            res->setStatusCode(qhttp::ESTATUS_OK);
                            res->addHeaderValue("Access-Control-Allow-Origin", QString("*"));
                            res->addHeaderValue("content-length", message.size());
                            res->end(message.toUtf8());
                        }
                        else
                        {
                            printf("Failed to read xml or get invoice data.\n");
                            return "Failed to read xml or get invoice data.\n";
                        }
                    }
                    else
                    {
                        printf("Failed to process pdf.\n");
                    }

                    // remove pdf file.
                    QFile::remove(fileName);

                    // remove json file.
                    QFile::remove(fileName.replace(".pdf", ".xml"));

                    // remove json file.
                    QFile::remove(fileName.replace(".pdf", ".json"));

                }
                else
                {
                    f.close();

                    printf("Failed to writte PDF in disk.\n");
                    return "Failed to writte PDF in disk.\n";
                }
            }
            else
            {
                printf("Failed to open file.\n");
            }
        }
        else
        {
            printf("Received an invalid file.\n");
        }
    });
}

ClientHandler::~ClientHandler()
{
    //qDebug("  ~ClientHandler(#%llu): I've being called automatically!",
    //        iconnectionId
    //        );
}
