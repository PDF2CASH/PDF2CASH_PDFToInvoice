from rest_framework import serializers
from .models import (
    Invoice,
    Seller,
    Receiver,
    Product_Service
)


class InvoiceSerializer(serializers.ModelSerializer):
    class Meta:
        model = Invoice
        fields = '__all__'


class SellerSerializer(serializers.ModelSerializer):
    class Meta:
        model = Seller
        fields = '__all__'


class ReceiverSerializer(serializers.ModelSerializer):
    class Meta:
        model = Receiver
        fields = '__all__'


class ProductServiceSerializer(serializers.ModelSerializer):
    class Meta:
        model = Product_Service
        fields = '__all__'
