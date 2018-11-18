#ifndef SEARCH_H
#define SEARCH_H

#include <QString>

class Search
{
public:
    Search();

    QString Convert(QString str);

private:
    QString ToLowerCase(QString str);
    QString RemoveAccents(QString str);
};

#endif // SEARCH_H
