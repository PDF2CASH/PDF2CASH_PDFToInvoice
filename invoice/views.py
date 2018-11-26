import json
from django.http import HttpResponse
from .models import (
        Invoice,
        Seller,
        Receiver,
        )
from .serializers import (
        InvoiceSerializer,
        SellerSerializer,
        ReceiverSerializer,
        )
import re
from rest_framework.parsers import MultiPartParser
from django.views import View
from django.http import Http404
from rest_framework.decorators import parser_classes


def get_object_invoice(pk):
    try:
        return Invoice.objects.get(pk=pk)
    except Invoice.DoesNotExist:
        raise Http404


def get_object_seller(pk):
    try:
        return Seller.objects.get(pk=pk)
    except Seller.DoesNotExist:
        raise Http404


def get_object_receiver(pk):
    try:
        return Receiver.objects.get(pk=pk)
    except Receiver.DoesNotExist:
        raise Http404


class InvoiceShowDelete(View):

    def get(self, request, pk):
        invoice = get_object_invoice(pk)
        serializer = InvoiceSerializer(invoice)
        return HttpResponse(json.dumps(serializer.data), status=200)

    def delete(self, request, pk):
        invoice = get_object_invoice(pk)
        invoice.delete()
        return HttpResponse(status=204)


class InvoiceCreateList(View):

    def get(self, request):

        invoices = Invoice.objects.all()
        serializer = InvoiceSerializer(invoices, many=True)
        return HttpResponse(json.dumps(serializer.data))

    def post(self, request):

        dict_invoice = {}

        dict_seller = {}

        dict_receiver = {}

        json_dict = json.loads(request.body)

        # access_key, uf_code_seller, cnpj_seller, number

        access_key = json_dict['main_access_key'].replace(' ', '')

        uf_code_seller = access_key[0:2]

        cnpj_seller = access_key[6:20]

        number = access_key[25:34]

        dict_invoice['access_key'] = access_key
        dict_invoice['number'] = number

        dict_seller['uf_code'] = uf_code_seller
        dict_seller['cnpj'] = cnpj_seller

        # cpf_cnpj_receiver

        cpf_cnpj_receiver = json_dict['sender_cnpj_cpf']

        cpf_cnpj_receiver = re.search(
                r'\d{11}|\d{14}|\d{3}\.\d{3}\.\d{3}\-\d{2}|\d{2}\.\d{3}\.\d{3}\/\d{4}\-\d{2}',
                cpf_cnpj_receiver,
                re.M | re.I
                )

        cpf_cnpj_receiver = str(cpf_cnpj_receiver.group())
        cpf_cnpj_receiver = cpf_cnpj_receiver.replace('-', '')
        cpf_cnpj_receiver = cpf_cnpj_receiver.replace('.', '')
        cpf_cnpj_receiver = cpf_cnpj_receiver.replace('/', '')
        cpf_cnpj_receiver = cpf_cnpj_receiver.replace(' ', '')

        dict_receiver['cpf_cnpj'] = cpf_cnpj_receiver

        # operation_nature

        dict_invoice['operation_nature'] = json_dict['main_nature_operation']

        # authorization_protocol

        dict_invoice['authorization_protocol'] = json_dict['main_protocol_authorization_use']

        # state_registration

        dict_invoice['state_registration'] = json_dict['main_state_registration']

        # emission_date

        emission_date = json_dict['sender_emission_date']

        emission_date = re.search(r'\d{2}\/\d{2}\/\d{4}', emission_date, re.M | re.I)

        emission_date = str(emission_date.group())

        emission_date = emission_date.split('/')

        emission_date = emission_date[2] + '-' + emission_date[1] + '-' + emission_date[0]

        dict_invoice['emission_date'] = emission_date

        # entry_exit_datetime

        entry_exit_datetime = json_dict['sender_out_input_date']

        entry_exit_datetime = entry_exit_datetime.split('/')

        time = json_dict['sender_output_time']

        u = entry_exit_datetime[2] + '-' + entry_exit_datetime[1] + '-' + entry_exit_datetime[0] + 'T' + time

        entry_exit_datetime = u

        dict_invoice['entry_exit_datetime'] = entry_exit_datetime

        # total_products_value

        total_products_value = json_dict['tax_total_cost_products']

        total_products_value = total_products_value.replace('.', '')
        total_products_value = total_products_value.replace(',', '.')

        dict_invoice['total_products_value'] = float(total_products_value)

        # total_invoice_value

        total_invoice_value = json_dict['tax_cost_total_note']

        total_invoice_value = total_invoice_value.replace('.', '')
        total_invoice_value = total_invoice_value.replace(',', '.')

        dict_invoice['total_invoice_value'] = float(total_invoice_value)

        # basis_calculation_icms

        basis_calculation_icms = json_dict['tax_icms_basis']

        basis_calculation_icms = basis_calculation_icms.replace('.', '')
        basis_calculation_icms = basis_calculation_icms.replace(',', '.')

        dict_invoice['basis_calculation_icms'] = float(basis_calculation_icms)

        # freight_value

        freight_value = json_dict['tax_cost_freight']

        freight_value = freight_value.replace('.', '')
        freight_value = freight_value.replace(',', '.')

        dict_invoice['freight_value'] = float(freight_value)

        #  insurance_value

        insurance_value = json_dict['tax_cost_insurance']

        insurance_value = insurance_value.replace('.', '')
        insurance_value = insurance_value.replace(',', '.')

        dict_invoice['insurance_value'] = float(insurance_value)

        # icms_value

        icms_value = json_dict['tax_cost_icms']

        icms_value = icms_value.replace('.', '')
        icms_value = icms_value.replace(',', '.')

        dict_invoice['icms_value'] = float(icms_value)

        # discount_value

        discount_value = json_dict['tax_discount']

        discount_value = discount_value.replace('.', '')
        discount_value = discount_value.replace(',', '.')

        dict_invoice['discount_value'] = float(discount_value)

        # basis_calculation_icms_st

        basis_calculation_icms_st = json_dict['tax_icms_basis_st']

        basis_calculation_icms_st = basis_calculation_icms_st.replace('.', '')
        basis_calculation_icms_st = basis_calculation_icms_st.replace(',', '.')

        dict_invoice['basis_calculation_icms_st'] = float(basis_calculation_icms_st)

        # icms_value_st

        icms_value_st = json_dict['tax_cost_icms_replacement']

        icms_value_st = icms_value_st.replace('.', '')
        icms_value_st = icms_value_st.replace(',', '.')

        dict_invoice['icms_value_st'] = float(icms_value_st)

        # other_expenditure

        other_expenditure = json_dict['tax_other_expenditure']

        other_expenditure = other_expenditure.replace('.', '')
        other_expenditure = other_expenditure.replace(',', '.')

        dict_invoice['other_expenditure'] = float(other_expenditure)

        # ipi_value

        ipi_value = json_dict['tax_cost_ipi']

        ipi_value = ipi_value.replace('.', '')
        ipi_value = ipi_value.replace(',', '.')

        dict_invoice['ipi_value'] = float(ipi_value)

        # receiver

        dict_receiver['name'] = json_dict['sender_name_social']
        dict_receiver['address'] = json_dict['sender_address']
        dict_receiver['neighborhood'] = json_dict['sender_neighborhood_district']
        dict_receiver['cep'] = json_dict['sender_cep'].replace('-', '')
        dict_receiver['county'] = json_dict['sender_county']
        dict_receiver['uf'] = json_dict['sender_uf']
        dict_receiver['phone'] = json_dict['sender_phone_fax']

        # ------------------------
        if Receiver.objects.filter(cpf_cnpj=cpf_cnpj_receiver).count() == 1:
            receiver = Receiver.objects.get(cpf_cnpj=cpf_cnpj_receiver)
            dict_invoice['receiver'] = receiver.pk
        else:
            receiver_serializer = ReceiverSerializer(data=dict_receiver)

            if receiver_serializer.is_valid():
                receiver_serializer.save()
            else:
                return HttpResponse(
                    json.dumps([
                        receiver_serializer.errors,
                        ]),
                    status=400
                )
            dict_invoice['receiver'] = receiver_serializer.data['id']
            
        if Seller.objects.filter(cnpj=cnpj_seller).count() == 1:
            seller = Seller.objects.get(cnpj=cnpj_seller)
            dict_invoice['seller'] = seller.pk
        else:
            seller_serializer = SellerSerializer(data=dict_seller)

            if seller_serializer.is_valid():
                seller_serializer.save()
            else:
                return HttpResponse(
                    json.dumps([
                        seller_serializer.errors,
                        ]),
                    status=400
                )

            dict_invoice['seller'] = seller_serializer.data['id']

        invoice_serializer = InvoiceSerializer(data=dict_invoice)

        if invoice_serializer.is_valid():
            invoice_serializer.save()
        else:
            return HttpResponse(
                json.dumps(
                    invoice_serializer.errors
                    ),
                status=400
            )

        return HttpResponse(
            json.dumps([
                invoice_serializer.data,
            ]),
            status=200
        )


def sellerShow(request, pk):
    if request.method == 'GET':
        seller = get_object_seller(pk)
        serializer = SellerSerializer(seller)
        return HttpResponse(json.dumps(serializer.data), status=200)
    return HttpResponse(status=400)


def receiverShow(request, pk):
    if request.method == 'GET':
        receiver = get_object_receiver(pk)
        serializer = ReceiverSerializer(receiver)
        return HttpResponse(json.dumps(serializer.data), status=200)
    return HttpResponse(status=400)


def sellerList(request):
    if request.method == 'GET':
        seller = Seller.objects.all()
        serializer = SellerSerializer(seller, many=True)
        return HttpResponse(json.dumps(serializer.data))
    return HttpResponse(status=400)


def receiverList(request):
    if request.method == 'GET':
        receiver = Receiver.objects.all()
        serializer = ReceiverSerializer(receiver, many=True)
        return HttpResponse(json.dumps(serializer.data))
    return HttpResponse(status=400)
