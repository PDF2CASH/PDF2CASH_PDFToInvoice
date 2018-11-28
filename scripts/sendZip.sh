#!/bin/bash

apt get install zip
apt get install curl
zip -r  ./pdftoinvoice.zip ./build
curl -F 'file=@./pdftoinvoice.zip' https://pdf2cash-cloudupdater.herokuapp.com/api/system/pdftoinvoice/
