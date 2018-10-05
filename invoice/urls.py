from django.conf.urls import url, include
from .views import InvoiceView
from rest_framework import routers


urlpatterns = [
    url(r'^invoice/', InvoiceView.as_view())
]
