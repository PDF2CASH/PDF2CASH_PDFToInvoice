from django.db import models


class PDF(models.Model):

    pdf = models.FileField(upload_to= 'invoice/images/')


class Invoice(models.Model):

    pdf = models.OneToOneField(PDF, on_delete=models.CASCADE)

    file = models.FileField(upload_to= 'invoice/images/')

    text = models.TextField()

    number = models.CharField(max_length= 9)

    operation_nature = models.CharField(max_length= 255) #NATUREZA DA OPERAÇÃO

    authorization_protocol = models.CharField(max_length= 255) #PROTOCOLO DE AUTORIZAÇÃO

    access_key = models.CharField(max_length= 44, unique=True) #CHAVE DE ACESSO

    state_registration = models.CharField(max_length= 255) #INSCRIÇÃO ESTADUAL

    receiver = models.ForeignKey(
        'invoice.Receiver',
        related_name='invoices',
        on_delete=models.CASCADE
    )

    seller = models.ForeignKey(
        'invoice.Seller',
        related_name='invoices',
        on_delete=models.CASCADE
    )

    emission_date = models.DateField()

    entry_exit_datetime = models.DateField()

    total_products_value = models.CharField(max_length= 30)

    total_invoice_value = models.CharField(max_length= 30)
    
    freight_value =  models.CharField(max_length= 30)

    icms_value = models.CharField(max_length= 30)

    discount_value = models.CharField(max_length= 30)



class Seller(models.Model):

        name = models.CharField(max_length= 255)

        cnpj = models.CharField(max_length= 14)

        cep = models.CharField(max_length= 8, blank=True)

        uf_code = models.IntegerField()



class Receiver(models.Model):

        name = models.CharField(max_length= 255)

        cpf_cnpj = models.CharField(max_length= 14, unique=True)

        address = models.CharField(max_length= 255)

        neighborhood = models.CharField(max_length= 255)

        cep = models.CharField(max_length= 8)

        county = models.CharField(max_length= 255)

        uf = models.CharField(max_length= 2)

        phone = models.CharField(max_length= 13)



class Product_Service(models.Model):

        code = models.CharField(max_length= 100)

        description = models.CharField(max_length= 255)

        qtd = models.IntegerField()

        unity_value = models.CharField(max_length= 30)

        total_value = models.CharField(max_length= 30)

        discount_value = models.CharField(max_length= 30)

