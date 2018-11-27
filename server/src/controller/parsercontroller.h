#ifndef PARSERCONTROLLER_H
#define PARSERCONTROLLER_H


#include <QCache>
#include <QMutex>
#include "httpglobal.h"
#include "httprequest.h"
#include "httpresponse.h"
#include "httprequesthandler.h"

#include "../parser/Parser.h"

namespace stefanfrings {

/**
  Delivers static files. It is usually called by the applications main request handler when
  the caller requests a path that is mapped to static files.
  <p>
  The following settings are required in the config file:
  <code><pre>
  path=../docroot
  encoding=UTF-8
  maxAge=60000
  cacheTime=60000
  cacheSize=1000000
  maxCachedFileSize=65536
  </pre></code>
  The path is relative to the directory of the config file. In case of windows, if the
  settings are in the registry, the path is relative to the current working directory.
  <p>
  The encoding is sent to the web browser in case of text and html files.
  <p>
  The cache improves performance of small files when loaded from a network
  drive. Large files are not cached. Files are cached as long as possible,
  when cacheTime=0. The maxAge value (in msec!) controls the remote browsers cache.
  <p>
  Do not instantiate this class in each request, because this would make the file cache
  useless. Better create one instance during start-up and call it when the application
  received a related HTTP request.
*/

class DECLSPEC ParserController : public HttpRequestHandler  {
    Q_OBJECT
    Q_DISABLE_COPY(ParserController)
public:

    /** Constructor */
    ParserController();

    /** Generates the response */
    void service(HttpRequest& request, HttpResponse& response, pdf2cash::Parser* parser);
};

} // end of namespace

#endif // PARSERCONTROLLER_H
