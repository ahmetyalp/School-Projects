from django import forms

class MainPageForm(forms.Form):
    pass

class ListForm(forms.Form):
    pmname = forms.CharField(label = 'Project Manager Name')

