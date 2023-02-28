#pragma once
#include <vector>
#include <string>
#include <map>
using namespace std;

#define SHIFT 1
#define REDUCE 2
#define ACC 3
#define ERR 0

#define TERMINAL 1    //终结符
#define OPERATOR 2   //操作符
#define NONTERMINAL 3 //非终结符
#define ID 4      //变量
#define CONST_INT 5     //常量

struct state
{
    int swift = ERR;//下一个转化
    int nextstate = -1;//下一个状态
};



class ACTIONGO_table//action和goto表
{
public:
    ACTIONGO_table(int state_sum, const vector<string>& terminator);//action表
    const state get(int state_number, string terminat);//获得
    void change(int state_number, string terminat, const state& new_state);//加入
private:
    vector<vector<state>> content;
    vector<string> terminator_list;
    int all_state_sum;
    int get_index(const string& terminat);
};

class parsetoken
{
public:
    string symbol;
    int tag;
    parsetoken(const string& init);
    void set(const string& para_name, string content);
    string get(const string& para_name);
    friend ostream& operator<<(ostream& out, parsetoken& time);
    operator string() const;
    map<string, string> parameter;
private:
};

class variable_table_class {
public:
    variable_table_class();
    string lookup(const string& variable_name);//空就是错误
    string newvar(const string& variable_name); 
    string newtemp();
    void newlayer();
    void poplayer();
    void new_function();
private:
    int current_layer=0;
    vector<map<string, string>> table;
    vector<int> temp_count;
};