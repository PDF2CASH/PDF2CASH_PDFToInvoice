#!/bin/bash

apt get install zip
apt get install curl
cd ..
zip -r  ./pdftoinvoice.zip ./*
curl -F 'file=@./pdftoinvoice.zip' https://pdf2cash-cloudupdater.herokuapp.com/api/system/pdftoinvoice/
