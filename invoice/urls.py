from django.conf.urls import url, include
from django.urls import path
from .views import *
from rest_framework import routers


router = routers.DefaultRouter()
router.register(r'invoice', InvoiceViewSet)

urlpatterns = [
    url(r'^', include(router.urls)),
]