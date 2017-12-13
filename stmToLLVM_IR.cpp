#include <iostream>
#include <cstdio>
#include <unordered_set>
#include <vector>
#include <string>
#include <map>
#include <stack>
#include <algorithm>
#include <fstream>
#include <regex>

using namespace std;

long long int var = 1;//current number of temporary variables

map<char, string> m;//map for binary operations to LLVM IR operations

unordered_set<string> var_set;// set of all variables used in stm file

ofstream out;
/* Checks syntax for variables . Only alpha-numerical with first element alphabetic allowed*/
void check_var_syntax(string str){
	if(str.size()==0)
		throw 7;
	if(str[0]>='0' and str[0]<='9')
		throw 7;
	//regex rr("[[:alnum:]]+");
	//if(!regex_match(str,rr))
    string::iterator it = str.begin();
    while ( it<str.end() ) {
        if( !isalnum(*it) )
            throw 7;
        else it++;
    }
}

/*Removes spaces in expressions*/
string remove_spaces(string str){
	string::iterator it=str.begin();
	while(it < str.end()){
		if((*it)==' ')
			it = str.erase(it);
		else if((*it) == '=')
			throw 6;
		else
			it++;
	}
	return str;
}/*
	Removes \n or \r
*/
string remove_others(string str){
	string::iterator it=str.begin();
	while(it < str.end()){
		if((*it)=='\n' or (*it)=='\r')
			it = str.erase(it);
		else
			it++;
	}
	return str;
	}
/* Calculates expression that is paranthesis free and has only same priority operators like * and / or + and - (more likely it calculates terms,actually I dont use for additions) */
string cal_exp_wout_par(string str){
	vector<string> v;// parsing string into operators,numbers and variables
	for(long int i=0;i<str.size();i++){
		if(str[i]=='+' || str[i]=='-'){
			v.push_back(str.substr(0,i));
			v.push_back(string(1,str[i]));
			str=str.substr(i+1,str.size());
			i=-1;
		}
		else if(str[i]=='*' || str[i]=='/'){
			v.push_back(str.substr(0,i));
			v.push_back(string(1,str[i]));
			str=str.substr(i+1,str.size());
			i=-1;
		}
	}
	v.push_back(str);
	for(vector<string>::iterator it = v.begin();it != v.end();it++)// If there is a empty string, it means that at least two operator is adjacent
		if((*it)=="")
			throw 2;
	while(v.size()>2){
		if(v[0][0]!='%' && ('0'>v[0][0] || v[0][0]>'9')){// If program hits a variable first load it than use it in calculations
			if(var_set.count(v[0])==0 and v[0][0]!='%')
				throw 1;
			out<<"%"<<var<<" = load i32* %"<<v[0]<<endl;
			v[0]="%"+to_string(var);
			var++;
		}
		else if(v[0][0]!='%'){
			string::size_type p;
			stoi(v[0],&p,10);// just don't look first char only but all characters
			if(p!=v[0].size())
				throw 5;
		}
		if(v[2][0]!='%' && ('0'>v[2][0] || v[2][0]>'9')){// If program hits a variable first load it than use it in calculations
			if(var_set.count(v[2])==0 and v[2][0]!='%')
				throw 1;
			out<<"%"<<var<<" = load i32* %"<<v[2]<<endl;
			v[2]="%"+to_string(var);
			var++;
		}
		else if(v[2][0]!='%'){
			string::size_type p;
			stoi(v[2],&p,10);// just don't look first char only but all characters
			if(p!=v[2].size())
				throw 5;
		}
		out<<"%"<<var<<" = "<<m[v[1][0]]<<" i32 "<<v[0]<<", "<<v[2]<<endl;
		v.erase(v.begin(),v.begin()+2);
		v[0]="%"+to_string(var);
		var++;
	}

	return v[0];
}
/* Prints calculation steps of an expression 'str' on the ll file */
string cal_exp(string str){
	str=remove_spaces(str);
	if(str.size()==0)
		throw 100;
	stack<int> pp;// stack used to understand which open paranthesis matches close paranthesis
	for(long int i=0;i<str.size();i++)
	{
		if(str[i]=='('){
			if(i-1>=0 and str[i-1]!='+' and str[i-1]!='-' and str[i-1]!='*' and str[i-1]!='/' and str[i-1]!='(')
				throw 3;
			pp.push(i);
		}
		else if(str[i]==')'){
			if(pp.size()==0)
				throw 0;
			if(i+1<str.size() and str[i+1]!='+' and str[i+1]!='-' and str[i+1]!='*' and str[i+1]!='/' and str[i+1]!=')')
				throw 3;
			int j = pp.top();
			pp.pop();
			str=str.substr(0,j)+cal_exp(str.substr(j+1,i-j-1))+str.substr(i+1,str.size());
			i=j-1;
		}
	}// if number of open paranthesis and close paranthesis is not equal then there is an error
	if(pp.size()>0)
		throw 0;
	vector<string> v;// parse string into terms
	for(long int i=0;i<str.size();i++){
		if(str[i]=='+' || str[i]=='-'){
			v.push_back(str.substr(0,i));
			v.push_back(string(1,str[i]));
			str=str.substr(i+1,str.size());
			i=-1;
		}
	}
	v.push_back(str);
	vector<string>::iterator it = v.begin();
	if((*it)=="")
		it=v.erase(it);
	while( it < v.end() ){// If there is a empty string, it means that at least two operator is adjacent
		if( (*it)=="" )
			throw 2;
		else
			it++;
	}

	// calculate terms
	for(long int i=0;i<v.size();i++)
		if(v[i][0]!='-' && v[i][0]!='+')
			v[i] = cal_exp_wout_par(v[i]);
	//handling unary minus
	if(v[0]=="-"){
		if(v[1][0]=='%')
		{
			throw 10;
		}
		else if(v[1][0]>='0' and v[1][0]<='9')
		{
			v.erase(v.begin());
			v[0]="-"+v[0];
		}
		else{
			throw 10;
		}
	}
	if(v[0]=="+")
		throw 2;
	if(v.size()==1){
		if(v[0][0]>='0' and v[0][0]<='9'){// check wheter it is an number
			string::size_type p;
			stoi(v[0],&p,10);// just don't look first char only but all characters
			if(p!=v[0].size())
				throw 5;
			return v[0];
		}
		else if(v[0][0]=='%' and v[0][1]>='0' and v[0][1]<='9'){// check wheter it is a temporary variables
			return v[0];
		}
		else if(v[0][0]=='-' and v[0][1]>='0' and v[0][1]<='9'){
			string::size_type p;
			stoi(v[0],&p,10);// just don't look first char only but all characters
			if(p!=v[0].size())
				throw 5;
			return v[0];
		}
		else{// If program hits a variable first load it than use it in calculations
			if(var_set.count(v[0])==0 and v[0][0]!='%')
				throw 1;
			out<<"%"<<var<<" = load i32* %"<<v[0]<<endl;
			v[0]="%"+to_string(var);
			var++;
			return v[0];
		}
	}
	while(v.size()>2){

			if(v[0][0]!='%' && ('0'>v[0][0] || v[0][0]>'9') && v[0][0]!='-'){// check wheter it is a variable
				if(var_set.count(v[0])==0 and v[0][0]!='%')
					throw 1;
				out<<"%"<<var<<" = load i32* %"<<v[0]<<endl;
				v[0]="%"+to_string(var);
				var++;
			}
			else if(v[0][0]!='%'){
				string::size_type p;
				stoi(v[0],&p,10);// just don't look first char only but all characters
				if(p!=v[0].size())
					throw 5;
			}
			if(v[2][0]!='%' && ('0'>v[2][0] || v[2][0]>'9') && v[0][0]!='-'){// check wheter it is a variable
				if(var_set.count(v[2])==0 and v[2][0]!='%')
					throw 1;
				out<<"%"<<var<<" = load i32* %"<<v[2]<<endl;
				v[2]="%"+to_string(var);
				var++;
			}
			else if(v[2][0]!='%'){
				string::size_type p;
				stoi(v[2],&p,10);// just don't look first char only but all characters
				if(p!=v[2].size())
					throw 5;
			}
			out<<"%"<<var<<" = "<<m[v[1][0]]<<" i32 "<<v[0]<<", "<<v[2]<<endl;
			v.erase(v.begin(),v.begin()+2);
			v[0]="%"+to_string(var);
			var++;
		}
	if(v.size()==2)//there is an extra operator
		throw 2;

	return v[0];
}

int main(int argc, char const *argv[])
{
	m['+'] = "add";
	m['-'] = "sub";
	m['*'] = "mul";
	m['/'] = "sdiv";

	ifstream in(argv[1]);
	if(!in.is_open())
	{
		cout<<"Unable to open input file\n";
		return 0;
	}
	string arg(argv[1]);

	for(int i=arg.size()-1;i>=0;i--)
		if(arg[i]=='.'){
			arg=arg.substr(0,i)+".ll";
			break;
		}

	out.open(arg);
	// initializing part
	out << "; ModuleID = 'stm2ir'"<< endl;
	out << "declare i32 @printf(i8*, ...)"<< endl;
	out << "@print.str = constant [4 x i8] c\"%d\\0A\\00\""<< endl;
	out << "define i32 @main() {"<< endl;

	string s;
	string varp;
	bool c;
	int line=0;
	/*
		Read a line from stm file. If there is not an assigment then print the result of expression, else calculate right part of assigment operator and assign it to variable which is on the left part
	*/
	while(getline(in,s)){//get nonempty line every time
		s=remove_others(s);
		if(s.size()==0)
			continue;
		line++;
		c=false;
		try{
			for(int i=0;i<s.size();i++)
				if(s[i]=='='){
					varp=remove_spaces(s.substr(0,i));
					check_var_syntax(varp);
					s = s.substr(i+1,s.size());
					s = cal_exp(s);
					if(var_set.count(varp)==0){
						var_set.insert(varp);
						out<<"%"<<varp<<" = alloca i32"<<endl;
					}
					out<<"store i32 "<<s<<", i32* %"<<varp<<endl;
					c = true;
					break;
				}
			if(c)//checks if it is an assigment or printing operation
				continue;
			s=cal_exp(s);
			out<<"call i32 (i8*, ...)* @printf(i8* getelementptr ([4 x i8]* @print.str, i32 0, i32 0), i32 "<<s<<")"<<endl;
			var++;
		}
		/* When an error occurs prints error message*/
		catch(int e){
			if(e==0){
				cout<<"Error: Line "<<line<<":  missing paranthesis"<<endl;
			}
			else if(e==1){
				cout<<"Error: Line "<<line<<":  undefined variable"<<endl;
			}
			else if(e==2){
				cout<<"Error: Line "<<line<<":  extra operator"<<endl;
			}
			else if(e==3){
				cout<<"Error: Line "<<line<<":  operator expected"<<endl;
			}
			else if(e==6)
				cout<<"Error: Line "<<line<<": you can't do more than one assigment per line"<<endl;
			else if(e==7)
				cout<<"Error: Line "<<line<<": invalid syntax for variable on the LHS"<<endl;
			else if(e==10)
				cout<<"Error: Line "<<line<<": unary minus is only allowed for numbers"<<endl;
			else
				cout<<"Error: Line "<<line<<": unknown"<<endl;
			remove(arg.c_str());
			return 0;
		}

	}

	out<<"ret i32 0"<<endl;
	out<<"}"<<endl;
	return 0;
}
