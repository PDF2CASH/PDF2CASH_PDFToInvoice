#pragma once

#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <QObject>

#include "qhttpserver.hpp"
#include "qhttpserverconnection.hpp"
#include "qhttpserverrequest.hpp"
#include "qhttpserverresponse.hpp"

#include "parser/Parser.h"

/** connection class for gathering incoming chunks of data from HTTP client.
 * @warning please note that the incoming request instance is the parent of
 * this QObject instance. Thus this class will be deleted automatically.
 * */
class ClientHandler : public QObject
{
public:
    explicit ClientHandler(quint64 id, qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res, pdf2cash::Parser* parser);
    virtual ~ClientHandler();

protected:
    quint64    iconnectionId;
};

#endif // CLIENTHANDLER_H
