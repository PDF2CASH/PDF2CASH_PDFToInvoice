#include "Search.h"

#include <QChar>
#include <QRegExp>

Search::Search()
{
    Initialization();
}

void Search::Initialization()
{
    // Telefone.
    _abbreviationsMap.insert(Convert("telefone", false),
                             QList<QString>
                             {
                                 "fone",
                                 "tel"
                             });

    // Substituição
    _abbreviationsMap.insert(Convert("substituição", false),
                             QList<QString>
                             {
                                 "st",
                                 "subst",
                                 "substit"
                             });

    // Inscrição
    _abbreviationsMap.insert(Convert("inscrição", false),
                             QList<QString>
                             {
                                 "insc"
                             });

    // Estadual
    _abbreviationsMap.insert(Convert("estadual", false),
                             QList<QString>
                             {
                                 "est"
                             });

    // Código
    _abbreviationsMap.insert(Convert("codigo", false),
                             QList<QString>
                             {
                                 "cod"
                             });

    // Produto
    _abbreviationsMap.insert(Convert("produto", false),
                             QList<QString>
                             {
                                 "prod",
                                 "produt"
                             });

    // Unidade
    _abbreviationsMap.insert(Convert("unidade", false),
                             QList<QString>
                             {
                                 "un"
                             });

    // Valor
    _abbreviationsMap.insert(Convert("valor", false),
                             QList<QString>
                             {
                                 "v"
                             });

    // Base de Cálculo
    _abbreviationsMap.insert(Convert("base de cálculo", false),
                             QList<QString>
                             {
                                 "bc"
                             });

    // Quantidade
    _abbreviationsMap.insert(Convert("quantidade", false),
                             QList<QString>
                             {
                                 "qt",
                                 "quant",
                                 "quantid",
                                 "qtd"
                             });
}

QString Search::Convert(QString str, bool useAbbreviation)
{
    if(str.isEmpty() || str.isNull()) return "";

    auto data = new QString(str);

    // 1. convert to lower.
    ToLowerCase(data);

    // 2. remove all accents from string (case has accents).
    RemoveAccents(data);

    // 3. remove special character from string.
    RemoveSpecialCharacter(data);

    // 4. remove extra spaces on strings, example: "Str1  Str2" -> "Str1 Str2".
    RemoveExtraCharacter(data, QChar::Space);

    // 4. remove extra character.
    RemoveExtraCharacter(data, '/');

    // 5. remove abbreviation and replace for original text.
    if(useAbbreviation)
    {
        RemoveAbbreviation(data);
    }

    // 6. remove abnormal characters.
    RemoveAbnormal(data);

    return QString(*data);
}

bool Search::ToLowerCase(QString* str)
{
    if(str == nullptr) return false;

    *str = str->toLower();

    return true;
}

bool Search::RemoveAccents(QString* str)
{
    if(str == nullptr) return false;

    QChar c = QChar::Null;
    auto data = QString(*str);

    for(auto i = 0; i < data.length(); i++)
    {
        c = str->at(i);

        if(c.isNull()) continue;

        if(c == "â" || c == "ã" || c == "á" || c == "à" || c == "ä")
        {
            data[i] = 'a';
        }
        else if(c == "é" || c == "è" || c == "ê" || c == "ë")
        {
            data[i] = 'e';
        }
        else if(c == "í" || c == "ì" || c == "î" || c == "ï")
        {
            data[i] = 'i';
        }
        else if(c == "ó" || c == "ò" || c == "ô" || c == "õ" || c == "ö")
        {
            data[i] = 'o';
        }
        else if(c == "ú" || c == "ù" || c == "û" || c == "ü")
        {
            data[i] = 'u';
        }
        else if(c == "ç" || c == "ć")
        {
            data[i] = 'c';
        }
        else if(c == "ñ")
        {
            data[i] = 'n';
        }
        else
        {
            continue;
        }
    }

    auto isEdited = (*(str) == data) ? true : false;
    *str = data;

    return isEdited;
}

bool Search::RemoveSpecialCharacter(QString* str)
{
    if(str == nullptr) return false;

    QChar c = QChar::Null;
    auto data = QString(*str);

    for(auto i = 0; i < data.length(); i++)
    {
        c = str->at(i);

        if(c.isNull()) continue;

        if(c == "!" || c == "?" || c == ":" || c == ";" ||
           c == "*" || c == "'" || c == "," || c == "." ||
           c == "<" || c == ">" || c == "@" || c == "#" ||
           c == "$" || c == "%" || c == "(" || c == ")" ||
           c == "-" || c == "+" || c == "=" || c == "&" ||
           c == "[" || c == "]" || c == "~" || c == "^" ||
           c == "{" || c == "}" || c == "`" || c == "|")
        {
            data[i] = QChar::Space;
        }
        else if(c == "\\")
        {
            data[i] = '/';
        }
        else
        {
            continue;
        }
    }

    auto isEdited = (*(str) == data) ? true : false;
    *str = data;

    return isEdited;
}

bool Search::RemoveExtraCharacter(QString* str, const QChar c)
{
    if(str == nullptr) return false;

    QString data = "";

    std::unique_copy(str->begin(), str->end(),
                     std::back_insert_iterator<QString>(data),
                    [c](QChar a, QChar b)
                    {
                        return a == c && b == c;
                    });

    auto isEdited = (*(str) == data) ? true : false;
    *str = data;

    return isEdited;
}

bool Search::RemoveAbbreviation(QString* str)
{
    if(str == nullptr)
        return false;

    auto strList = str->split(QChar::Space, QString::SkipEmptyParts);
    if(strList.size() <= 0)
        return false;

    QList<QString>* tmpList = nullptr;
    QString tmpStr = "";

    for(auto it = _abbreviationsMap.begin(); it != _abbreviationsMap.end(); ++it)
    {
        tmpList = &(*it);
        if(tmpList == nullptr)
            continue;

        for(auto itLst = tmpList->begin(); itLst != tmpList->end(); ++itLst)
        {
            tmpStr = (*itLst);
            if(tmpStr.isEmpty() || tmpStr.isNull())
                continue;

            for(auto i = 0; i < strList.count(); i++)
            {
                if(strList[i] == tmpStr)
                {
                    strList[i] = it.key();
                }
            }
        }
    }

    *str = strList.join(QChar::Space);

    return true;
}

bool Search::RemoveAbnormal(QString* str)
{
    if(str == nullptr) return false;

    QString data = *str;

    data = data.replace(" / ", " ");
    data = data.replace(QRegExp(" d[aeiou] "), " ");

    auto isEdited = (*(str) == data) ? true : false;
    *str = data;

    return isEdited;
}
