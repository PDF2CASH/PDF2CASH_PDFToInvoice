[![Build Status](https://travis-ci.org/PDF2CASH/PDF2CASH_PDFToInvoice.svg?branch=development)](https://travis-ci.org/PDF2CASH/PDF2CASH_PDFToInvoice)
[![Coverage Status](https://coveralls.io/repos/github/PDF2CASH/PDF2CASH_PDFToInvoice/badge.svg?branch=development)](https://coveralls.io/github/PDF2CASH/PDF2CASH_PDFToInvoice?branch=master)


# PDF2CASH_PDFToInvoice

## Como compilar
Antes de compilar, é necessário compilar as 2 bibliotecas utilizadas no projeto do Parser.

### Pré-requisitos

- Qt 5.11.2

### 1. Poppler
Vá na pasta do Poppler, é encontrada no diretório:

`PROJDIR/3rdparty/poppler-library`

Logo após, execute os comandos abaixo para compilação.:

`$ make`
`$ sudo make install`

> PROJDIR = pasta root do projeto onde foi clonado.

### 2. QHTTP
Vá na pasta do QHTTP, é encontrada no diretório:

`PROJDIR/3rdparty/qhttp`

Para compilação do QHTTP, primeiramente é necessário instalar as dependências exigidas.

Execute o comando abaixo.:

`$ ./update-dependencies.sh`

Logo após, compile o QHTTP.:

`$ qmake -r qhttp.pro`
`$ make -j 8`

E instale.:

`$ sudo make install`

> PROJDIR = pasta root do projeto onde foi clonado.

### 3. Servidor
Depois de ter compilado todos as bibliotecas utilizadas no projeto, agora por último, será o processo da compilação do servidor do parser.

Vá na pasta do Servidor, é encontrado no diretório:

`PROJDIR/server`

E compile-o.:

`make`

> PROJDIR = pasta root do projeto onde foi clonado.

## Contribuindo

Por favor leia o nosso [CONTRIBUTING.md](https://github.com/fga-eps-mds/2018.2-PDF2CASH/blob/master/CONTRIBUTING.md) se deseja contribuir com nosso projeto.

## Licensa

Esse projeto é licensiado sobre a licensa MIT
