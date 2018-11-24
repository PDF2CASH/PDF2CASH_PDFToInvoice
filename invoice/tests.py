from django.test import TestCase
from django.core.files import File
import json
import datetime
import mock
from .models import (
    Invoice,
    Seller,
    Receiver,
)

# Create your tests here.


class SellerTest(TestCase):
    url = '/api/invoice/seller/'

    def setUp(self):
        self.seller1 = Seller.objects.create(
            name='TECARDF VEICULOS E SERVICOS LTDA',
            cnpj='04621624000187',
            cep='71200010',
            uf_code=51
        )

    def as_dict(self):
        return {
            'id': self.seller1.id,
            'name': self.seller1.name,
            'cnpj': self.seller1.cnpj,
            'cep': self.seller1.cep,
            'uf_code': self.seller1.uf_code
        }

    def test_seller_object_get(self):
        response = self.client.get(self.url)
        self.assertEqual(response.status_code, 200)
        self.assertEqual(len(json.loads(response.content)), 1)

    def test_seller_object_read(self):
        self.url += '{}/'.format(self.seller1.id)
        response = self.client.get(self.url)
        self.assertEqual(response.status_code, 200)
        self.assertAlmostEqual(json.loads(response.content), self.as_dict())

    def test_seller_object_update(self):
        self.url += '{}/'.format(self.seller1.id)
        response = self.client.put(
            self.url,
            data={},
            content_type='application/json'
        )
        self.assertEqual(response.status_code, 400)

    def test_seller_object_partial_update(self):
        self.url += '{}/'.format(self.seller1.id)
        response = self.client.patch(
            self.url,
            data={},
            content_type='application/json'
        )
        self.assertEqual(response.status_code, 400)

    def tearDown(self):
        Seller.objects.all().delete()


class ReceiverTest(TestCase):
    url = '/api/invoice/receiver/'

    def setUp(self):
        self.receiver1 = Receiver.objects.create(
            name='CARLOS ENRIQUE RODRIGUES ARAGON',
            cpf_cnpj='05018695145',
            address='SMPW QUADRA 20 CONJUNTO 1 LT 07 C, 06',
            neighborhood='PARK WAY',
            cep='71745001',
            county='BRASILIA',
            uf='DF',
            phone='6133430264'
        )

    def as_dict(self):
        return {
            'id': self.receiver1.id,
            'name': self.receiver1.name,
            'cpf_cnpj': self.receiver1.cpf_cnpj,
            'address': self.receiver1.address,
            'neighborhood': self.receiver1.neighborhood,
            'cep': self.receiver1.cep,
            'county': self.receiver1.county,
            'uf': self.receiver1.uf,
            'phone': self.receiver1.phone
        }

    def test_receiver_object_get(self):
        response = self.client.get(self.url)
        self.assertEqual(response.status_code, 200)
        self.assertEqual(len(json.loads(response.content)), 1)

    def test_receiver_object_read(self):
        self.url += '{}/'.format(self.receiver1.id)
        response = self.client.get(self.url)
        self.assertEqual(response.status_code, 200)
        self.assertAlmostEqual(json.loads(response.content), self.as_dict())

    def test_receiver_object_update(self):
        self.url += '{}/'.format(self.receiver1.id)
        response = self.client.put(
            self.url,
            data={},
            content_type='application/json'
        )
        self.assertEqual(response.status_code, 400)

    def test_receiver_object_partial_update(self):
        self.url += '{}/'.format(self.receiver1.id)
        response = self.client.patch(
            self.url,
            data={},
            content_type='application/json'
        )
        self.assertEqual(response.status_code, 400)

    def tearDown(self):
        Receiver.objects.all().delete()


class InvoiceTest(TestCase):
    url = '/api/invoice/invoice/'
    TestCase.maxDiff = None

    def setUp(self):
        file_mock = mock.MagicMock(spec=File)
        file_mock.name = "test.pdf"

        self.seller1 = Seller.objects.create(
            name='TECARDF VEICULOS E SERVICOS LTDA',
            cnpj='04621624000187',
            cep='71200010',
            uf_code=55
        )

        self.receiver1 = Receiver.objects.create(
            name='CARLOS ENRIQUE RODRIGUES ARAGON',
            cpf_cnpj='05018695145',
            address='SMPW QUADRA 20 CONJUNTO 1 LT 07 C, 06',
            neighborhood='PARK WAY',
            cep='71745001',
            county='BRASILIA',
            uf='DF',
            phone='6133430264'
        )

        self.invoice1 = Invoice.objects.create(
            number='000441407',
            operation_nature='DFVENDA ACESSORIOS EM VN',
            authorization_protocol='353180007787599 27/02/2018 16:54',
            access_key='53180204621624000187550000004414071007026370',
            state_registration='0742544700104',
            seller=self.seller1,
            receiver=self.receiver1,
            emission_date=datetime.date(2018, 2, 27),
            entry_exit_datetime=datetime.datetime(2018, 2, 27, 16, 20, 55),
            total_products_value=146.00,
            total_invoice_value=146.00,
            basis_calculation_icms=0.0,
            freight_value=0.0,
            insurance_value=0.0,
            icms_value=0.0,
            discount_value=0.0,
            basis_calculation_icms_st=0.0,
            icms_value_st=0.0,
            other_expenditure=0.0,
            ipi_value=0.0
            )

    def as_dict(self):
        return {
            'id': self.invoice1.id,
            'number': self.invoice1.number,
            'operation_nature': self.invoice1.operation_nature,
            'authorization_protocol': self.invoice1.authorization_protocol,
            'access_key': self.invoice1.access_key,
            'state_registration': self.invoice1.state_registration,
            'seller': self.seller1.id,
            'receiver': self.receiver1.id,
            'emission_date': self.invoice1.emission_date.strftime('%Y-%m-%d'),
            'entry_exit_datetime': self.invoice1.entry_exit_datetime.strftime('%Y-%m-%dT%H:%M:%SZ'),
            # 'total_products_value': self.invoice1.total_products_value,
            'total_invoice_value': self.invoice1.total_invoice_value,
            'basis_calculation_icms': self.invoice1.basis_calculation_icms,
            'freight_value': self.invoice1.freight_value,
            'insurance_value': self.invoice1.insurance_value,
            'icms_value': self.invoice1.icms_value,
            'discount_value': self.invoice1.discount_value,
            'basis_calculation_icms_st': self.invoice1.basis_calculation_icms_st,
            'icms_value_st': self.invoice1.icms_value_st,
            'other_expenditure': self.invoice1.other_expenditure,
            'ipi_value': self.invoice1.ipi_value
        }

    def test_invoice_object_get(self):
        response = self.client.get(self.url)
        self.assertEqual(response.status_code, 200)
        self.assertEqual(len(json.loads(response.content)), 1)

    def test_invoice_object_delete(self):
        self.url += '{}/'.format(self.invoice1.id)
        response = self.client.delete(self.url)
        self.assertEqual(response.status_code, 204)
        response = self.client.get('/api/invoice/invoice/')
        self.assertEqual(len(json.loads(response.content)), 0)

    def test_invoice_object_read(self):
        self.url += '{}/'.format(self.invoice1.id)
        response = self.client.get(self.url)
        self.assertEqual(response.status_code, 200)
        self.assertDictContainsSubset(self.as_dict(), json.loads(response.content))

    def test_invoice_object_update(self):
        self.url += '{}/'.format(self.invoice1.id)
        response = self.client.put(
            self.url,
            data={},
            content_type='application/json'
        )
        self.assertEqual(response.status_code, 405)

    def test_invoice_object_partial_update(self):
        self.url += '{}/'.format(self.invoice1.id)
        response = self.client.patch(
            self.url,
            data={},
            content_type='application/json'
        )
        self.assertEqual(response.status_code, 405)

    def test_invoice_object_create(self):
        file = open("invoice/test.pdf", 'rb')
        json_test = {
                    "number": '000441407',
                    "main_nature_operation": 'DFVENDA ACESSORIOS EM VN',
                    "main_protocol_authorization_use": '353180007787599 27/02/2018 16:54',
                    "main_access_key": '53180204621724000187550000004414071007026370',
                    "main_state_registration": '0742544700104',
                    "sender_cnpj_cpf": "12345678911",
                    "sender_emission_date": "27/02/2018",
                    "sender_out_input_date": "27/02/2018",
                    "sender_output_time": "16:20:55",
                    "tax_total_cost_products": "146,00",
                    "tax_cost_total_note": "146,00",
                    "tax_icms_basis": "0,0",
                    "tax_cost_freight": "0,0",
                    "tax_cost_insurance": "0,0",
                    "tax_cost_icms": "0,0",
                    "tax_discount": "0,0",
                    "tax_icms_basis_st": "0,0",
                    "tax_cost_icms_replacement": "0,0",
                    "tax_other_expenditure": "0,0",
                    "tax_cost_ipi": "0,0",
                    "sender_name_social": self.receiver1.name,
                    "sender_address": self.receiver1.address,
                    "sender_neighborhood_district":  self.receiver1.neighborhood,
                    "sender_cep": self.receiver1.cep,
                    "sender_county": self.receiver1.county,
                    "sender_uf": self.receiver1.uf,
                    "sender_phone_fax": self.receiver1.phone,
                }
        json_test['file'] = file
        response = self.client.post('/api/invoice/invoice/', json_test, format='multipart')
        self.assertEqual(response.status_code, 200)

    def tearDown(self):
        Invoice.objects.all().delete()
