//
// Created by roy on 09/03/2023.
//

#ifndef PROJECT1_GENERATER_H
#define PROJECT1_GENERATER_H
#include <vector>
#include <map>
#include <string>
#include <stack>
#include <LR_analyse.h>
#include <unordered_map>
#define EMPTY 0
#define ACTIVE 1

#define MEM 0
#define REG 1

#define REGSU 1//申请到寄存器，不需要移动其它变量
#define SWITCH 2//申请到寄存器，需要移动其它变量到内存
#define RECALL 3//从内存调回变量
#define SWAP 4//从内存交换变量

struct active_information{
    int line;
    int active;
};

using namespace std;

struct register_location{//寄存器状态与操作
    int status;
    string reg_location;//寄存器中的位置
    string var_to_menory;//需要移动到内存的变量
    string var_to_reg;//需要从内存调回的变量
};


class register_manager {
public:
    register_manager(vector<four_tuple>* four_tuple,map<string,vector<string>*>* function);
    register_location get_register(const string& varible_name,bool write=1);//获取寄存器
    void generate_active_table();//建立活跃表
    void call_function(string& function);//call函数
    void return_function();//return函数
    void pop_sentence(four_tuple&);//弹出变量
    int get_variable_num(const string&);//获取变量
    register_location back_to_memory(const string&);//回收寄存器
    bool attach_memory_varaible(const string&memory,const string& reg);//直接将内存赋给寄存器
    string get_memory(const string&);//只需要给新变量获取内存
private:
    unordered_map<string,string> memory_register;//内存->寄存器 变量表
    unordered_map<string,string> register_memory;//寄存器->内存 表

    unordered_map<string,string> variable_memory;//变量->内存 位置表
    unordered_map<string,string> memory_variable;//内存->变量 位置表


    map<string,map<string,stack<active_information>>*> active_table;//活跃信息表
    map<string,int> function_variable_number;//函数变量个数

    map<string,stack<active_information>> *now_active_information;//当前函数的活跃信息指针


    //第一个string确定函数，第二个string确定变量，vector<pair<int,int>>确定活跃区间
    static int add_active_information(string&,map<string,stack<active_information>>*,int,int active= ACTIVE,bool ret=false);


    vector<four_tuple>* four_tuple_list;
    map<string,vector<string>*>* function_table;

    int build_single_function_table(int begin,int end);//对单个函数建立活跃表
    pair<int,string> find_LRU_register();//寻找寄存器
    void count_temp_variable(const string& function_name,int begin,int end);//计算临时变量个数

    string now_function;//当前函数
    int now_top;//当前栈顶
};

class assble_code{
public:
    string header;
    string oper;
    string arg1;
    string arg2;
    string arg3;
    friend void operator<<(ostream& out,const assble_code& assble_code);
};

class generator {
public:
    generator(vector<four_tuple> *four_tuple, map<string, vector<string> *> *function, register_manager &reg);

    void generate_assble_code();//生成汇编代码

    void show();
private:
    bool generate_single_assble_code(const four_tuple& four_code,int line,const string& head = ""); //生成单个函数的汇编代码
    vector<four_tuple>* four_tuple_list;
    map<string,vector<string>*>* function_table;

    map<string,vector<assble_code>> assble_code_list;//汇编代码

    vector<assble_code> now_assble_code;//当前函数的汇编代码

    register_manager& reg_man;
    void handle_function(const register_location& re);//处理内存异常函数

    int jump_number;//跳转标号
    unordered_map<int,string> jump_table;//跳转表
    void return_action(string block_name,const string& return_va="");
    string now_function;
};

#endif //PROJECT1_GENERATER_H
