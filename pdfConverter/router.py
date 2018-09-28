from rest_framework import routers
from invoices import views

router = routers.DefaultRouter()

router.register('invoices', views.InvoiceViewSet)
router.register('sellers', views.SellerViewSet)
router.register('receivers', views.ReceiverViewSet)
router.register('products-service', views.ProductServiceViewSet)
