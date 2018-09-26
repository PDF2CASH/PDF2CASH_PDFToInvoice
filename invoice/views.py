from rest_framework.decorators import api_view
from rest_framework.response import Response
from .models import *
from .serializers import *
from rest_framework import status
from rest_framework import viewsets
import PyPDF2 
from nltk.tokenize import word_tokenize
from nltk.corpus import stopwords
import re
from django.shortcuts import get_object_or_404


class InvoiceViewSet(viewsets.ModelViewSet):

    queryset = Invoice.objects.all()
    serializer_class = InvoiceSerializer

    def create(self, request):

        data = request.data

        dict_invoice = {}

        #-----------------salva-pdf---------------------#

        pdf = PDF()

        pdf.pdf = data.get('file')

        pdf.save()

        dict_invoice['pdf'] = pdf

        #---------------Converter-para-texto-------------#

        filename = str(pdf.pdf).split('/')[2]
        
        pdfFileObj = open("invoice/images/" + filename,'rb')
       
        pdfReader = PyPDF2.PdfFileReader(pdfFileObj)
       
        num_pages = pdfReader.numPages
        count = 0
        text = ""
        
        while count < num_pages:
            pageObj = pdfReader.getPage(count)
            count +=1
            text += pageObj.extractText()


        #--------------------VALIDAÇÃO-PDF/NF----------------#

        if text != "":
            text = text
        else:
            return Response(status=400) 

        tokens = word_tokenize(text)

        punctuations = ['(',')',';',':','[',']',',']

        keywords = [word for word in tokens if not word in punctuations]

        print("-----------------------------")
        print(keywords)
        print("-----------------------------")   

        validation_words = [
            "WWW.NFE.FAZENDA.GOV.BR/PORTAL",
            "www.nfe.fazenda.gov.br/portal",
            "WWW.NFE.FAZENDA.GOV.BR",
            "www.nfe.fazenda.gov.br",
            ]

        i = 0
        
        for word in validation_words:
            if word in keywords:
                i = i + 1

        if i < 1:       
            return Response(status=400)

        dict_invoice['text'] = text 

        #----------------------------Parser-AccessKey------------------------#
        
        new_text = text.replace('\n','')

        access_key = re.search( r'\d{4}\s+\d{4}\s+\d{4}\s+\d{4}\s+\d{4}\s+\d{4}\s+\d{4}\s+\d{4}\s+\d{4}\s+\d{4}\s+\d{4}', new_text, re.M|re.I)

        access_key = str(access_key.group()).replace(' ','')

        uf_code_seller = access_key[0:2]

        cnpj_seller = access_key[6:20]

        number = access_key[25:34]


        