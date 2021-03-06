from . import views
from django.urls import path


urlpatterns = [
    path(r'invoice/', views.InvoiceCreateList.as_view()),
    path(r'invoice/<int:pk>/', views.InvoiceShowDelete.as_view()),
    path(r'seller/<int:pk>/', views.sellerShow),
    path(r'receiver/<int:pk>/', views.receiverShow),
    path(r'seller/', views.sellerList),
    path(r'receiver/', views.receiverList),
    path(r'chart_total_value_per_time/', views.chart_total_value_per_time),
    path(r'chart_qtd_per_time/', views.chart_qtd_per_time),
    path(r'chart_total_value_per_chosen_date/', views.chart_total_value_per_chosen_date),
    path(r'chart_total_value_per_category/', views.chart_total_value_per_category),
    path(r'chart_freight_value_per_date/', views.chart_freight_value_per_date),
    path(r'chart_total_value_by_seller/', views.chart_total_value_by_seller),
    path(r'chart_total_value_current/', views.chart_total_value_current),
    path(r'information_invoices/', views.information_invoices),
]
