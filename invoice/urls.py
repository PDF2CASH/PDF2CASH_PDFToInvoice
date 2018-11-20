from . import views
from django.urls import path


urlpatterns = [
    path(r'invoice/', views.InvoiceCreateList.as_view()),
    path(r'invoice/<int:pk>/', views.InvoiceShowDelete.as_view()),
    path(r'seller/<int:pk>/', views.sellerShow),
    path(r'receiver/<int:pk>/', views.receiverShow),
    path(r'seller/', views.sellerList),
    path(r'receiver/', views.receiverList),
]
