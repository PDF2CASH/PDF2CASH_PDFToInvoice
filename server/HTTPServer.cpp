#include <QCoreApplication>

#include "qhttpserverrequest.hpp"
#include "qhttpserverresponse.hpp"

#include "CHandler.h"
#include "parser/Parser.h"

#if defined(Q_OS_UNIX)
#include <signal.h>
#include <unistd.h>
#endif // Q_OS_UNIX

#include <QCryptographicHash>
#include <QDateTime>

void messageInitialization()
{
    printf("\t\t\t ____   _    ____  ____  _____ ____  \n");
    printf("\t\t\t|  _ \\ / \\  |  _ \\/ ___|| ____|  _ \\ \n");
    printf("\t\t\t| |_) / _ \\ | |_) \\___ \\|  _| | |_) |\n");
    printf("\t\t\t|  __/ ___ \\|  _ < ___) | |___|  _ < \n");
    printf("\t\t\t|_| /_/   \\_|_| \\_|____/|_____|_| \\_\\ \n");

    printf("\n> Server is started at [%s].\n", QDateTime::currentDateTime().toString().toUpper().toStdString().c_str());
    printf("> To exit the server, click CTRL + C.\n\n");
}

int main(int argc, char ** argv)
{
    QCoreApplication app(argc, argv);

#if defined(Q_OS_UNIX)
    const std::vector<int>& quitSignals = {SIGQUIT, SIGINT, SIGTERM, SIGHUP};
    const std::vector<int>& ignoreSignals = std::vector<int>();

    auto handler = [](int sig) ->void {
        printf("\nquit the application (user request signal = %d).\n", sig);
        QCoreApplication::quit();
    };

    for ( int sig : ignoreSignals )
        signal(sig, SIG_IGN);

    for ( int sig : quitSignals )
        signal(sig, handler);
#endif

    messageInitialization();

    // dumb (trivial) connection counter
    quint64 iconnectionCounter = 0;

    QString portOrUnixSocket("10022"); // default: TCP port 10022

    pdf2cash::Parser* parser = new pdf2cash::Parser();
    if(parser == nullptr)
    {
        printf("Failed to initialize Parser.\n");
        return -1;
    }

    qhttp::server::QHttpServer server(&app);
    server.listen(portOrUnixSocket, [&](qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res)
    {
        new ClientHandler(++iconnectionCounter, req, res, parser);
        // this ClientHandler object will be deleted automatically when:
        // socket disconnects (automatically after data has been sent to the client)
        //     -> QHttpConnection destroys
        //         -> QHttpRequest destroys -> ClientHandler destroys
        //         -> QHttpResponse destroys
        // all by parent-child model of QObject.
    });

    if ( !server.isListening() )
    {
        fprintf(stderr, "Can not listen on port [%s].\n", qPrintable(portOrUnixSocket));
        return -1;
    }

    app.exec();
}
