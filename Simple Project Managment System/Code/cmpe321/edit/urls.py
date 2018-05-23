from django.urls import path

from . import views

urlpatterns = [
    path('addPM/', views.addPM, name='addPM'),
    path('editPM/', views.editPM, name='editPM'),
    path('delPM/', views.delPM, name='delPM'),
    path('addPR/', views.addPR, name='addPR'),
    path('editPR/', views.editPR, name='editPR'),
    path('delPR/', views.delPR, name='delPR'),
    path('addE/', views.addE, name='addE'),
    path('editE/', views.editE, name='editE'),
    path('delE', views.delE, name='delE'),
    path('addT/', views.addT, name='addT'),
    path('editT/', views.editT, name='editT'),
    path('delT/', views.delT, name='delT'),
    path('assignTask/' , views.assignTask , name='assignTask'),
    path('listTask/' , views.listTask , name='listTask'),
    path('listEmp/' , views.listEmployees , name='listEmp'),
]