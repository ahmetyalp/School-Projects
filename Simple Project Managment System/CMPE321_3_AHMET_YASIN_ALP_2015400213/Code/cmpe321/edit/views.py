from django.shortcuts import render,redirect
from django.http import HttpResponse
from .forms import AddPMForm,EditPMForm,DelPMForm,AddPRForm,EditPRForm,DelPRForm,AddEForm,EditEForm,DelEForm,AddTForm,EditTForm,DelTForm,AssignTaskForm,listTaskForm
import psycopg2
import re


def addPM(request):
    if request.session.get('user') is None:
        return redirect('/login')
    if request.session.get('admin') != 1:
        return HttpResponse('Please login as Admin')
    if request.method == 'POST':
        form = AddPMForm(request.POST)
        if form.is_valid():
            conn = psycopg2.connect("dbname=database.db user=postgres password=1234")
            c = conn.cursor()
            t = (form.cleaned_data['user_name'],form.cleaned_data['pasw'],)
            c.execute("INSERT INTO Users VALUES(%s,%s,0)" , t)
            t = (form.cleaned_data['user_name'],)
            c.execute("INSERT INTO NoP VALUES(%s , 0)" , t)
            conn.commit()
            c.close()
            conn.close()
            return redirect('/mainpage')
        else:
            return HttpResponse('404')
    else:
        form = AddPMForm()
        return render(request , 'edit/addPM.html' , {'form' : form})

def editPM(request):
    if request.session.get('user') is None:
        return redirect('/login')
    if request.session.get('admin') != 1:
        return HttpResponse('Please login as Admin')
    if request.method == 'POST':
        form = EditPMForm(request.POST)
        if form.is_valid():
            conn = psycopg2.connect("dbname=database.db user=postgres password=1234")
            c = conn.cursor()
            t = (form.cleaned_data['pasw'],form.cleaned_data['user_name'],)
            c.execute("UPDATE Users SET password = %s WHERE username = %s", t)
            conn.commit()
            c.close()
            conn.close()
            return redirect('/mainpage')
        else:
            return HttpResponse('404')
    else:
        form = EditPMForm()
        return render(request , 'edit/editPM.html' , {'form' : form})

def delPM(request):
    if request.session.get('user') is None:
        return redirect('/login')
    if request.session.get('admin') != 1:
        return HttpResponse('Please login as Admin')
    if request.method == 'POST':
        form = DelPMForm(request.POST)
        if form.is_valid():
            conn = psycopg2.connect("dbname=database.db user=postgres password=1234")
            c = conn.cursor()
            t = (form.cleaned_data['user_name'] ,)
            c.execute("DELETE FROM Users WHERE username = %s" , t)
            c.execute("DELETE FROM NoP WHERE pmname = %s" , t)
            c.execute("DELETE FROM PPM WHERE pmname = %s" , t)
            conn.commit()
            c.close()
            conn.close()
            return redirect('/mainpage')
        else:
            return HttpResponse('404')
    else:
        form = DelPMForm()
        return render(request , 'edit/delPM.html' , {'form' : form})

def addPR(request):
    if request.session.get('user') is None:
        return redirect('/login')
    if request.session.get('admin') != 1:
        return HttpResponse('Please login as Admin')
    if request.method == 'POST':
        form = AddPRForm(request.POST)
        if form.is_valid():
            conn = psycopg2.connect("dbname=database.db user=postgres password=1234")
            c = conn.cursor()
            t = (form.cleaned_data['name'],form.cleaned_data['startdate'],form.cleaned_data['ettd'],)
            c.execute("INSERT INTO Project VALUES(%s,%s,%s)" , t)
            s = form.cleaned_data['assigned_pms'].split(',')
            if s[0] != 'None':
                for pm in s:
                    t = (form.cleaned_data['name'],pm,)
                    c.execute("SELECT * FROM PPM WHERE pname = %s AND pmname = %s" , t)
                    if len(c.fetchall()) > 0:
                        continue
                    c.execute("INSERT INTO PPM VALUES(%s,%s)",t)
            conn.commit()
            c.close()
            conn.close()
            return redirect('/mainpage')
        else:
            return HttpResponse('404')
    else:
        form = AddPRForm()
        return render(request , 'edit/addPR.html' , {'form' : form})

def editPR(request):
    if request.session.get('user') is None:
        return redirect('/login')
    if request.session.get('admin') != 1:
        return HttpResponse('Please login as Admin')
    if request.method == 'POST':
        form = EditPRForm(request.POST)
        if form.is_valid():
            conn = psycopg2.connect("dbname=database.db user=postgres password=1234")
            c = conn.cursor()
            t = (form.cleaned_data['ettd'] , form.cleaned_data['name'] , )
            c.execute("UPDATE Project SET ettd = %s WHERE project_name = %s" , t)
            conn.commit()
            c.close()
            conn.close()
            return redirect('/mainpage')
        else:
            return HttpResponse('404')
    else:
        form = EditPRForm()
        return render(request , 'edit/editPR.html' , { 'form':form })

def delPR(request):
    if request.session.get('user') is None:
        return redirect('/login')
    if request.session.get('admin') != 1:
        return HttpResponse('Please login as Admin')
    if request.method == 'POST':
        form = DelPRForm(request.POST)
        if form.is_valid():
            conn = psycopg2.connect("dbname=database.db user=postgres password=1234")
            c = conn.cursor()
            t = (form.cleaned_data['name'] , )
            c.execute("DELETE FROM Project WHERE project_name = %s" , t)
            c.execute("DELETE FROM PPM WHERE pname = %s" , t)
            c.execute("SELECT tid FROM PT WHERE pname = %s " , t)
            l = c.fetchall()
            c.execute("DELETE FROM PT WHERE pname = %s" , t)
            for x in l:
                t = (x[0] , )
                c.execute("DELETE FROM Task WHERE id = %s" , t)
                c.execute("DELETE FROM ET WHERE tid = %s" , t)
            conn.commit()
            c.close()
            conn.close()
            return redirect('/mainpage')
        else:
            return HttpResponse('404')
    else:
        form = DelPRForm()
        return render(request , 'edit/delPR.html' , { 'form' : form })

def addE(request):
    if request.session.get('user') is None:
        return redirect('/login')
    if request.session.get('admin') != 1:
        return HttpResponse('Please login as Admin')
    if request.method == 'POST':
        form = AddEForm(request.POST)
        if form.is_valid():
            conn = psycopg2.connect("dbname=database.db user=postgres password=1234")
            c = conn.cursor()
            t = (form.cleaned_data['id'] , form.cleaned_data['name'] , )
            c.execute("INSERT INTO Employee VALUES(%s,%s)" , t)
            conn.commit()
            c.close()
            conn.close()
            return redirect('/mainpage')
        else:
            return HttpResponse('404')
    else:
        form = AddEForm()
        return render(request , 'edit/addE.html' , {'form' : form})

def editE(request):
    if request.session.get('user') is None:
        return redirect('/login')
    if request.session.get('admin') != 1:
        return HttpResponse('Please login as Admin')
    if request.method == 'POST':
        form = EditEForm(request.POST)
        if form.is_valid():
            conn = psycopg2.connect("dbname=database.db user=postgres password=1234")
            c = conn.cursor()
            t = (form.cleaned_data['name'] , form.cleaned_data['id']) 
            c.execute("UPDATE Employee SET name = %s WHERE id = %s" , t)
            conn.commit()
            c.close()
            conn.close()
            return redirect('/mainpage')
        else:
            return HttpResponse('404')
    else:
        form = EditEForm()
        return render( request , 'edit/editE.html' , {'form' : form})

def delE(request):
    if request.session.get('user') is None:
        return redirect('/login')
    if request.session.get('admin') != 1:
        return HttpResponse('Please login as Admin')
    if request.method == 'POST':
        form = DelEForm(request.POST)
        if form.is_valid():
            conn = psycopg2.connect("dbname=database.db user=postgres password=1234")
            c = conn.cursor()
            t = (form.cleaned_data['id'] , )
            c.execute("DELETE FROM Employee WHERE id = %s" , t)
            conn.commit()
            c.close()
            conn.close()
            return redirect('/mainpage')
        else:
            return HttpResponse('404')
    else:
        form = DelEForm()
        return render( request , 'edit/delE.html' , { 'form' : form } )
  
def addT(request):
    if request.session.get('user') is None:
        return redirect('/login')
    if request.session.get('admin') != 0:
        return HttpResponse('Please login as Project Manager')
    if request.method == 'POST':
        form = AddTForm(request.POST)
        if form.is_valid():
            conn = psycopg2.connect("dbname=database.db user=postgres password=1234")
            c = conn.cursor()
            t = (form.cleaned_data['name'] , form.cleaned_data['startdate'] , form.cleaned_data['finishdate'] , form.cleaned_data['pname'] , )
            c.execute("INSERT INTO Task (name , startdate , finishdate , pname) VALUES(%s,%s,%s,%s) RETURNING id" , t)
            id = c.fetchall()[0][0]
            t = (id , form.cleaned_data['pname'])
            c.execute("INSERT INTO PT VALUES(%s , %s)" , t)
            conn.commit()
            c.close()
            conn.close()
            return redirect('/mainpage')
        else:
            return HttpResponse('404')
    else:
        form = AddTForm()
        return render(request , 'edit/addT.html' , {'form' : form})

def editT(request):
    if request.session.get('user') is None:
        return redirect('/login')
    if request.session.get('admin') != 0:
        return HttpResponse('Please login as Project Manager')
    if request.method == 'POST':
        form = EditTForm(request.POST)
        if form.is_valid():
            conn = psycopg2.connect("dbname=database.db user=postgres password=1234")
            c = conn.cursor()
            t = (form.cleaned_data['name'] , form.cleaned_data['finishdate'] , form.cleaned_data['prename'] , form.cleaned_data['pname'], )
            c.execute("UPDATE Task SET name = %s , finishdate = %s WHERE name = %s AND pname = %s" , t)
            conn.commit()
            c.close()
            conn.close()
            return redirect('/mainpage')
        else:
            return HttpResponse('404')
    else:
        form = EditTForm()
        return render( request , 'edit/editT.html' , { 'form' : form } )

def delT(request):
    if request.session.get('user') is None:
        return redirect('/login')
    if request.session.get('admin') != 0:
        return HttpResponse('Please login as Project Manager')
    if request.method == 'POST':
        form = DelTForm(request.POST)
        if form.is_valid():
            conn = psycopg2.connect("dbname=database.db user=postgres password=1234")
            c = conn.cursor()
            t = (form.cleaned_data['name'], form.cleaned_data['pname'] , )
            c.execute("DELETE FROM Task WHERE name = %s AND pname = %s RETURNING id" , t)
            _id = c.fetchall()[0][0]
            t = (_id , )
            c.execute("DELETE FROM PT WHERE tid = %s" , t)
            c.execute("DELETE FROM ET WHERE tid = %s" , t)
            conn.commit()
            c.close()
            conn.close()
            return redirect('/mainpage')
        else:
            return HttpResponse('404')
    else:
        form = DelTForm()
        return render( request , 'edit/delT.html' , {'form':form})
  

def assignTask(request):
    if request.session.get('user') is None:
        return redirect('/login')
    if request.session.get('admin') != 0:
        return HttpResponse('Please login as Project Manager')
    if request.method == 'POST':
        form = AssignTaskForm(request.POST)
        if form.is_valid():
            conn = psycopg2.connect("dbname=database.db user=postgres password=1234")
            c = conn.cursor()
            t = (form.cleaned_data['pname'] , request.session.get('user') ,)
            c.execute("SELECT * FROM PPM WHERE pname = %s AND pmname = %s" , t)
            if len(c.fetchall()) == 0:
                return HttpResponse('This is not ypur project!')
            t = (form.cleaned_data['name'] , form.cleaned_data['pname'] , )
            c.execute("SELECT startdate, finishdate FROM Task WHERE name = %s AND pname = %s " , t)
            x = c.fetchall()[0]
            t = (form.cleaned_data['id'] , )
            c.execute("SELECT startdate, finishdate FROM Task WHERE EXISTS (SELECT * FROM ET WHERE tid = id AND eid = %s)" , t)
            y = c.fetchall()
            for q in y:
                if not(q[0] > x[1] or x[0] > q[1]):
                    return HttpResponse('This Employee is Busy!')
            t = (form.cleaned_data['name'] , form.cleaned_data['pname'] , )
            c.execute("SELECT id FROM Task WHERE name = %s AND pname = %s " , t)
            x = c.fetchall()[0][0]
            t = (x , form.cleaned_data['id'] , )
            c.execute("INSERT INTO ET VALUES(%s , %s )" , t)
            conn.commit()
            c.close()
            conn.close()
            return redirect('/mainpage')
        else:
            return HttpResponse('404')
    else:
        form = AssignTaskForm()
        return render( request , 'edit/assignTask.html' , { 'form' : form } )

def listTask(request):
    if request.session.get('user') is None:
        return redirect('/login')
    if request.session.get('admin') != 0:
        return HttpResponse('Please login as Project Manager')
    if request.method == 'POST':
        form = listTaskForm(request.POST)
        if form.is_valid():
            conn = psycopg2.connect("dbname=database.db user=postgres password=1234")
            c = conn.cursor()
            t = (form.cleaned_data['pname'] , request.session.get('user') , )
            c.execute("SELECT * FROM PPM WHERE pname = %s AND pmname = %s" , t)
            if len(c.fetchall()) == 0:
                return HttpResponse('This is not ypur Project!')
            t = (form.cleaned_data['pname'] , )
            c.execute("SELECT tid FROM PT WHERE pname = %s" , t)
            ans = []
            x = c.fetchall()
            for y in x:
                t = (y[0] , )
                c.execute("SELECT * FROM Task WHERE id = %s" , t)
                ans.append(c.fetchall()[0])
            conn.commit()
            c.close()
            conn.close()
            return render( request , 'edit/showlist.html' , { 'list':ans , 'pname' : form.cleaned_data['pname'] } )
        else:
            return HttpResponse('404')
    else:
        form = listTaskForm()
        return render( request , 'edit/listTask.html' , { 'form' : form } )

def listEmployees(request):
    if request.session.get('user') is None:
        return redirect('/login')
    if request.session.get('admin') != 1:
        return HttpResponse('Please login as Admin')
    conn = psycopg2.connect("dbname=database.db user=postgres password=1234")
    c = conn.cursor()
    c.execute("SELECT * FROM Employee")
    alist = c.fetchall()
    conn.commit()
    c.close()
    conn.close()
    return render( request , 'edit/listEmployees.html' , {'list': alist})