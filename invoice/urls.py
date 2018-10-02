from django.conf.urls import url, include
from .views import InvoiceViewSet
from rest_framework import routers


router = routers.DefaultRouter()
router.register(r'invoice', InvoiceViewSet)

urlpatterns = [
    url(r'^', include(router.urls)),
]
