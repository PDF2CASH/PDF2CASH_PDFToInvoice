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
    req->collectData(8*1024*1024); // maximum 8MB of data for each post request
    // the better approach is to use req->onData(...)

    req->onEnd([req, res, parser]()
    {
        res->setStatusCode(qhttp::ESTATUS_OK);
        res->addHeader("connection", "close"); // optional header (added by default)

        int size = req->collectedData().size();
        //auto message = [size]() -> QByteArray
        //{
        //    if ( size == 0 )
        //        return "Received an invalid PDF or got some problem in to receive.\n";
        //
        //    char buffer[65] = {0};
        //    qsnprintf(buffer, 64, "Received PDF with %d bytes.\n", size);
        //    return buffer;
        //};
        //
        //res->end(message());  // response body data

        // dump the incoming data into a file
        if ( size > 0 )
        {
            QString fileName = generatePDFName(req->collectedData());
            if(fileName == "")
                fileName = QDateTime::currentDateTime().toString();

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
                            res->addHeaderValue("content-length", message.size());
                            res->end(message.toUtf8());
                        }
                        else
                        {
                            return "Failed to read xml or get invoice data.\n";
                        }
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
                    return "Failed to writte PDF in disk.\n";
                }
            }
        }
    });
}

ClientHandler::~ClientHandler()
{
    qDebug("  ~ClientHandler(#%llu): I've being called automatically!",
            iconnectionId
            );
}
