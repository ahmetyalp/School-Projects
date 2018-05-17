from django import forms

class AddPMForm(forms.Form):
    user_name = forms.CharField(label = 'User Name' , max_length=100)
    pasw = forms.CharField(label = 'Password' , widget = forms.PasswordInput)

class EditPMForm(forms.Form):
    user_name = forms.CharField(label = 'User Name' , max_length=100)
    pasw = forms.CharField(label = 'New Password' , widget = forms.PasswordInput)

class DelPMForm(forms.Form):
    user_name = forms.CharField(label = 'User Name' , max_length=100)

class AddPRForm(forms.Form):
    name = forms.CharField(label = 'Project Name')
    startdate = forms.CharField(label = 'Start Date')
    ettd = forms.IntegerField(label = 'Estimated total task-days')
    assigned_pms = forms.CharField(label = "Name of the project managers who will be assigned to this project.(Please use comma to seperate multiple names. Write 'None' to assign nobody except auto assign.)")

class EditPRForm(forms.Form):
    name = forms.CharField(label = 'Project Name')
    ettd = forms.IntegerField(label = 'New Estimated total task-days')

class DelPRForm(forms.Form):
    name = forms.CharField(label = 'Project Name')

class AddEForm(forms.Form):
    id = forms.IntegerField(label = 'Employee Number')
    name = forms.CharField(label = 'Employee Name')

class EditEForm(forms.Form):
    id = forms.IntegerField(label = 'Employee Number')
    name = forms.CharField(label = 'New Employee Name')

class DelEForm(forms.Form):
    id = forms.IntegerField(label = 'Employee Number')

class AddTForm(forms.Form):
    name = forms.CharField(label = 'Task Name')
    pname = forms.CharField(label = 'Project Name')
    startdate = forms.CharField(label = 'Start Date')
    finishdate  = forms.CharField(label = 'Finish Date')

class EditTForm(forms.Form):
    prename = forms.CharField(label = 'Old Task Name')
    pname = forms.CharField(label = 'Project Name')
    name = forms.CharField(label = 'New Task Name')
    finishdate  = forms.CharField(label = 'New Finish Date')

class DelTForm(forms.Form):
    name = forms.CharField(label = 'Task Name')
    pname = forms.CharField(label = 'Project Name')

class AssignTaskForm(forms.Form):
    name = forms.CharField(label = 'Task Name')
    pname = forms.CharField(label = 'Project Name')
    id = forms.IntegerField(label = 'Employee Number')

class listTaskForm(forms.Form):
    pname = forms.CharField(label = 'Project Name')

    
