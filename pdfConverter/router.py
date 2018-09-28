from rest_framework import routers
from invoices import views

router = routers.DefaultRouter()

router.register('sellers', views.SellerViewSet)
router.register('receivers', views.ReceiverViewSet)
