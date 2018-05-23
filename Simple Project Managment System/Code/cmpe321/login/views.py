from django.shortcuts import render,redirect
from django.http import HttpResponse
from .forms import LoginForm
import psycopg2


def index(request):
    if request.method == 'POST':
        form = LoginForm(request.POST)
        if form.is_valid():
            conn = psycopg2.connect("dbname=database.db user=postgres password=1234")
            c = conn.cursor()
            t = (form.cleaned_data['user_name'],form.cleaned_data['pasw'],)
            c.execute("SELECT isadmin FROM Users WHERE username = %s AND password = %s" , t)
            l = c.fetchall()
            if len(l) > 0:
                request.session['admin'] = l[0][0]
                request.session['user'] = t[0]
                c.close()
                conn.close()
                return redirect('/mainpage')
            c.close()
            conn.close()
            return HttpResponse('Wrong User Name or Password')
        else:
            return HttpResponse('In valid input')
    else:
        if request.session.get('user') is not None:
            return redirect('/mainpage')
        form = LoginForm()
        return render(request , 'login/index.html'  , {'form' : form})
    