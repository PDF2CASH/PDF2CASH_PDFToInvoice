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

    // 2. remove all accents from string (case has accents).
    data = RemoveAccents(data);

    // 3. remove extra spaces on strings, example: "Str1  Str2" -> "Str1 Str2".
    data = RemoveExtraSpaces(data);

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
        else if(c == "ç" || c == "ć")
        {
            str.insert(i, "c");
        }
        else
        {
            continue;
        }
    }

    return str;
}

QString Search::RemoveExtraSpaces(QString str)
{
    QString data = "";
    std::unique_copy(str.begin(), str.end(),
                     std::back_insert_iterator<QString>(data),
                    [](QChar a, QChar b)
                    {
                        return a.isSpace() && b.isSpace();
                    });

    return data;
}
