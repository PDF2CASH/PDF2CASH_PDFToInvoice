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

