#include "Search.h"

#include "Utils.h"

#include <QChar>
#include <QRegularExpression>

#include <QDebug>

#include <QSize>
#include <QPoint>

#include "Parser.h"

using namespace pdf2cash;

Search::Search()
{
    // Init abbreviation's map.
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
                                 "sub",
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

    // Aproximado
    _abbreviationsMap.insert(Convert("aproximado", false),
                             QList<QString>
    {
                                 "aprox"
                             });

    // Tributario
    _abbreviationsMap.insert(Convert("tributario", false),
                             QList<QString>
    {
                                 "tributaria"
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

    return (data == nullptr || data->isNull() || data->isEmpty()) ?
                str :
                QString(*data);
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
                c == "{" || c == "}" || c == "`" || c == "|" ||
                c == "/" || c == "\\")
        {
            data[i] = QChar::Space;
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

    // Remove from string: da(s) de(s) di(s) do(s) du(s).
    data = data.replace(QRegularExpression(" d[aeiou](?:[s])? "), " ");

    // Remove from string: a(s) e(s) i(s) o(s) u(s).
    data = data.replace(QRegularExpression(" [aeiou](?:[s])? "), " ");

    auto isEdited = (*(str) == data) ? true : false;
    *str = data;

    return isEdited;
}

// ---------------------------------------------------------------
// Functions related for search.
// ---------------------------------------------------------------

sTEXTDATA* Search::CalculeDistance(const QRect rect, const QList<sTEXTDATA*> dataList)
{
    QRect tmpRect;
    sTEXTDATA* tmpData = nullptr;
    sTEXTDATA* currentData = nullptr;

    const QPoint rectCenter = rect.center();
    QPoint tmpCenter;

    int distance = 9999;
    int currentDistance = 0;

    for(auto it = dataList.begin(); it != dataList.end(); ++it)
    {
        tmpData = (*it);
        tmpRect = QRect(QPoint(tmpData->left, tmpData->top), QSize(tmpData->width, tmpData->height));
        tmpCenter = tmpRect.center();

        currentDistance = qMax((qAbs(rectCenter.x() - tmpCenter.x()) - (rect.width() + tmpRect.width())) / 2,
             (qAbs(rectCenter.y() - tmpCenter.y()) - (rect.height() + tmpRect.height())) / 2);

        if(currentDistance < distance)
        {
            distance = currentDistance;
            currentData = &(*tmpData);
        }
    }

    return (tmpData != nullptr) ? tmpData : dataList.first();
}

sTEXTDATA* Search::SearchText(const QString pattern, const QList<sTEXTDATA*> strList, const int averageLevenstein,
                              const QString mainPattern, const bool checkDistance, const QRect* rect)
{
    // Check if list is null.
    if(strList.size() <= 0)
        return nullptr;

    // List will be used for convert all texts.
    QList<sTEXTDATA*> strListProcessed;

    sTEXTDATA* tmpData = nullptr;

    // Process every element from list and convert text's.
    for(auto it = strList.begin(); it != strList.end(); ++it)
    {
        tmpData = (*it);
        if(tmpData == nullptr)
            continue;

        strListProcessed.push_back(new sTEXTDATA(tmpData->top, tmpData->left,
                                                 tmpData->height, tmpData->width,
                                                 tmpData->fontIndex,
                                                 Convert(tmpData->text)));
    }

    tmpData = nullptr;

    // List's.
    QList<sTEXTDATA*> strExactLst;
    QList<sTEXTDATA*> strKMPLst;
    QList<SearchDataDistance> strLevensteinLst;
    QList<SearchDataDistance> strTagsLst;

    // Temp variables.
    QString tmpStr = "";
    QString tmpConvertedStr = "";

    const QString patternConverted = Convert(pattern);

    // Trie Node.
    TrieNode* node = new TrieNode();
    node->insert(patternConverted);

    QStringList patternLst = patternConverted.split(QChar::Space, QString::SkipEmptyParts);

    int tmpTax = 0;

    // 1. - let to check if has string with exact string.
    for(auto it = strListProcessed.begin(); it != strListProcessed.end(); ++it)
    {
        tmpStr = (*it)->text;
        if(tmpStr.isEmpty() || tmpStr.isNull())
            continue;

        tmpConvertedStr = Convert(tmpStr);
        if(tmpConvertedStr.isEmpty() || tmpConvertedStr.isNull())
        {
            tmpConvertedStr = tmpStr;
        }

        // First let to search by exact algorithm.
        if(SearchByExact(patternConverted, tmpConvertedStr))
        {
            strExactLst.push_back(*it);
        }

        // Second let to search by KMP algorithm.
        if(SearchByKMP(patternConverted, tmpConvertedStr))
        {
            strKMPLst.push_back(*it);
        }

        // Third let to searby by Levenstein algorithm.
        tmpTax = SearchByLevenstein(node, tmpConvertedStr);
        if(tmpTax <= averageLevenstein)
        {
            strLevensteinLst.push_back(*(new SearchDataDistance(tmpTax, (*it))));
        }

        tmpTax = SearchByTags(patternLst, tmpConvertedStr, mainPattern);
        if(tmpTax > 0)
        {
            strTagsLst.push_back(*(new SearchDataDistance(tmpTax, (*it))));
        }
    }

    // Sort list of levenstein by rate.
    std::sort(strLevensteinLst.begin(), strLevensteinLst.end(),
              [](const SearchDataDistance & s1, const SearchDataDistance & s2)
                {
                    return s1.rate < s2.rate;
                });

    // Sort list of levenstein by rate.
    std::sort(strTagsLst.begin(), strTagsLst.end(),
              [](const SearchDataDistance & s1, const SearchDataDistance & s2)
                {
                    return s1.rate > s2.rate;
                });

    // Check if found some exact string.
    if(strExactLst.size() > 0)
    {
        if(strExactLst.size() == 1)
        {
            return strExactLst.first();
        }
        else
        {
            return (!checkDistance || (checkDistance == true && rect == nullptr)) ?
                        strExactLst.first() :
                        CalculeDistance(*rect, strExactLst);
        }
    }
    else
    {
        // Not found exact string, now try to find using KMP algorithm.

        // 2. - let to check if has string match with KMP algorithm.
        if(strKMPLst.size() > 0)
        {
            return (strKMPLst.size() == 1 || !checkDistance || (checkDistance == true && rect == nullptr)) ?
                        strKMPLst.first() :
                        CalculeDistance(*rect, strKMPLst);
        }
        else
        {
            // 3. - let to check if has string match with levenstein algorithm.
            if(strLevensteinLst.size() > 0 && strTagsLst.size() > 0)
            {
                QList<sTEXTDATA*> tmpList;
                SearchDataDistance* tmpSearchData = nullptr;
                const int rate = strLevensteinLst.first().rate;

                for(auto it = strLevensteinLst.begin(); it != strLevensteinLst.end(); ++it)
                {
                    tmpSearchData = &(*it);
                    if(tmpSearchData == nullptr)
                        continue;

                    if(tmpSearchData->rate == rate)
                    {
                        tmpList.push_back(tmpSearchData->data);
                    }
                }

                if(tmpList.size() > 0)
                {
                    if(checkDistance == true && rect != nullptr)
                    {
                        QList<sTEXTDATA*> tmpTagList;

                        for(auto it = strTagsLst.begin(); it != strTagsLst.end(); ++it)
                        {
                            tmpSearchData = &(*it);
                            if(tmpSearchData == nullptr)
                                continue;

                            if(tmpSearchData->rate == rate)
                            {
                                tmpTagList.push_back(tmpSearchData->data);
                            }
                        }

                        return CalculeDistance(*rect, tmpTagList);
                    }
                    else
                    {
                        return strTagsLst.first().data;
                    }

                    //return (tmpList.size() == 1 || !checkDistance || (checkDistance == true && rect == nullptr)) ?
                    //            tmpList.first() :
                    //            CalculeDistance(*rect, tmpList);
                }
                else
                {
                    return strTagsLst.first().data;
                }
            }
            else
            {
                return nullptr;
            }
        }
    }
}

bool Search::SearchByExact(const QString pattern, QString str)
{
    if(pattern.length() != str.length())
        return false;

    return (pattern.compare(str, Qt::CaseInsensitive) == 0) ? true : false;
}

void Search::SearchImpl(TrieNode* tree, QChar ch, QVector<int> last_row, const QString& word, int* minCost)
{
    int sz = last_row.size();

    QVector<int> current_row(sz);
    current_row[0] = last_row[0] + 1;

    // Calculate the min cost of insertion, deletion, match or substution
    int insert_or_del, replace;
    for (auto i = 1; i < sz; ++i)
    {
        insert_or_del = qMin(current_row[i - 1] + 1, last_row[i] + 1);
        replace = (word[i - 1] == ch) ? last_row[i - 1] : (last_row[i - 1] + 1);

        current_row[i] = qMin(insert_or_del, replace);
    }

    // When we find a cost that is less than the min_cost, is because
    // it is the minimum until the current row, so we update
    if ((current_row[sz - 1] < (*minCost)) && (tree->word != ""))
    {
        *minCost = current_row[sz - 1];
    }

    // If there is an element wich is smaller than the current minimum cost,
    // we can have another cost smaller than the current minimum cost
    if (*std::min_element(current_row.begin(), current_row.end()) < (*minCost))
    {
        for (auto it = tree->next.begin(); it != tree->next.end(); ++it)
        {
            SearchImpl(it.value(), it.key(), current_row, word, minCost);
        }
    }
}

int Search::SearchByLevenstein(TrieNode* node, QString word, int minCost)
{
    // Check if trie node is not null.
    if(node == nullptr)
    {
        printf("TrieNode is null.\n");
        return -1;
    }

    word = QString("$") + word;

    auto sz = word.size();

    // The minimum cost of a given word to be changed to a word of the dictionary
    int min_cost = minCost;

    QVector<int> current_row(sz + 1);

    // Naive DP initialization
    for (auto i = 0; i < sz; ++i)
    {
        current_row[i] = i;
    }

    current_row[sz] = sz;

    // For each letter in the root map wich matches with a
    //  letter in word, we must call the search
    for (auto i = 0; i < sz; ++i)
    {
        if (node->next.find(word[i]) != node->next.end())
        {
            SearchImpl(node->next[word[i]], word[i], current_row, word, &min_cost);
        }
    }

    return min_cost;
}

// ---------------------------------------------------------------
// Functions related for test.
// ---------------------------------------------------------------

QList<QString>* Search::CreateListTest()
{
    QList<QString>* lst = new QList<QString>();

    //lst->push_back(L"CHAVE DE ACESSO");
    lst->push_back(Convert("CHAVE DE ACESSO DA NF-e P/ CONSULTA DE AUTENTICIDADE NO SITE WWW.NFE.FAZENDA.GOV.BR"));
    lst->push_back(Convert("NATUREZA DA OPERAÇÃO"));
    lst->push_back(Convert("PROTOCOLO DE AUTORIZAÇÃO DE USO"));
    lst->push_back(Convert("INSCRIÇÃO ESTADUAL"));
    lst->push_back(Convert("INSCRIÇÃO ESTADUAL SUB. TRIBUTARIA"));
    //lst->push_back(L"INSC.ESTADUAL DO SUBST. TRIBUTÁRIO");
    lst->push_back(Convert("CNPJ"));
    lst->push_back(Convert("NOME/RAZÃO SOCIAL"));
    lst->push_back(Convert("CNPJ/CPF"));
    lst->push_back(Convert("DATA DA EMISSÃO"));
    lst->push_back(Convert("ENDEREÇO"));
    lst->push_back(Convert("BAIRRO/DISTRITO"));
    lst->push_back(Convert("BAIRRO / DISTRITO"));
    lst->push_back(Convert("CEP"));
    lst->push_back(Convert("DATA DE SAÍDA/ENTRADA"));
    //lst->push_back(L"DATA DA SAÍDA/ENTRADA");
    lst->push_back(Convert("MUNICÍPIO"));
    lst->push_back(Convert("TELEFONE/FAX"));
    lst->push_back(Convert("FONE/FAX"));
    lst->push_back(Convert("UF"));
    lst->push_back(Convert("INSCRIÇÃO ESTADUAL"));
    lst->push_back(Convert("HORA DE SAÍDA"));
    lst->push_back(Convert("DESTINATÁRIO/REMETENTE"));
    //lst->push_back(L"DESTINATÁRIO / REMETENTE");
    lst->push_back(Convert("BASE DE CÁLCULO DE ICMS"));
    //lst->push_back(L"BASE DE CÁLCULO DO ICMS");
    lst->push_back(Convert("VALOR DO ICMS"));
    lst->push_back(Convert("BASE DE CÁLCULO ICMS ST"));
    //lst->push_back(L"BASE DE CÁLCULO DO ICMS ST");
    lst->push_back(Convert("VALOR DO ICMS SUBSTITUIÇÃO"));
    lst->push_back(Convert("VALOR DO ICMS ST"));
    lst->push_back(Convert("VALOR TOTAL DOS PRODUTOS"));
    lst->push_back(Convert("VALOR DO FRETE"));
    lst->push_back(Convert("VALOR DO SEGURO"));
    lst->push_back(Convert("DESCONTO"));
    lst->push_back(Convert("OUTRAS DESPESAS ACESSÓRIAS"));
    lst->push_back(Convert("VALOR DO IPI"));
    lst->push_back(Convert("VALOR APROX. DOS TRIBUTOS"));
    lst->push_back(Convert("VALOR TOTAL DA NOTA"));
    lst->push_back(Convert("CÁLCULO DO IMPOSTO"));
    lst->push_back(Convert("RAZÃO SOCIAL"));
    lst->push_back(Convert("FRETE POR CONTA"));
    lst->push_back(Convert("CÓDIGO ANTT"));
    lst->push_back(Convert("PLACA DO VEÍCULO"));
    lst->push_back(Convert("UF"));
    lst->push_back(Convert("CNPJ/CPF"));
    //lst->push_back(L"CNPJ / CPF");
    lst->push_back(Convert("ENDEREÇO"));
    lst->push_back(Convert("MUNICÍPIO"));
    lst->push_back(Convert("UF"));
    lst->push_back(Convert("INSCRIÇÃO ESTADUAL"));
    lst->push_back(Convert("QUANTIDADE"));
    lst->push_back(Convert("ESPÉCIE"));
    lst->push_back(Convert("MARCA"));
    lst->push_back(Convert("NUMERAÇÃO"));
    lst->push_back(Convert("PESO BRUTO"));
    //lst->push_back(L"PESO LIQUIDO");
    lst->push_back(Convert("PESO LÍQUIDO"));
    lst->push_back(Convert("TRANSPORTADOR/VOLUMES TRANSPORTADOS"));
    //lst->push_back(L"TRANSPORTADOR / VOLUMES TRANSPORTADOS DADOS");
    lst->push_back(Convert("DADOS DO PRODUTO/SERVIÇO"));
    //lst->push_back(L"DADOS DO PRODUTO / SERVIÇO");
    lst->push_back(Convert("INSCRIÇÃO MUNICIPAL"));
    lst->push_back(Convert("VALOR TOTAL DOS SERVIÇOS"));
    //lst->push_back(L"BASE DE CALCULO DO ISSQN");
    lst->push_back(Convert("BASE DE CÁLCULO DO ISSQN"));
    lst->push_back(Convert("VALOR DO ISSQN"));
    lst->push_back(Convert("CÁLCULO DO ISSQN"));
    lst->push_back(Convert("DADOS ADICIONAIS"));

    return lst;
}

void Search::TestByLevenstein()
{
    QString t = Convert("BASE DE CÁLCULO DO ICMS", false);

    auto tree = new TrieNode();

    tree->insert(t);

    qInfo() << "[Database]:\t" << t << "\n\n";

    auto dictionary = CreateListTest();
    QList<SearchDataDistance> possiblesLst;

    int tmpTax = 0;

    for (auto it = dictionary->begin(); it != dictionary->end(); ++it)
    {
        QString s = (*it);
        if (s.isEmpty() || s.isNull())
            continue;

        tmpTax = SearchByLevenstein(tree, s);
        if (tmpTax > 20)
            continue;

        possiblesLst.push_back(*(new SearchDataDistance(tmpTax, new sTEXTDATA(0, 0, 0, 0, 0, s))));
    }

    std::sort(possiblesLst.begin(), possiblesLst.end(), [](const SearchDataDistance & data1, const SearchDataDistance & data2)
    {
        return data1.rate < data2.rate;
    });

    SearchDataDistance* tmpData = nullptr;

    for (auto it = possiblesLst.begin(); it != possiblesLst.end(); ++it)
    {
        tmpData = &(*it);
        qInfo() << "[Rate]:" << "\t" << tmpData->rate << "\t" << "[String]: " << tmpData->data->text;
    }

    qInfo() << "\nFinished Test Levenstein algorithm.\n";
}

// Fills lps[] for given patttern pat[0..M-1]
void Search::ComputeLPSArray(QString pat, int M, int* lps)
{
    // length of the previous longest prefix suffix
    int len = 0;

    lps[0] = 0; // lps[0] is always 0

    // the loop calculates lps[i] for i = 1 to M-1
    int i = 1;
    while (i < M)
    {
        if (pat[i] == pat[len])
        {
            len++;
            lps[i] = len;
            i++;
        }
        else // (pat[i] != pat[len])
        {
            // This is tricky. Consider the example.
            // AAACAAAA and i = 7. The idea is similar
            // to search step.
            if (len != 0)
            {
                len = lps[len - 1];

                // Also, note that we do not increment
                // i here
            }
            else // if (len == 0)
            {
                lps[i] = 0;
                i++;
            }
        }
    }
}


bool Search::SearchByKMP(QString pattern, QString str)
{
    bool found = false;

    int M = pattern.length();
    int N = str.length();

    // create lps[] that will hold the longest prefix suffix
    // values for pattern
    int lps[M];

    // Preprocess the pattern (calculate lps[] array)
    ComputeLPSArray(pattern, M, lps);

    int i = 0; // index for txt[]
    int j = 0; // index for pat[]
    while (i < N)
    {
        if (pattern[j] == str[i])
        {
            j++;
            i++;
        }

        if (j == M)
        {
            //qInfo() << "Found pattern at index\n" << i - j;
            j = lps[j - 1];

            // found pattern in current txt
            found = true;
        }

        // mismatch after j matches
        else if (i < N && pattern[j] != str[i])
        {
            // Do not match lps[0..lps[j-1]] characters,
            // they will match anyway
            if (j != 0)
            {
                j = lps[j - 1];
            }
            else
            {
                i = i + 1;
            }
        }
    }

    return found;
}

void Search::TestByKMP()
{
    auto list = CreateListTest();
    QList<QString> foundList;

    QString t = Convert("CHAVE DE ACESSO", false);

    for(auto it = list->begin(); it != list->end(); ++it)
    {
        if(SearchByKMP(t, (*it)))
        {
            foundList.push_back((*it));
        }
    }

    for(auto it = foundList.begin(); it != foundList.end(); ++it)
    {
        qInfo() << (*it);
    }

    qInfo() << "\nFinished Test Levenstein algorithm.\n";
}

int Search::SearchByTags(QStringList pattern, QString str, QString mainPattern)
{
    int matches = 0;

    bool checkPattern = (mainPattern.isEmpty() || mainPattern.isNull()) ? false : true;
    bool containPattern = false;

    QStringList strLst = str.split(QChar::Space, QString::SkipEmptyParts);

    for(auto i = 0; i < strLst.length(); i++)
    {
        for(auto k = 0; k < pattern.length(); k++)
        {
            if(checkPattern && strLst[i] == mainPattern)
            {
                containPattern = true;
            }

            if(strLst[i] == pattern[k])
            {
                matches++;
                break;
            }
        }
    }

    return (checkPattern == true && containPattern == false) ? 0 : matches;
}
