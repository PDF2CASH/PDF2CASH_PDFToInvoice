from django.test import TestCase
import json
from .models import (
    Seller,
    Receiver
)

# Create your tests here.


class SellerTest(TestCase):
    url = '/api/sellers/'

    def setUp(self):
        self.seller1 = Seller.objects.create(
            name='TECARDF VEICULOS E SERVICOS LTDA',
            cnpj='04621624000187',
            cep='71200010'
        )

    def as_dict(self):
        return {
            'id': self.seller1.id,
            'name': self.seller1.name,
            'cnpj': self.seller1.cnpj,
            'cep': self.seller1.cep
        }

    def test_seller_object_get(self):
        response = self.client.get(self.url)
        self.assertEqual(response.status_code, 200)
        self.assertEqual(len(json.loads(response.content)), 1)

    def test_seller_object_post(self):
        data = {
            'name': "PRISMA DISTRIBUIDORA LTDA ME",
            'cnpj': "12123544000104",
            'cep': "41852690"
        }
        response = self.client.post(self.url, data)
        self.assertEqual(response.status_code, 201)
        data['id'] = json.loads(response.content)['id']
        self.assertEqual(json.loads(response.content), data)

    def test_seller_object_delete(self):
        self.url += '{}/'.format(self.seller1.id)
        response = self.client.delete(self.url)
        self.assertEqual(response.status_code, 204)
        response = self.client.get('/api/sellers/')
        self.assertEqual(len(json.loads(response.content)), 0)

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
    url = '/api/receivers/'

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

    def test_receiver_object_post(self):
        data = {
            'name': 'Rafael Teodosio',
            'cpf_cnpj': '39403757102',
            'address': 'Qr 400 conjunto K casa 20',
            'neighborhood': 'Gama',
            'cep': '68394002',
            'county': 'BRASILIA',
            'uf': 'DF',
            'phone': '6133420214'
        }
        response = self.client.post(self.url, data)
        self.assertEqual(response.status_code, 201)
        data['id'] = json.loads(response.content)['id']
        self.assertEqual(json.loads(response.content), data)

    def test_receiver_object_delete(self):
        self.url += '{}/'.format(self.receiver1.id)
        response = self.client.delete(self.url)
        self.assertEqual(response.status_code, 204)
        response = self.client.get('/api/receivers/')
        self.assertEqual(len(json.loads(response.content)), 0)

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
