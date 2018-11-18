#ifndef PARSER_H
#define PARSER_H

#include "Search.h"

#include <QMap>
#include <QList>

#include <QRect>
#include <QPoint>
#include <QSize>
#include <QString>

const int AVERAGE_LEVENSTEIN_VALUE = 30;
const int SPACE_HEIGHT_BETWEEN_HEADER_VALUE = 14;

enum eHEADER_NFE                            /// CABEÇALHO
{
    H_ACCESS_KEY = 0,                         // CHAVE DE ACESSO
    H_NATURE_OPERATION,                       // NATUREZA DA OPERAÇÃO
    H_PROTOCOL_AUTHORIZATION_USE,             // PROTOCOLO DE AUTORIZAÇÃO DE USO
    H_STATE_REGISTRATION,                     // INSCRIÇÃO ESTADUAL
    H_STATE_REGISTRATION_SUB_TAXATION,        // INSCRIÇÃO ESTADUAL SUB. TRIBUTARIA
    H_CNPJ,                                   // CNPJ

    H_MAX,
};

enum eADDRESSEE_SENDER                      /// DESTINATÁRIO/REMETENTE
{
    A_S_NAME_SOCIAL_REASON = 0,             // NOME/RAZÃO SOCIAL
    A_S_CPNJ_CPF,                           // CNPJ/CPF
    A_S_EMISSION_DATE,                      // DATA DA EMISSÃO
    A_S_ADDRESS,                            // ENDEREÇO
    A_S_NEIGHBORHOOD_DISTRICT,              // BAIRRO/DISTRITO
    A_S_CEP,                                // CEP
    A_S_OUTPUT_INPUT_DATE,                  // DATA DE SAÍDA/ENTRADA
    A_S_COUNTY,                             // MUNICÍPIO
    A_S_PHONE_FAX,                          // TELEFONE/FAX
    A_S_UF,                                 // UF
    A_S_STATE_REGISTRATION,                 // INSCRIÇÃO ESTADUAL
    A_S_EXIT_TIME,                          // HORA DE SAÍDA

    A_S_MAX,
};

enum eTAX_CALCULATION                       /// CÁLCULO DO IMPOSTO
{
    T_C_ICMS_CALCULATION_BASIS = 0,             // BASE DE CÁLCULO DE ICMS
    T_C_COST_ICMS,                          // VALOR DO ICMS
    T_C_CALCULATION_BASIS_ICMS_ST,          // BASE DE CÁLCULO ICMS ST
    T_C_VALUE_ICMS_REPLACEMENT,             // VALOR DO ICMS SUBSTITUIÇÃO
    T_C_TOTAL_VALUE_PRODUCTS,               // VALOR TOTAL DOS PRODUTOS
    T_C_COST_FREIGHT,                       // VALOR DO FRETE
    T_C_COST_INSURANCE,                     // VALOR DO SEGURO
    T_C_DISCOUNT,                           // DESCONTO
    T_C_OTHER_EXPENDITURE,                  // OUTRAS DESPESAS ACESSÓRIAS
    T_C_COST_IPI,                           // VALOR DO IPI
    T_C_APPROXIMATE_COST_TAXES,             // VALOR APROX. DOS TRIBUTOS
    T_C_COST_TOTAL_NOTE,                    // VALOR TOTAL DA NOTA

    T_C_MAX,
};

enum eCONVEYOR_VOLUMES                      /// TRANSPORTADOR/VOLUMES TRANSPORTADOS
{
    C_V_SOCIAL_REASON = 0,                      // RAZÃO SOCIAL
    C_V_FREIGHT_ACCOUNT,                    // FRETE POR CONTA
    C_V_CODE_ANTT,                          // CÓDIGO ANTT
    C_V_VEHICLE_PLATE,                      // PLACA DO VEÍCULO
    C_V_UF_1,                               // UF
    C_V_CNPJ_CPF,                           // CNPJ/CPF
    C_V_ADDRESS,                            // ENDEREÇO
    C_V_COUNTY,                             // MUNICÍPIO
    C_V_UF_2,                               // UF
    C_V_STATE_REGISTRATION,                 // INSCRIÇÃO ESTADUAL
    C_V_QUANTITY,                           // QUANTIDADE
    C_V_SPECIES,                            // ESPÉCIE
    C_V_MARK,                               // MARCA
    C_V_NUMBERING,                          // NUMERAÇÃO
    C_V_GROSS_WEIGHT,                       // PESO BRUTO
    C_V_NET_WEIGHT,                         // PESO LIQUIDO

    C_V_MAX,
};

enum eISSQN_CALCULATION                     /// CÁLCULO DO ISSQN
{
    I_C_MUNICIPAL_REGISTRATION = 0,             // INSCRIÇÃO MUNICIPAL
    I_C_TOTAL_COST_SERVICES,                // VALOR TOTAL DOS SERVIÇOS
    I_C_ISSQN_CALCULATION_BASE,             // BASE DE CALCULO DO ISSQN
    I_C_COST_ISSQN,                         // VALOR DO ISSQN

    I_C_MAX,
};

enum eINVOICE_HEADER
{
    MAIN = 0,
    ADDRESSEE_SENDER,                       // DESTINATÁRIO/REMETENTE
    TAX_CALCULATION,                        // CÁLCULO DO IMPOSTO
    CONVEYOR_VOLUMES,                       // TRANSPORTADOR/VOLUMES TRANSPORTADOS
    PRODUCT_SERVICE_DATA,                   // DADOS DO PRODUTO/SERVIÇO
    ISSQN_CALCULATION,                      // CÁLCULO DO ISSQN
    ADDITIONAL_DATA,                        // DADOS ADICIONAIS

    MAX,
};

struct sTEXTDATA
{
    int top;
    int left;
    int height;
    int width;

    long fontIndex;

    QString text;

    sTEXTDATA() {}

    sTEXTDATA(int t, int l, int h, int w, long f, QString txt)
    {
        top = t;
        left = l;
        height = h;
        width = w;
        fontIndex = f;
        text = txt;
    }

    sTEXTDATA(sTEXTDATA* data)
    {
        top = data->top;
        left = data->left;
        height = data->height;
        width = data->width;
        fontIndex = data->fontIndex;
        text = data->text;
    }
};

struct sFONTDESCRIPTION
{
    int index; // it's necessary to define?
    int size;
    QString family;
    QString color; // hex value
};

struct sPAGE
{
    long number;
    QString position;

    int top;
    int left;
    int height;
    int width;

    QMap<int, sFONTDESCRIPTION*> fontMap;
    QList<sTEXTDATA*> txtDataList;
};

struct sINVOICEHEADER
{
    eINVOICE_HEADER header;
    QRect rect;
};

struct sINVOICEDATA
{
    QString header;
    QString value;

    int headerID;
    int subHeaderID;
};

class Parser
{
public:
    Parser();

    bool ReadInvoiceXML(QString fileName);
    bool GetInvoiceData();
    bool ConvertToJson();

    void DebugShow();

    int GetMaxDataHeader(int header);

private:
    QRect TrySimulateRectHeader(QRect headerRect, QList<sTEXTDATA*>* possibleValues, int maxPageHeight, int maxPageWidth);
    bool TryGetValue(sTEXTDATA* header, QList<sTEXTDATA*>* possibleValues, QString* value, int maxPageHeight, int maxPageWidth);

    QString ConvertEnumToText(eINVOICE_HEADER header, int value = -1);

    bool FindValueData(QString value, QList<sTEXTDATA*> list, QRect* rect);

    sINVOICEHEADER* GetInvoiceHeader(QList<sINVOICEHEADER*> list, int value);
    QList<sINVOICEHEADER*> ProcessInvoiceHeader(QList<sTEXTDATA*> possibleHeaders, int maxWidth, int maxHeight);

    sTEXTDATA* GetTextData(QString header, QList<sTEXTDATA*> possibleValues);

    QString ConvertToJsonHeader(int header, int value);
    QString GenerateJson(QMap<int, QList<sINVOICEDATA*>> map);

private:
    QString _fileName;

    QMap<int, sPAGE*>* _pageMap;
    QMap<int, QList<sINVOICEDATA*>> _invoicesMap;

    Search* _search;
};

#endif // PARSER_H
