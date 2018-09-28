from rest_framework.response import Response
from rest_framework import viewsets, status
from .serializers import (
    InvoiceSerializer,
    SellerSerializer,
    ReceiverSerializer,
    ProductServiceSerializer
)
from .models import (
    Invoice,
    Seller,
    Receiver,
    ProductService
)
# Create your views here.


class InvoiceViewSet(viewsets.ModelViewSet):
    queryset = Invoice.objects.all()
    serializer_class = InvoiceSerializer

    def update(self, request, pk=None):
        return Response(status=status.HTTP_400_BAD_REQUEST)

    def partial_update(self, request, pk=None):
        return Response(status=status.HTTP_400_BAD_REQUEST)


class SellerViewSet(viewsets.ModelViewSet):
    queryset = Seller.objects.all()
    serializer_class = SellerSerializer

    def update(self, request, pk=None):
        return Response(status=status.HTTP_400_BAD_REQUEST)

    def partial_update(self, request, pk=None):
        return Response(status=status.HTTP_400_BAD_REQUEST)


class ReceiverViewSet(viewsets.ModelViewSet):
    queryset = Receiver.objects.all()
    serializer_class = ReceiverSerializer

    def update(self, request, pk=None):
        return Response(status=status.HTTP_400_BAD_REQUEST)

    def partial_update(self, request, pk=None):
        return Response(status=status.HTTP_400_BAD_REQUEST)


class ProductServiceViewSet(viewsets.ModelViewSet):
    queryset = ProductService.objects.all()
    serializer_class = ProductServiceSerializer

    def update(self, request, pk=None):
        return Response(status=status.HTTP_400_BAD_REQUEST)

    def partial_update(self, request, pk=None):
        return Response(status=status.HTTP_400_BAD_REQUEST)
