#include "parsercontroller.h"

#include <QFileInfo>
#include <QDir>

#include <QFile>
#include <QCryptographicHash>
#include <QDateTime>

#include "../parser/Parser.h"
#include "../parser/Utils.h"

using namespace stefanfrings;

ParserController::ParserController()
{

}

QString generatePDFName(const QByteArray buffer)
{
    QString fileName = "./" + QString(QCryptographicHash::hash((buffer), QCryptographicHash::Md5).toHex()) + QString::number(QDateTime::currentSecsSinceEpoch());
    return QString(fileName.toStdString().substr(fileName.toStdString().length()/2).c_str()) + ".pdf";
}

void ParserController::service(HttpRequest& request, HttpResponse& response, pdf2cash::Parser* parser)
{
    //response.setHeader("Content-Type", "application/json");
    //QTemporaryFile* file=request.getUploadedFile("test");
    //file=request.getUploadedFile("test.pdf");


    auto uploadedFiles = request.getUploadedFileMap();
    if(uploadedFiles.size() > 0)
    {
        pdf2cash::Parser* p = nullptr;
        QTemporaryFile* file = nullptr;
        QString fileName = "";

        QByteArray buffer;

        for(auto it = uploadedFiles.begin(); it != uploadedFiles.end(); ++it)
        {
            file = (*it);
            if(file != nullptr)
            {
                buffer = file->readAll();

                if(buffer.size() > 0)
                {
                    fileName = generatePDFName(buffer);

                    if(UtilsLit::FileExists(fileName))
                    {
                        QFile::remove(fileName);
                        QFile::remove(fileName.replace(".pdf", ".xml"));
                        QFile::remove(fileName.replace(".pdf", ".json"));
                    }

                    QFile pdf(fileName);

                    printf("\nFile name: %s\n", fileName.toStdString().c_str());

                    if ( pdf.open(QFile::WriteOnly) )
                    {
                        if(pdf.write(buffer))
                        {
                            pdf.flush();

                            p = new pdf2cash::Parser();

                            if(p->ProcessPDF(fileName))
                            {
                                if(p->ReadInvoiceXML(fileName) && p->GetInvoiceData())
                                {
                                    // Function used for debug purpose.
                                    //parser->DebugShow();

                                    QString message = p->ConvertToJsonBuffer();

                                    response.setStatus(200);
                                    response.setHeader("Access-Control-Allow-Origin", "*");
                                    response.setHeader("content-length", message.size());

                                    response.write(message.toStdString().c_str());
                                }
                                else
                                {
                                    printf("Failed to read xml or get invoice data.\n");
                                }
                            }

                            delete p;
                            //p->Clear();
                        }
                    }

                    if(pdf.isOpen())
                        pdf.close();

                    QFile::remove(fileName);
                    QFile::remove(fileName.replace(".pdf", ".xml"));
                    QFile::remove(fileName.replace(".pdf", ".json"));
                }


            }

            fileName = "";
        }
    }



    //if (file)
    //{
    //    while (!file->atEnd() && !file->error())
    //    {
    //        QByteArray buffer=file->read(65536);
    //        response.write(buffer);
    //    }
    //}
    //else
    //{
    //    response.write("upload failed");
    //}

    //QByteArray path=request.getPath();
    //// Check if we have the file in cache
    //qint64 now=QDateTime::currentMSecsSinceEpoch();
    //mutex.lock();
    //CacheEntry* entry=cache.object(path);
    //if (entry && (cacheTimeout==0 || entry->created>now-cacheTimeout))
    //{
    //    QByteArray document=entry->document; //copy the cached document, because other threads may destroy the cached entry immediately after mutex unlock.
    //    QByteArray filename=entry->filename;
    //    mutex.unlock();
    //    qDebug("ParserController: Cache hit for %s",path.data());
    //    setContentType(filename,response);
    //    response.setHeader("Cache-Control","max-age="+QByteArray::number(maxAge/1000));
    //    response.write(document);
    //}
    //else
    //{
    //    mutex.unlock();
    //    // The file is not in cache.
    //    qDebug("ParserController: Cache miss for %s",path.data());
    //    // Forbid access to files outside the docroot directory
    //    if (path.contains("/.."))
    //    {
    //        qWarning("ParserController: detected forbidden characters in path %s",path.data());
    //        response.setStatus(403,"forbidden");
    //        response.write("403 forbidden",true);
    //        return;
    //    }
    //    // If the filename is a directory, append index.html.
    //    if (QFileInfo(docroot+path).isDir())
    //    {
    //        path+="/index.html";
    //    }
    //    // Try to open the file
    //    QFile file(docroot+path);
    //    qDebug("ParserController: Open file %s",qPrintable(file.fileName()));
    //    if (file.open(QIODevice::ReadOnly))
    //    {
    //        setContentType(path,response);
    //        response.setHeader("Cache-Control","max-age="+QByteArray::number(maxAge/1000));
    //        if (file.size()<=maxCachedFileSize)
    //        {
    //            // Return the file content and store it also in the cache
    //            entry=new CacheEntry();
    //            while (!file.atEnd() && !file.error())
    //            {
    //                QByteArray buffer=file.read(65536);
    //                response.write(buffer);
    //                entry->document.append(buffer);
    //            }
    //            entry->created=now;
    //            entry->filename=path;
    //            mutex.lock();
    //            cache.insert(request.getPath(),entry,entry->document.size());
    //            mutex.unlock();
    //        }
    //        else
    //        {
    //            // Return the file content, do not store in cache
    //            while (!file.atEnd() && !file.error())
    //            {
    //                response.write(file.read(65536));
    //            }
    //        }
    //        file.close();
    //    }
    //    else {
    //        if (file.exists())
    //        {
    //            qWarning("ParserController: Cannot open existing file %s for reading",qPrintable(file.fileName()));
    //            response.setStatus(403,"forbidden");
    //            response.write("403 forbidden",true);
    //        }
    //        else
    //        {
    //            response.setStatus(404,"not found");
    //            response.write("404 not found",true);
    //        }
    //    }
    //}
}
