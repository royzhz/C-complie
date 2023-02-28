#pragma once
#include <vector>
#include "actiongoto.h"
#include "lexical_analyzer.h"
using namespace std;
struct progamma_tree {
    string words;
    vector<progamma_tree*> son;
};

class four_tuple {//四元式
public:
    string op;
    string arg1;
    string arg2;
    string result;
    friend ostream& operator<<(ostream& out, four_tuple& tmp);
};

class language//文法类
{
public:
    void add(const string& start, const vector<string>& end,int do_list);//start->end
    pair<int,pair<string, vector<string>>> get(int number);
private:
    vector<pair<string, vector<string>>> sentences;
    vector<int> do_function_list;
};

class LR_analyse {
public:
    LR_analyse();
    progamma_tree* analyse_sentence(vector<parsetoken> sentence);//主函数，返回progamma_tree?
    vector<string> out_state_stack;
    vector<string> out_symbol_stack;
    vector<string> left_symbol;
    vector<string> out_commond;
private:
    ACTIONGO_table* ACTION;
    ACTIONGO_table* GO;
    language lang;
    int nextquad = 0;
    vector<four_tuple> Intermediate_code;
    variable_table_class variable_table;

    void load_languange();
    void load_action();
    void load_goto();
    void emit(const string& op, const string& arg1, const string& arg2, const string& result);
    int do_function(int oper,vector<parsetoken>& string_stack,  const pair<string, vector<string>>& reduce_sentence);
    void backpatch(const string & merge_father, const string & content);
    string merge(const string & merge_father, const string & merge_son);

};

string show_sentence(const vector<parsetoken>&sentence,int location=0);
string show_sentence(const vector<string>&sentence,int location=0);
vector<parsetoken> init_input(const vector<string>& origin,const vector<string>& syntax_input_buf);