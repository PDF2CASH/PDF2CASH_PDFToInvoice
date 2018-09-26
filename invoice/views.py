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



        