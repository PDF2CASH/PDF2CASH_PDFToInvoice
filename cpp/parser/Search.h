#ifndef SEARCH_H
#define SEARCH_H

#include <QString>
#include <QStringList>

#include <QMap>
#include <QList>

class Search
{
public:
    Search();

    void Initialization();

    QString Convert(QString str);
    QStringList Split(QString str);

private:
    bool ToLowerCase(QString* str);
    bool RemoveAccents(QString* str);
    bool RemoveSpecialCharacter(QString* str);
    bool RemoveExtraCharacter(QString* str, const QChar c);

    bool RemoveAbbreviation(QString* str);

private:
    QMap<QString, QList<QString>> _abbreviationsMap;
};

#endif // SEARCH_H
