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
