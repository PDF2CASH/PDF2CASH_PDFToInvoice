from django.conf.urls import url, include
from . import views
from rest_framework import routers

router = routers.DefaultRouter()
router.register(r'invoice', views.InvoiceViewSet)
router.register(r'seller', views.SellerViewSet)
router.register(r'receiver', views.ReceiverViewSet)
router.register(r'product-service', views.ProductServiceViewSet)

urlpatterns = [
    url(r'/', include(router.urls)),
]
