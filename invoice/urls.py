from . import views
from django.urls import path


urlpatterns = [
    path(r'invoice/', views.InvoiceCreateList.as_view()),
    path(r'invoice/<int:pk>/', views.InvoiceShowDelete.as_view()),
    path(r'seller/<int:pk>/', views.sellerShow),
    path(r'receiver/<int:pk>/', views.receiverShow),
    path(r'product-service/<int:pk>/', views.productShow),
    path(r'seller/', views.sellerList),
    path(r'receiver/', views.receiverList),
    path(r'product-service/', views.productList),
    path(r'char_total_value_per_time/', views.chart_total_value_per_time),
]
