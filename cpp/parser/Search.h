#ifndef SEARCH_H
#define SEARCH_H

#include <QString>

class Search
{
public:
    Search();

    QString Convert(QString str);

private:
    bool ToLowerCase(QString* str);
    bool RemoveAccents(QString* str);
    bool RemoveExtraCharacter(QString* str, const QChar c);

};

#endif // SEARCH_H
