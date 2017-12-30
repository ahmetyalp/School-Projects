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
    vector<int> blocks;
    int itt;
    int last_executed_line;
    bool status;
    Process(string _file){
        this->file = _file;
        this->last_executed_line = 0;
        ifstream in;
        string s;
        itt = 0;
        status = true;
        in.open(this->file);
        while ( getline(in, s) ) { // Read line by line
            if( s.substr(0,4) == "exit" ) // Exit always 10ms
                inst.push_back(10);
            else if( s.substr(0,5) == "dispM" ){
                for( int i = s.size()-1 ; i > -1 ; i-- ) // Get instruction time by take after space character
                    if(isspace(s[i])){
                        inst.push_back(-3*stoi(s.substr(i+1,s.size())) + stoi(s.substr(6,1)));
                        break;
                    }
            }
            else if( s.substr(0,5) == "readM" ){
                for( int i = s.size()-1 ; i > -1 ; i-- ) // Get instruction time by take after space character
                    if(isspace(s[i])){
                        inst.push_back(-3*stoi(s.substr(i+1,s.size())) + 2);
                        int b;
                        for(b = 7 ; b < s.size() ; b++)
                            if(isspace(s[b]))
                                break;
                        blocks.push_back(stoi(s.substr(6,b-6)));
                        break;
                    }
            }
            else{
                for( int i = s.size()-1 ; i > -1 ; i-- ) // Get instruction time by take after space character
                    if(isspace(s[i])){
                        inst.push_back(stoi(s.substr(i+1,s.size())));
                        break;
                    }
                
            }
        }
        for(auto i : inst)
             cerr << i << " ";
        cerr << endl;
        for(auto i : blocks)
            cerr << i << " ";
        cerr << endl;
        in.close();
    }
};


vector<int> FIFO; //FIFO queue
vector<Process*> pr; // Pointer to map id to PCB
queue<pair<int,int> > arr_time[4]; // queue that keeps arrivel times in increasing order
vector<pair<int, int > > io[3];
int io_time[3];
queue<int> blk;
pair<int,int> cache;
int lru = 0;
const int quant = 100;
int current; // id of current running process
int _time; // time since algorithm running

void print_log(vector<int> * q, int t, string os){
    freopen(os.c_str(), "a", stdout);
    cout << t << "::HEAD";
    for(auto i : *q )
        cout << "-P" << i+1;
    cout << "-TAIL" << endl; // Output
    
}

void round_robin(){
    io_time[0] = 0;
    io_time[1] = 0;
    io_time[2] = 0;
    int exe; // execution time of current process
    int p;
    ofstream o10("myoutput_10");
    ofstream o11("myoutput_11");
    ofstream o12("myoutput_12");
    while (( !arr_time[3].empty() ) && (arr_time[3].front().first <= _time) ) { // First push new arrived processes to FIFO
        FIFO.push_back(arr_time[3].front().second);
        arr_time[3].pop();
    }
    do{
        
        cerr << "time is " << _time << endl;
        if( (current > -1) && ( pr[current]->status ) &&( pr[current]->last_executed_line < pr[current]->inst.size())){ // If current process hasn't finished push back FIFO
            FIFO.push_back(current);
        }

        if(current > -1)
            FIFO.erase(FIFO.begin());

        print_log(&FIFO, _time, "myoutput");

        exe = 0;
        
        if ( !FIFO.empty() )
            current = *(FIFO.begin());
        
        while ( (exe < quant) && ( pr[current]->last_executed_line < pr[current]->inst.size() ) )   { // Execute process until time slot expired or process finished
            
            
            p = pr[current]->inst[pr[current]->last_executed_line];
            cerr << current << " " << pr[current]->last_executed_line << " " << p << endl;
            pr[current]->last_executed_line++;
            if( p > 0 ){
                exe += p;
                
                if(!io[0].empty() && (*io[0].begin()).second <= _time + exe ){
                    FIFO.push_back((*io[0].begin()).first);
                    pr[(*io[0].begin()).first]->status = true;
                    if( (*io[0].begin()).second < _time + exe ) {
                        for(int i = 1 ; i < io[0].size() ; i++)
                            io[0][i].second += _time + exe - io[0][0].second;
                        io_time[0] += _time + exe -io[0][0].second;
                    }
                    io[0].erase(io[0].begin());
                    o10 << _time + exe << "::HEAD";
                    for(auto i : io[0] )
                        o10 << "-P" << i.first+1;
                    o10 << "-TAIL" << endl;
                }
                if(!io[1].empty() && (*io[1].begin()).second <= _time + exe ){
                    FIFO.push_back((*io[1].begin()).first);
                    pr[(*io[1].begin()).first]->status = true;
                    if( (*io[1].begin()).second < _time + exe ) {
                        for(int i = 1 ; i < io[1].size() ; i++)
                            io[1][i].second += _time + exe - io[1][0].second;
                        io_time[1] += _time + exe -io[1][0].second;
                    }
                    io[1].erase(io[1].begin());
                    o11 << _time + exe << "::HEAD";
                    for(auto i : io[1] )
                        o11 << "-P" << i.first+1;
                    o11 << "-TAIL" << endl;
                }
                if(!io[2].empty() && (*io[2].begin()).second <= _time + exe ){
                    FIFO.push_back((*io[2].begin()).first);
                    pr[(*io[2].begin()).first]->status = true;
                    if( lru == 0){
                        lru = 1;
                        cache.first = blk.front();
                        blk.pop();
                    }
                    else{
                        lru = 0;
                        cache.second = blk.front();
                        blk.pop();
                    }
                    if( (*io[2].begin()).second < _time + exe ) {
                        for(int i = 1 ; i < io[2].size() ; i++)
                            io[2][i].second += _time + exe - io[2][0].second;
                        io_time[2] += _time + exe -io[2][0].second;
                    }
                    io[2].erase(io[2].begin());
                    o12 << _time + exe << "::HEAD";
                    for(auto i : io[2] )
                        o12 << "-P" << i.first+1;
                    o12 << "-TAIL" << endl;

                }
                
                while (( !arr_time[3].empty() ) && (arr_time[3].front().first <= _time + exe) ) { // First push new arrived processes to FIFO
                    FIFO.push_back(arr_time[3].front().second);
                    arr_time[3].pop();
                }
            }
            else if( (-p)%3 == 1 ){
                cerr << "reading" << endl;
                cerr << "cache is " << cache.first << " " << cache.second << " black is " << pr[current]->blocks[pr[current]->itt] << endl;
                if( cache.first != pr[current]->blocks[pr[current]->itt] && cache.second != pr[current]->blocks[pr[current]->itt] ){
                    pr[current]->status = false;
                    if(io_time[2] > _time+exe)
                        io_time[2] += -(p-2)/3;
                    else
                        io_time[2] = _time+exe-(p-2)/3;
                    io[2].push_back(make_pair(current,io_time[2]));
                    blk.push(pr[current]->blocks[pr[current]->itt]);
                    o12 << _time + exe << "::HEAD";
                    for(auto i : io[2] )
                        o12 << "-P" << i.first+1;
                    o12 << "-TAIL" << endl;
                        
                    pr[current]->itt++;
                    break;
                }
                if( cache.first == pr[current]->blocks[pr[current]->itt])
                    lru = 1;
                else
                    lru = 0;
                pr[current]->itt++;
            }
            else{
                cerr << " printing " ;
                int pp = p%3;
                if(pp<0)
                    pp+=3;
                cerr <<pp << " " << io_time[pp] <<" " << _time + exe << " ";
                pr[current]->status = false;
                if(io_time[pp] > _time+exe)
                    io_time[pp] += -(p-pp)/3;
                else
                    io_time[pp] = _time+exe-(p-pp)/3;
                io[pp].push_back(make_pair(current, io_time[pp] ));
                ofstream oo;
                if(pp == 0){
                    o10 << _time + exe << "::HEAD";
                    for(auto i : io[0] )
                        o10 << "-P" << i.first+1;
                    o10 << "-TAIL" << endl;
                }
                else{
                    o11 << _time + exe << "::HEAD";
                    for(auto i : io[1] )
                        o11 << "-P" << i.first+1;
                    o11 << "-TAIL" << endl;

                }
                break;
            }
            
                    }
        _time += exe;
    }
    while ( (!FIFO.empty()) || (!arr_time[0].empty()) || (!io[0].empty()) || (!io[1].empty()) || (!io[2].empty()) );
}

int main(int argc, const char * argv[]) {
    remove("myoutput");
    _time = 0;
    current = -1; // Current running process id(-1 means IDLE)
    cache = make_pair(-1, -1);
    ifstream in;
    in.open(argv[1]); // Reads definition file (**IMPORTANT**Before read in unix-like system please convert input files line-endings,like dos2unix)
    string s;
    while (getline(in, s)) { // Read line by line
        for( int i = 0; i < s.size(); i++)
            if( isspace(s[i])  ){ // Parse every line into three token. First one unimportant(I assumed that process ids is in increasing order), second one code file, third one arrivel time
                for( int j = i+1; j < s.size(); j++)
                    if( isspace( s[j] ) ){ // Please use one space char between tokens in definition file
                        pr.push_back(new Process(s.substr(i+1,j-1-i)));
                        arr_time[3].push(make_pair(stoi(s.substr(j+1,s.size())), pr.size()-1));
                        break;
                    }
                break;
            }
        
    }
    
    in.close();
    round_robin();
    return 0;
    
}



