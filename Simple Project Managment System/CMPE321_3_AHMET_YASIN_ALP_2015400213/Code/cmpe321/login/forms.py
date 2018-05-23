from django import forms

class LoginForm(forms.Form):
    user_name = forms.CharField(label = 'User Name' , max_length=100)
    pasw = forms.CharField(label = 'Password' , widget = forms.PasswordInput)
    