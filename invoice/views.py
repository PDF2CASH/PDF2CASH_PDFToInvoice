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
from pdfminer.pdfinterp import PDFResourceManager, PDFPageInterpreter
from pdfminer.converter import TextConverter
from pdfminer.layout import LAParams
from pdfminer.pdfpage import PDFPage
from io import StringIO
import pdftotext


def convert_pdf_to_txt(path):
    rsrcmgr = PDFResourceManager()
    retstr = StringIO()
    codec = 'utf-8'
    laparams = LAParams()
    device = TextConverter(rsrcmgr, retstr, codec=codec, laparams=laparams)
    fp = open(path, 'rb')
    interpreter = PDFPageInterpreter(rsrcmgr, device)
    password = ""
    maxpages = 0
    caching = True
    pagenos=set()
    for page in PDFPage.get_pages(fp, pagenos, maxpages=maxpages,password=password,caching=caching, check_extractable=True):

        interpreter.process_page(page)

    text = retstr.getvalue()

    fp.close()
    device.close()
    retstr.close()
    return text  


def search_create_receiver(cpnj_cpf_receiver, text):

    if Receiver.objects.filter(cpf_cnpj = cpnj_cpf_receiver).count() == 1:
            receiver = Receiver.objects.get(cpf_cnpj = cpnj_cpf_receiver)
    else:
        #---- Procurar os atributos do Receiver ----#
        name_receiver = re.search( r'NOME\/RAZÃO\s+SOCIAL\s+([\w+| |\.]+\w)', text, re.M|re.I)
        name_receiver = str(name_receiver.group()).replace('NOME/RAZÃO SOCIAL','')
        name_receiver = name_receiver.replace('\n','')

        print("---------------")
        print(name_receiver)
        print("---------------")

        address_receiver = re.search( r'ENDEREÇO\s+(.+)', text, re.M|re.I)
        address_receiver = str(address_receiver.group()).replace('ENDEREÇO','')
        address_receiver = address_receiver.replace('\n','')

        print("---------------")
        print(address_receiver)
        print("---------------")    
        
        neighborhood_receiver = re.search( r'BAIRRO\/DISTRITO\s+(.+)', text, re.M|re.I)
        neighborhood_receiver = str(neighborhood_receiver.group()).replace('BAIRRO/DISTRITO','')
        neighborhood_receiver = neighborhood_receiver.replace('\n','')

        print("---------------")
        print(neighborhood_receiver)
        print("---------------")

        cep_receiver = re.search( r'DESTINATÁRIO[\W|\w]+CEP[\W|\w]+\s(\d{5}\-\d{3}|\d{2}\.\d{3}\-\d{3}|\d{8})\s[\W|\w]+CÁLCULO DO IMPOSTO', text, re.M|re.I)
        cep_receiver = str(cep_receiver.group())
        cep_receiver = re.search( r'CEP[\W|\w]+\s(\d{5}\-\d{3}|\d{2}\.\d{3}\-\d{3}|\d{8})', cep_receiver, re.M|re.I)
        cep_receiver = str(cep_receiver.group()).replace('\n','')
        cep_receiver = cep_receiver.replace('-','')
        cep_receiver = cep_receiver.replace('.','')
        cep_receiver = cep_receiver.replace('/','')
        cep_receiver = cep_receiver.replace(' ','')
        cep_receiver = cep_receiver.replace('CEP','')
        cep_receiver = cep_receiver[-8:]

        print("---------------")
        print(cep_receiver)
        print("---------------") 

        county_receiver = re.search( r'MUNICÍPIO\s+(FONE\/FAX\s+)?(.+)', text, re.M|re.I)
        county_receiver = str(county_receiver.group()).replace('MUNICÍPIO','')
        county_receiver = county_receiver.replace('\n','')
        county_receiver = county_receiver.replace('FONE/FAX','')

        print("---------------")
        print(county_receiver)
        print("---------------")

        uf_receiver = re.search( r'DESTINATÁRIO[\W|\w]+UF\s+([a-zA-Z]{2})\s[\W|\w]+CÁLCULO DO IMPOSTO', text, re.M|re.I)
        uf_receiver = str(uf_receiver.group())
        uf_receiver = re.search( r'UF\s+([a-zA-Z]{2})', uf_receiver, re.M|re.I)
        uf_receiver = str(uf_receiver.group()).replace('\n','')
        uf_receiver = uf_receiver.replace('\n','')
        uf_receiver = uf_receiver.replace('UF','')
        uf_receiver = uf_receiver[-2:]
        print("---------------")
        print(uf_receiver)
        print("---------------")


        phone_receiver = re.search( r'DESTINATÁRIO[\W|\w]+FONE\/FAX[\W|\w]*\s(\(\d{2}\)\s?\d{5}\-\d{3}|\(\d{2}\)\s?\d{5}\-\d{4}|\d{10})\s[\W|\w]+CÁLCULO DO IMPOSTO', text, re.M|re.I)
        if phone_receiver:
            phone_receiver = str(phone_receiver.group()).replace('FONE/FAX','')
            phone_receiver = re.search( r'(\(\d{2}\)\s?\d{5}\-\d{3}|\(\d{2}\)\s?\d{5}\-\d{4}|\d{10})', phone_receiver, re.M|re.I)
            phone_receiver = str(phone_receiver.group()).replace('\n','')
            phone_receiver = phone_receiver.replace('(','')
            phone_receiver = phone_receiver.replace(')','')
            phone_receiver = phone_receiver.replace('-','')
            phone_receiver = phone_receiver.replace(' ','')
            phone_receiver = phone_receiver[-10:]
        else:
            phone_receiver = ''

        print("---------------")
        print(phone_receiver)
        print("---------------")
        
        receiver = Receiver.objects.create(
            cpf_cnpj = cpnj_cpf_receiver,
            name= name_receiver,
            address= address_receiver,
            neighborhood =neighborhood_receiver,
            cep = cep_receiver,
            county = county_receiver,
            uf = uf_receiver,
            phone = phone_receiver,
            )

    return receiver    



def search_create_seller(cnpj_seller, text, uf_code_seller):
    
    if Seller.objects.filter(cnpj = cnpj_seller).count() == 1:
            seller = Seller.objects.get(cnpj = cnpj_seller)
    else:
        #---- Procurar os atributos do Seller ----#
        name_seller = re.search( r'SÉRIE[0-9\:\s]+([a-zA-Z ]+)', text, re.M|re.I)
        name_seller = str(name_seller.group())
        name_seller = re.search( r'(\s[a-zA-Z ]+\s)', name_seller, re.M|re.I)
        name_seller = str(name_seller.group()).replace('\n','')
        name_seller = name_seller.replace('SÉRIE','')
        name_seller = name_seller.replace('LTDA','')

        print("-------------------")
        print(name_seller)
        print("-------------------")

        cep_seller = re.search( r'SÉRIE[\W|\w]+\s(\d{5}\-\d{3}|\d{2}\.\d{3}\-\d{3}|\d{8})\s[\W|\w]+NATUREZA DA OPERAÇÃO', text, re.M|re.I)
        cep_seller = str(cep_seller.group())
        cep_seller = re.search( r'\s(\d{5}\-\d{3}|\d{2}\.\d{3}\-\d{3}|\d{8})\s', cep_seller, re.M|re.I)
        cep_seller = str(cep_seller.group()).replace('\n','')
        cep_seller = cep_seller.replace('-','')
        cep_seller = cep_seller.replace('.','')
        cep_seller = cep_seller.replace('/','')
        cep_seller = cep_seller.replace(' ','')

        print("-------------------")
        print(cep_seller)
        print("-------------------")
        
        seller = Seller.objects.create(
            cnpj = cnpj_seller,
            name = name_seller,
            cep = cep_seller,
            uf_code = uf_code_seller,
            )

    return seller        
 

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

        dict_invoice['number'] = number 
        
        #---------------------Parser-CNPJ/CPF-reveiver------------------------#
        

        cpnj_cpf_receiver = re.findall( r'([\s+|\n]\d{11}\s+|[\s+|\n]\d{14}\s+|\d{3}\.\d{3}\.\d{3}\-\d{2}|\d{2}\.\d{3}\.\d{3}\/\d{4}\-\d{2})', text, re.M|re.I)

        for i in range(len(cpnj_cpf_receiver)):
           cpnj_cpf_receiver[i] = cpnj_cpf_receiver[i].replace('-','')
           cpnj_cpf_receiver[i] = cpnj_cpf_receiver[i].replace('.','')
           cpnj_cpf_receiver[i] = cpnj_cpf_receiver[i].replace('/','')
           cpnj_cpf_receiver[i] = cpnj_cpf_receiver[i].replace('\n','')

        if cnpj_seller in cpnj_cpf_receiver:
            cpnj_cpf_receiver.remove(cnpj_seller)

        cpnj_cpf_receiver = cpnj_cpf_receiver[0]


        #-------------------Verifica-existencia-de-entidades------------------#


        if Receiver.objects.filter(cpf_cnpj = cpnj_cpf_receiver).count() == 1:
            receiver = Receiver.objects.get(cpf_cnpj = cpnj_cpf_receiver)
            
        else:
            #---- Procurar os atributos do Receiver ----#
            receiver = Receiver.objects.create(cpf_cnpj = cpnj_cpf_receiver)

        dict_invoice['receiver'] = receiver

        if Seller.objects.filter(cnpj = cnpj_seller).count() == 1:
            seller = Seller.objects.get(cnpj = cnpj_seller)
            
        else:
            #---- Procurar os atributos do Seller ----#
            seller = Seller.objects.create(cnpj = cnpj_seller)

        dict_invoice['seller'] = seller        

        serializer = InvoiceSerializer(data= dict_invoice)

        if serializer.is_valid():
            serializer.save()
            return Response(serializer.data, status=status.HTTP_201_CREATED)