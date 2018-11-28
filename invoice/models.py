from django.db import models


class Invoice(models.Model):

    number = models.CharField(max_length=9, blank=True, null=True)

    operation_nature = models.CharField(
        max_length=255, blank=True, null=True)  # NATUREZA DA OPERAÇÃO

    authorization_protocol = models.CharField(
        max_length=255, blank=True, null=True)  # PROTOCOLO DE AUTORIZAÇÃO

    access_key = models.CharField(
        max_length=44, unique=True)  # CHAVE DE ACESSO

    state_registration = models.CharField(
        max_length=255, blank=True, null=True)  # INSCRIÇÃO ESTADUAL

    receiver = models.ForeignKey(
        'invoice.Receiver', related_name='invoices', on_delete=models.CASCADE)

    seller = models.ForeignKey(
        'invoice.Seller', related_name='invoices', on_delete=models.CASCADE)

    emission_date = models.DateField(blank=True, null=True)

    entry_exit_datetime = models.DateTimeField(blank=True, null=True)

    total_products_value = models.FloatField(max_length=30, blank=True, null=True)

    total_invoice_value = models.FloatField(blank=True, null=True)

    basis_calculation_icms = models.FloatField(blank=True, null=True)

    freight_value = models.FloatField(blank=True, null=True)

    insurance_value = models.FloatField(blank=True, null=True)

    icms_value = models.FloatField(blank=True, null=True)

    discount_value = models.FloatField(blank=True, null=True)

    basis_calculation_icms_st = models.FloatField(blank=True, null=True)

    icms_value_st = models.FloatField(blank=True, null=True)

    other_expenditure = models.FloatField(blank=True, null=True)

    ipi_value = models.FloatField(blank=True, null=True)


class Seller(models.Model):

    name = models.CharField(max_length=255, blank=True, null=True)

    cnpj = models.CharField(max_length=14, blank=True, null=True)

    cep = models.CharField(max_length=8, blank=True, null=True)

    uf_code = models.IntegerField(blank=True, null=True)


class Receiver(models.Model):

    name = models.CharField(max_length=255, blank=True, null=True)

    cpf_cnpj = models.CharField(max_length=14, unique=True, null=True)

    address = models.CharField(max_length=255, blank=True, null=True)

    neighborhood = models.CharField(max_length=255, blank=True, null=True)

    cep = models.CharField(max_length=8, blank=True, null=True)

    county = models.CharField(max_length=255, blank=True, null=True)

    uf = models.CharField(max_length=2, blank=True, null=True)

    phone = models.CharField(max_length=30, blank=True, null=True)
