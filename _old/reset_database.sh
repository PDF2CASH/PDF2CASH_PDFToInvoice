#!/bin/bash

apps="$(ls -d */ | grep PDF2CASH_PDFToInvoice -v)"

for app in ${apps[@]}
do
    rm -rf $app/migrations/0*
    sudo rm -rf $app/migrations/__pycache__
done

rm -rf mydatabase
docker-compose run web ./manage.py makemigrations
docker-compose run web ./manage.py migrate