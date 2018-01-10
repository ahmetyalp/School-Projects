// 2015400213
// I compile with -std=c++11 
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <cstring>
#include <string>
#include <cstdlib>

using namespace std;

class Process{ //PCB for process
public:
    string file; // Filename of code of process
    vector<int> inst; // Load instructions into vector
    int last_executed_line;
    Process(string _file){
        this->file = _file;
        this->last_executed_line = 0;
        ifstream in;
        string s;
        in.open(this->file);
        while ( getline(in, s) ) { // Read line by line
            if( s.substr(0,4) == "exit" ) // Exit always 10ms
                inst.push_back(10);
            else{
                for( int i = s.size()-1 ; i > -1 ; i-- ) // Get instruction time by take after space character
                    if(isspace(s[i])){
                        inst.push_back(stoi(s.substr(i+1,s.size())));
                        break;
                    }
        
            }
        }
        
        in.close();
    }
};


vector<int> FIFO; //FIFO queue
vector<Process*> pr; // Pointer to map id to PCB
queue<pair<int,int> > arr_time; // queue that keeps arrivel times in increasing order
const int quant = 100;
int current; // id of current running process 
int _time; // time since algorithm running


void round_robin(){
    int exe; // execution time of current process
    do{
        while (( !arr_time.empty() ) && (arr_time.front().first <= _time) ) { // First push new arrived processes to FIFO 
            FIFO.push_back(arr_time.front().second);
            arr_time.pop();
        }
        if( (current > -1) && ( pr[current]->last_executed_line < pr[current]->inst.size())){ // If current process hasn't finished push back FIFO
            FIFO.push_back(current);
        }
        if(current > -1)
            FIFO.erase(FIFO.begin());
        cout << _time << "::HEAD";
        for(auto i : FIFO )
            cout << "-P" << i+1;
        cout << "-TAIL" << endl; // Output

        exe = 0;
        if ( !FIFO.empty() )
        	current = *(FIFO.begin());
        while ( (exe < quant) && ( pr[current]->last_executed_line < pr[current]->inst.size() ) )   { // Execute process until time slot expired or process finished
            exe += pr[current]->inst[pr[current]->last_executed_line];
            pr[current]->last_executed_line += 1;
        }
        _time += exe;
        
        
    }
    while ( (!FIFO.empty()) || (!arr_time.empty()) ); // Until no waiting and coming process
    
}

int main(int argc, const char * argv[]) {
    freopen("myoutput.txt", "w", stdout);
    _time = 0;
    current = -1; // Current running process id(-1 means IDLE)
    ifstream in;
    in.open(argv[1]); // Reads definition file (**IMPORTANT**Before read in unix-like system please convert input files line-endings,like dos2unix)
    string s;
    while (getline(in, s)) { // Read line by line
        for( int i = 0; i < s.size(); i++)
            if( isspace(s[i])  ){ // Parse every line into three token. First one unimportant(I assumed that process ids is in increasing order), second one code file, third one arrivel time
                for( int j = i+1; j < s.size(); j++)
                    if( isspace( s[j] ) ){ // Please use one space char between tokens in definition file
                        pr.push_back(new Process(s.substr(i+1,j-1-i)));
                        arr_time.push(make_pair(stoi(s.substr(j+1,s.size())), pr.size()-1));
                        break;
                    }
                break;
            }
        
    }
    
    in.close();
    round_robin();
    return 0;
    
}



