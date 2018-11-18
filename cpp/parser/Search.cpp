#include "Search.h"

#include <QChar>

Search::Search()
{

}

QString Search::Convert(QString str)
{
    if(str.isEmpty() || str.isNull())
        return "";

    QString data = "";

    // 1. convert to lower.
    data = ToLowerCase(str);

    return data;
}

QString Search::ToLowerCase(QString str)
{
    return str.toLower();
}

QString Search::RemoveAccents(QString str)
{
    QChar c;

    for(auto i = 0; i < str.length(); i++)
    {
        c = str.at(i);

        if(c.isNull())
            continue;

        if(c == "â" || c == "ã" || c == "á" || c == "à" || c == "ä")
        {
            str.insert(i, "a");
        }
        else if(c == "é" || c == "è" || c == "ê" || c == "ë")
        {
            str.insert(i, "e");
        }
        else if(c == "í" || c == "ì" || c == "î" || c == "ï")
        {
            str.insert(i, "i");
        }
        else if(c == "ó" || c == "ò" || c == "ô" || c == "õ" || c == "ö")
        {
            str.insert(i, "o");
        }
        else if(c == "ú" || c == "ù" || c == "û" || c == "ü")
        {
            str.insert(i, "u");
        }
        else
        {
            continue;
        }
    }

    return str;
}
