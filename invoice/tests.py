
from django.test import TestCase
from django.core.files import File
import json
import datetime
from .models import (
    Invoice
)
import io
from django.core.files.uploadedfile import SimpleUploadedFile



class InvoiceTest(TestCase):

    def setUp(self):
        self.url = '/api/invoice/'

    def test_create(self):

        file = open("invoice/test.pdf", 'rb')
        response = self.client.post(self.url, {'file': file}, format='multipart')
        print(response)
        self.assertEqual(response.status_code, 200)
