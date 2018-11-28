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
from django.views import View
from django.http import Http404


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

        json_dict = None
        if request.body:
            json_dict = json.loads(request.body)
        elif request.POST:
            json_dict = request.POST

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


def productList(request):
    if request.method == 'GET':
        product_service = Product_Service.objects.all()
        serializer = ProductServiceSerializer(product_service, many=True)
        return HttpResponse(json.dumps(serializer.data))
    return HttpResponse(status=400)


def chart_total_value_per_time(request):
    if request.method == 'GET':
        invoices = Invoice.objects.all()
        date = []
        total = []
        for invoice in invoices:
            date.append(invoice.emission_date)
            total.append(invoice.total_invoice_value)

        df = pd.DataFrame({'date': date, 'total': total})
        df = df.sort_values(by='date')
        sf = df.groupby('date')['total'].sum()
        df = pd.DataFrame({'date': sf.index, 'total': sf.values})
        df['date'] = pd.to_datetime(df['date']).apply(lambda x: x.strftime('%d/%m/%Y'))
        df['total'] = pd.to_numeric(df['total'].apply(lambda x: round(x, 2)))
        data = df.to_dict('list')

        df = pd.DataFrame({'dateM': date, 'totalM': total})
        df = df.sort_values(by='dateM')
        df['dateM'] = pd.to_datetime(df['dateM']).apply(lambda x: x.strftime('%Y-%m'))
        sf = df.groupby('dateM')['totalM'].sum()
        df = pd.DataFrame({'dateM': sf.index, 'totalM': sf.values})
        df['dateM'] = pd.to_datetime(df['dateM']).apply(lambda x: x.strftime('%m/%Y'))
        df['totalM'] = pd.to_numeric(df['totalM'].apply(lambda x: round(x, 2)))

        data['dateM'] = df.to_dict('list')['dateM']
        data['totalM'] = df.to_dict('list')['totalM']

        df = pd.DataFrame({'dateY': date, 'totalY': total})
        df = df.sort_values(by='dateY')
        df['dateY'] = pd.to_datetime(df['dateY']).apply(lambda x: x.strftime('%Y'))
        sf = df.groupby('dateY')['totalY'].sum()
        df = pd.DataFrame({'dateY': sf.index, 'totalY': sf.values})
        df['totalY'] = pd.to_numeric(df['totalY'].apply(lambda x: round(x, 2)))

        data['dateY'] = df.to_dict('list')['dateY']
        data['totalY'] = df.to_dict('list')['totalY']

        return HttpResponse(json.dumps(data))
    return HttpResponse(status=400)


def chart_qtd_per_time(request):
    if request.method == 'GET':
        invoices = Invoice.objects.all()
        date = []
        for invoice in invoices:
            date.append(invoice.emission_date)

        df = pd.DataFrame({'date': date})
        df = df.sort_values(by='date')
        df['date'] = pd.to_datetime(df['date']).apply(lambda x: x.strftime('%Y-%m'))
        sf = df.groupby('date').size()
        df = pd.DataFrame({'date': sf.index, 'count': sf.values})
        df['date'] = pd.to_datetime(df['date']).apply(lambda x: x.strftime('%m/%Y'))

        data = df.to_dict('list')

        dfY = pd.DataFrame({'dateY': date})
        dfY = dfY.sort_values(by='dateY')
        dfY['dateY'] = pd.to_datetime(dfY['dateY']).apply(lambda x: x.strftime('%Y'))
        sf = dfY.groupby('dateY').size()
        dfY = pd.DataFrame({'dateY': sf.index, 'countY': sf.values})

        data['dateY'] = dfY.to_dict('list')['dateY']
        data['countY'] = dfY.to_dict('list')['countY']

        return HttpResponse(json.dumps(data))
    return HttpResponse(status=400)


def chart_total_value_per_chosen_date(request):
    if request.method == 'GET':
        invoices = Invoice.objects.all()
        date = []
        total = []
        for invoice in invoices:
            date.append(invoice.emission_date)
            total.append(invoice.total_invoice_value)

        df = pd.DataFrame({'date': date, 'total': total})
        df = df.sort_values(by='date')
        sf = df.groupby('date')['total'].sum()
        df = pd.DataFrame({'date': sf.index, 'total': sf.values})

        week = dt.datetime.now().date() - dt.timedelta(days=7)
        month = dt.datetime.now().date() - dt.timedelta(days=30)
        semester = dt.datetime.now().date() - dt.timedelta(days=182)
        year = dt.datetime.now().date() - dt.timedelta(days=365)

        yearDf = df.groupby('date').filter(lambda x: (x['date'] > year))
        semesterDf = yearDf.groupby('date').filter(lambda x: (x['date'] > semester))
        monthDf = semesterDf.groupby('date').filter(lambda x: (x['date'] > month))
        weekDf = monthDf.groupby('date').filter(lambda x: (x['date'] > week))

        sf = weekDf.groupby('date')['total'].mean()
        weekDf = pd.DataFrame({'date': sf.index, 'total': sf.values})
        sf = monthDf.groupby('date')['total'].mean()
        monthDf = pd.DataFrame({'date': sf.index, 'total': sf.values})
        sf = semesterDf.groupby('date')['total'].mean()
        semesterDf = pd.DataFrame({'date': sf.index, 'total': sf.values})
        sf = yearDf.groupby('date')['total'].mean()
        yearDf = pd.DataFrame({'date': sf.index, 'total': sf.values})

        weekDf['date'] = pd.to_datetime(weekDf['date']).apply(lambda x: x.strftime('%d/%m/%Y'))
        monthDf['date'] = pd.to_datetime(monthDf['date']).apply(lambda x: x.strftime('%d/%m/%Y'))
        semesterDf['date'] = pd.to_datetime(semesterDf['date']).apply(lambda x: x.strftime('%d/%m/%Y'))
        yearDf['date'] = pd.to_datetime(yearDf['date']).apply(lambda x: x.strftime('%d/%m/%Y'))

        weekDf['total'] = pd.to_numeric(weekDf['total'].apply(lambda x: round(x, 2)))
        monthDf['total'] = pd.to_numeric(monthDf['total'].apply(lambda x: round(x, 2)))
        semesterDf['total'] = pd.to_numeric(semesterDf['total'].apply(lambda x: round(x, 2)))
        yearDf['total'] = pd.to_numeric(yearDf['total'].apply(lambda x: round(x, 2)))

        data = {}
        data['dateW'] = weekDf.to_dict('list')['date']
        data['totalW'] = weekDf.to_dict('list')['total']
        data['dateM'] = monthDf.to_dict('list')['date']
        data['totalM'] = monthDf.to_dict('list')['total']
        data['dateS'] = semesterDf.to_dict('list')['date']
        data['totalS'] = semesterDf.to_dict('list')['total']
        data['dateY'] = yearDf.to_dict('list')['date']
        data['totalY'] = yearDf.to_dict('list')['total']

        return HttpResponse(json.dumps(data))
    return HttpResponse(status=400)


def chart_total_value_current(request):
    if request.method == 'GET':
        invoices = Invoice.objects.all()
        date = []
        total = []
        for invoice in invoices:
            date.append(invoice.emission_date)
            total.append(invoice.total_invoice_value)

        df = pd.DataFrame({'date': date, 'total': total})
        df = df.sort_values(by='date')
        sf = df.groupby('date')['total'].sum()
        df = pd.DataFrame({'date': sf.index, 'total': sf.values})
        df['total'] = pd.to_numeric(df['total'].apply(lambda x: round(x, 2)))

        current_year = dt.date(
            dt.datetime.now().year,
            1,
            1
        )
        current_month = dt.date(
            dt.datetime.now().year,
            dt.datetime.now().month,
            1
        )

        df = df.groupby('date').filter(lambda x: (x['date'] > current_year))
        sf = df.groupby('date')['total'].mean()
        df = pd.DataFrame({'date': sf.index, 'total': sf.values})

        dfM = df.groupby('date').filter(lambda x: (x['date'] > current_month))
        sf = dfM.groupby('date')['total'].mean()
        dfM = pd.DataFrame({'date': sf.index, 'total': sf.values})
        df['date'] = pd.to_datetime(df['date']).apply(lambda x: x.strftime('%d/%m/%Y'))
        dfM['date'] = pd.to_datetime(dfM['date']).apply(lambda x: x.strftime('%d/%m/%Y'))

        data = df.to_dict('list')
        data['dateM'] = dfM.to_dict('list')['date']
        data['totalM'] = dfM.to_dict('list')['total']

        return HttpResponse(json.dumps(data))
    return HttpResponse(status=400)


def chart_total_value_per_category(request):
    if request.method == 'GET':
        invoices = Invoice.objects.all()
        category = []
        total = []
        for invoice in invoices:
            category.append(invoice.operation_nature)
            total.append(invoice.total_invoice_value)

        df = pd.DataFrame({'category': category, 'total': total})
        df = df.sort_values(by='category')
        sf = df.groupby('category')['total'].sum()
        df = pd.DataFrame({'category': sf.index, 'total': sf.values})
        df['total'] = pd.to_numeric(df['total'].apply(lambda x: round(x, 2)))

        data = df.to_dict('list')

        return HttpResponse(json.dumps(data))
    return HttpResponse(status=400)


def chart_freight_value_per_date(request):
    if request.method == 'GET':
        invoices = Invoice.objects.all()
        date = []
        freight = []
        for invoice in invoices:
            date.append(invoice.emission_date)
            freight.append(invoice.freight_value)

        df = pd.DataFrame({'date': date, 'freight': freight})
        df = df.sort_values(by='date')
        sf = df.groupby('date')['freight'].sum()
        df = pd.DataFrame({'date': sf.index, 'freight': sf.values})
        df['date'] = pd.to_datetime(
            df['date']).apply(lambda x: x.strftime('%d/%m/%Y'))
        df['freight'] = pd.to_numeric(
            df['freight'].apply(lambda x: round(x, 2)))

        data = df.to_dict('list')

        return HttpResponse(json.dumps(data))
    return HttpResponse(status=400)


def chart_total_valueBySeller(request):
    if request.method == 'GET':
        invoices = Invoice.objects.all()
        sellers = []
        total = []
        for invoice in invoices:
            sellers.append(invoice.seller)
            total.append(invoice.total_invoice_value)

        for i, seller in enumerate(sellers):
            sellers[i] = seller.cnpj

        df = pd.DataFrame({'total': total, 'seller': sellers})
        df = df.sort_values(by='total')
        sf = df.groupby('seller')['total'].sum()
        df = pd.DataFrame({'seller': sf.index, 'total': sf.values})
        df['total'] = pd.to_numeric(df['total'].apply(lambda x: round(x, 2)))
        df = df.sort_values(by='total')

        data = df.to_dict('list')

        return HttpResponse(json.dumps(data))
    return HttpResponse(status=400)


def information_invoices(request):
    if request.method == 'GET':
        invoices = Invoice.objects.all()
        date = []
        total = []
        for invoice in invoices:
            date.append(invoice.emission_date)
            total.append(invoice.total_invoice_value)

        df = pd.DataFrame({'date': date, 'total': total})
        df = df.sort_values(by='date')
        sf = df.groupby('date')['total'].sum()
        df = pd.DataFrame({'date': sf.index, 'total': sf.values})

        current_month = dt.date(dt.datetime.now().year, dt.datetime.now().month, 1)

        df = df.groupby('date').filter(lambda x: (x['date'] > current_month))
        total = 0
        for x in df['total']:
            total += x

        total = round(total, 2)
        data = {}
        data['totalM'] = total

        df = pd.DataFrame({'date': date, 'total': total})
        df = df.sort_values(by='date')
        sf = df.groupby('date')['total'].sum()
        df = pd.DataFrame({'date': sf.index, 'total': sf.values})

        current_year = dt.date(dt.datetime.now().year,1,1)

        df = df.groupby('date').filter(lambda x: (x['date'] > current_year))
        sf = df.groupby('date')['total'].mean()

        df = pd.DataFrame({'date': sf.index, 'total': sf.values})

        sf = df.groupby('date').size()
        df = pd.DataFrame({'date': sf.index, 'count': sf.values})

        total_qtd_year = 0
        for x in df['count']:
            total_qtd_year += x

        df = df.groupby('date').filter(lambda x: (x['date'] > current_month))

        total_qtd_month = 0
        for x in df['count']:
            total_qtd_month += x

        data['total_qtd_year'] = total_qtd_year
        data['total_qtd_month'] = total_qtd_month

        sellers = []
        for invoice in invoices:
            sellers.append(invoice.seller)

        for i, seller in enumerate(sellers):
            sellers[i] = seller.cnpj

        df = pd.DataFrame({'sellers': sellers})
        df = df.sort_values(by='sellers')
        sf = df.groupby('sellers').size()
        df = pd.DataFrame({'sellers': sf.index, 'count': sf.values})

        sellersA = []
        countA = []
        for x in df['sellers']:
            sellersA.append(x)

        for x in df['count']:
            countA.append(x)

        data['sellers'] = sellersA
        data['count'] = countA

        return HttpResponse(json.dumps(data))
    return HttpResponse(status=400)
