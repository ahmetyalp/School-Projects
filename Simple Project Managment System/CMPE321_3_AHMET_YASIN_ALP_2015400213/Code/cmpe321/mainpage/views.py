from django.shortcuts import render,redirect
from django.http import HttpResponse
from .forms import MainPageForm, ListForm
import psycopg2

def _list(request):
    if request.session.get('user') is None:
        return redirect('/login')
    if request.method == 'POST':
        form = ListForm(request.POST)
        if form.is_valid():
            pmname = form.cleaned_data['pmname']
            if request.session.get('admin') != 1 and pmname != request.session.get('user'):
                return HttpResponse('You can see only your projects!')
            conn = psycopg2.connect("dbname=database.db user=postgres password=1234")
            c = conn.cursor()
            t = (pmname , )
            c.execute("SELECT * FROM listcomp(%s)" , t)
            complist = c.fetchall()
            c.execute("SELECT * FROM listuncomp(%s)" , t)
            uncomplist = c.fetchall()
            return render( request , 'mainpage/showlist.html' , {'complist' : complist , 'uncomplist' : uncomplist})
        else:
            return HttpResponse('404')
    else:
        form = ListForm()
        return render(request , 'mainpage/list.html' , { 'form' : form } )

def index(request):
    if request.session.get('user') is None:
        return redirect('/login')
    if request.method == 'POST':
        if '_logout' in request.POST:
            form = MainPageForm(request.POST)
            if form.is_valid():
                del request.session['user']
                del request.session['admin']
                return redirect('/login')
            else:
                return HttpResponse('404')
        elif '_addPM'  in request.POST:
            form = MainPageForm(request.POST)
            if form.is_valid():
                return redirect('/edit/addPM')
            else:
                return HttpResponse('404')
        elif '_editPM'  in request.POST:
            form = MainPageForm(request.POST)
            if form.is_valid():
                return redirect('/edit/editPM')
            else:
                return HttpResponse('404')
        elif '_delPM'  in request.POST:
            form = MainPageForm(request.POST)
            if form.is_valid():
                return redirect('/edit/delPM')
            else:
                return HttpResponse('404')
        elif '_addPR'  in request.POST:
            form = MainPageForm(request.POST)
            if form.is_valid():
                return redirect('/edit/addPR')
            else:
                return HttpResponse('404')
        elif '_editPR'  in request.POST:
            form = MainPageForm(request.POST)
            if form.is_valid():
                return redirect('/edit/editPR')
            else:
                return HttpResponse('404')
        elif '_delPR'  in request.POST:
            form = MainPageForm(request.POST)
            if form.is_valid():
                return redirect('/edit/delPR')
            else:
                return HttpResponse('404')
        elif '_addE'  in request.POST:
            form = MainPageForm(request.POST)
            if form.is_valid():
                return redirect('/edit/addE')
            else:
                return HttpResponse('404')
        elif '_editE'  in request.POST:
            form = MainPageForm(request.POST)
            if form.is_valid():
                return redirect('/edit/editE')
            else:
                return HttpResponse('404')
        elif '_delE'  in request.POST:
            form = MainPageForm(request.POST)
            if form.is_valid():
                return redirect('/edit/delE')
            else:
                return HttpResponse('404')
        elif '_addT'  in request.POST:
            form = MainPageForm(request.POST)
            if form.is_valid():
                return redirect('/edit/addT')
            else:
                return HttpResponse('404')
        elif '_editT'  in request.POST:
            form = MainPageForm(request.POST)
            if form.is_valid():
                return redirect('/edit/editT')
            else:
                return HttpResponse('404')
        elif '_delT'  in request.POST:
            form = MainPageForm(request.POST)
            if form.is_valid():
                return redirect('/edit/delT')
            else:
                return HttpResponse('404')
        elif '_listprojects' in request.POST:
            form = MainPageForm(request.POST)
            if form.is_valid():
                return redirect('list')
            else:
                return HttpResponse('404')
        elif '_assignTask' in request.POST:
            form = MainPageForm(request.POST)
            if form.is_valid():
                return redirect('/edit/assignTask')
            else:
                return HttpResponse('404')
        elif '_listTask' in request.POST:
            form = MainPageForm(request.POST)
            if form.is_valid():
                return redirect('/edit/listTask')
            else:
                return HttpResponse('404')
        elif '_listEmp' in request.POST:
            form = MainPageForm(request.POST)
            if form.is_valid():
                return redirect('/edit/listEmp')
            else:
                return HttpResponse('404')
    else:
        form = MainPageForm()
        return  render(request , 'mainpage/index.html'  , {'username' : request.session['user'] ,
         'form': form , 'admin' : request.session['admin']})

