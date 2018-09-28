from django.db import models
from django.core.validators import (
    RegexValidator,
    MinLengthValidator,
    MaxLengthValidator,
)

# Create your models here.


class Seller(models.Model):

        name = models.CharField(
            max_length=255,
            null=False
        )

        cnpj = models.CharField(
            max_length=14,
            null=False,
            validators=[
                MinLengthValidator(14),
                MaxLengthValidator(14),
                RegexValidator(
                    regex=r'\d'
                )
            ]
        )

        cep = models.CharField(
            max_length=8,
            blank=True,
            null=False,
            validators=[
                MinLengthValidator(8),
                MaxLengthValidator(8),
                RegexValidator(
                    regex=r'\d'
                )
            ]
        )


class Receiver(models.Model):

        name = models.CharField(
            max_length=255,
            null=False
        )

        cpf_cnpj = models.CharField(
            max_length=14,
            unique=True,
            null=False
        )

        address = models.CharField(
            max_length=255,
            null=False
        )

        neighborhood = models.CharField(
            max_length=255,
            null=False
        )

        cep = models.CharField(
            max_length=8,
            null=False,
            validators=[
                MinLengthValidator(8),
                MaxLengthValidator(8),
                RegexValidator(
                    regex=r'\d'
                )
            ]
        )

        county = models.CharField(
            max_length=255,
            null=False
        )

        uf = models.CharField(
            max_length=2,
            null=False,
            validators=[
                MinLengthValidator(2),
                MaxLengthValidator(2)
            ]
        )

        phone = models.CharField(
            max_length=13,
            null=False,
            validators=[
                MinLengthValidator(8),
                MaxLengthValidator(13),
                RegexValidator(
                    regex=r'\d'
                )
            ]
        )


class Invoice(models.Model):

    pdf = models.FileField(
        upload_to='invoice/images/',
        null=False,
        validators=[
            RegexValidator(
                regex=r'.pdf$'
                )
        ]
    )

    text = models.TextField(
        null=False
    )

    number = models.CharField(
        max_length=9,
        null=False,
        validators=[
            MinLengthValidator(9),
            MaxLengthValidator(9),
            RegexValidator(
                regex=r'\d'
            )
        ]
    )

    operation_nature = models.CharField(
        max_length=255,
        null=False
    )  # NATUREZA DA OPERAÇÃO

    authorization_protocol = models.CharField(
        max_length=255,
        null=False
    )  # PROTOCOLO DE AUTORIZAÇÃO

    acess_key = models.CharField(
        max_length=44,
        unique=True,
        null=False,
        validators=[
            MinLengthValidator(44),
            MaxLengthValidator(44),
            RegexValidator(
                regex=r'\d'
            )
        ]
    )  # CHAVE DE ACESSO

    state_registration = models.CharField(
        max_length=255,
        null=False
    )  # INSCRIÇÃO ESTADUAL

    receiver = models.ForeignKey(
        'invoices.Receiver',
        related_name='invoices',
        on_delete=models.CASCADE
    )

    seller = models.ForeignKey(
        'invoices.Seller',
        related_name='invoices',
        on_delete=models.CASCADE
    )

    emission_date = models.DateField(
        null=False
    )

    entry_exit_datetime = models.DateField(
        null=False
    )

    total_products_value = models.CharField(
        max_length=30,
        null=False
    )

    total_invoice_value = models.CharField(
        max_length=30,
        null=False
    )

    freight_value = models.CharField(
        max_length=30,
        null=False,
        default=',00'
    )

    icms_value = models.CharField(
        max_length=30,
        null=False,
        default=',00'
    )

    discount_value = models.CharField(
        max_length=30,
        null=False,
        default=',00'
    )


class ProductService(models.Model):

        code = models.CharField(
            max_length=100,
            null=False
        )

        description = models.CharField(
            max_length=255
        )

        qtd = models.IntegerField(
            null=False
        )

        unity_value = models.CharField(
            max_length=30,
            null=False
        )

        total_value = models.CharField(
            max_length=30,
            null=False
        )

        discount_value = models.CharField(
            max_length=30,
            null=False
        )
