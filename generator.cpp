//
// Created by roy on 09/03/2023.
//
#include "generator.h"
#include <stack>
#include <unordered_set>
#include <utility>

#define RA "$ra"//返回地址寄存器
#define SP "$sp"//栈指针
#define FP "$fp"//帧指针
#define ZERO "$zero"//零寄存器
#define AT "$at"//保留寄存器
#define REV "$v0"//函数返回值寄存器

//const string return_list[2]={"v0","v1"};
//const string callre_list[4]={"a0","a1","a2","a3"};
//const string register_list[10]={"$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7", "$t8", "$t9"};
const string recover_list[9]={"$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7", "$s8"};
const string available_list[25]={"$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7", "$t8", "$t9","v0","v1","a0","a1","a2","a3","$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7", "$s8"};
const string save_list[16]={"$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7", "$t8", "$t9","v0","v1","a0","a1","a2","a3"};

generator::generator(vector<four_tuple> *four_tuple, map<string, vector<string> *> *function, register_manager &reg):reg_man(reg)
{
    four_tuple_list=four_tuple;
    function_table=function;
    jump_number=0;

}


void generator::return_action(string block_name,const string& return_va){//返回操作,前面是标号,后面是返回寄存器
    if(!return_va.empty()){
        now_assble_code.push_back({block_name,"add",REV,ZERO,return_va});
        block_name="";
    }
    now_assble_code.push_back({block_name, "addi", "$sp", "$sp", to_string(reg_man.get_variable_num(now_function) * 4)});
    if(now_function!="main")
        now_assble_code.push_back({"","jr",RA});

    else
        now_assble_code.push_back({"","halt"});



    reg_man.return_function();
}
bool generator::generate_single_assble_code(const four_tuple& four_code,int line,const string& head){
    if(!head.empty()){//函数头，处理函数
        now_assble_code.push_back({head,"subi","$sp","$sp",to_string(reg_man.get_variable_num(head)*4)});
        //调回所有老变量
//        for(auto &i:*((*function_table)[head])){
//            handle_function(reg_man.get_register(i));
//        }
    }
    string block_name= to_string(line);
    if(four_code.op=="="){
        auto reg=reg_man.get_register(four_code.result,false);
        handle_function(reg);
        if(four_code.arg1[0]!='t'){//说明是常量
            now_assble_code.push_back({block_name,"addi",reg.reg_location,ZERO,four_code.arg1});
        }
        else{
            auto reg2=reg_man.get_register(four_code.arg1);
            handle_function(reg2);
            now_assble_code.push_back({block_name,"add",reg.reg_location,ZERO,reg2.reg_location});
        }
    }
    else if(four_code.op[0]=='j'){
        //先对最后一个定义
        assble_code code;
        int last=stoi(four_code.result);
        string jump_place=jump_table[last];
        if(!jump_place.empty()){
            jump_place=jump_table[last];
        }
        else{
            jump_place="jump"+to_string(jump_number);
            jump_number++;
            jump_table[last]=jump_place;
        }

        if(four_code.op=="j")
            code={block_name,"j",jump_place};
        else {
            auto reg1=reg_man.get_register(four_code.arg1);
            handle_function(reg1);
            auto reg2=reg_man.get_register(four_code.arg1);
            handle_function(reg2);
            if (four_code.op == "j==")
                code = {block_name,"beq", reg1.reg_location,reg2.reg_location,jump_place};
            else if (four_code.op == "j!=")
                code = {block_name,"bne",reg1.reg_location,reg2.reg_location,jump_place};
            else if(four_code.op=="jnz")
                code ={block_name,"bne",reg1.reg_location,ZERO,jump_place};
            else {//jop
                now_assble_code.push_back({block_name,"sub",reg1.reg_location,reg2.reg_location,AT});
                if(four_code.op=="j<=")
                    code = {"","blez ",AT,jump_place};
                else if(four_code.op=="j<")
                    code = {"","bltz ",AT,jump_place};
                else if(four_code.op=="j>")
                    code = {"","bgtz ",AT,jump_place};
                else if(four_code.op=="j>=")
                    code = {"","bgez ",AT,jump_place};
            }

        }
        now_assble_code.push_back(code);
    }
    else if(four_code.op=="return"){
        return_action(block_name,four_code.result);
        return true;
    }
    else if(four_code.op=="call"){

        int function_parameter_number=(*function_table)[four_code.arg1]->size();
        now_assble_code.push_back({block_name,"subi","$sp","$sp", to_string(36+function_parameter_number)});
        int offset=0;
        //将参数压栈
        auto parameter=(vector<string>*) stoll(four_code.result);

        if(parameter->size()!=function_parameter_number){
            cout<<"error"<<endl;
        }

        for(auto & i: *(parameter)){
            auto reg_loc=reg_man.get_register(i);
            handle_function(reg_loc);
            now_assble_code.push_back({"","sw",reg_loc.reg_location,to_string(offset)+"($sp)"});
            offset+=4;
        }
        //对保存寄存器进行压栈操作
        for(const auto& i:recover_list){
            now_assble_code.push_back({"","sw",i,to_string(offset)+"($sp)"});
            offset+=4;
        }

        //对不保存的寄存器进行返回内存操作
        for(const auto& i:save_list){
            auto reg_info=reg_man.back_to_memory(i);
            if(reg_info.status==RECALL){
                now_assble_code.push_back({"","sw",reg_info.reg_location,reg_info.var_to_menory});
                block_name="";
            }
        }

        //进行跳转
        now_assble_code.push_back({"","jal",four_code.arg1});

        //将对保存寄存器进行返回
        offset-=4;
        for(int i=8;i>=0;i--){
            now_assble_code.push_back({"","lw",recover_list[i],to_string(offset)+"($sp)"});
            offset-=4;
        }
        //退出栈帧
        now_assble_code.push_back({"","addi","$sp","$sp", to_string(36+function_parameter_number)});
        //定义返回值处理
        auto mem=reg_man.get_memory(four_code.arg2);
        reg_man.attach_memory_varaible(mem,REV);
    }
    else{
        assble_code code;
        string oper;
        if(four_code.op=="+"){
            oper="add";
        }
        else if(four_code.op=="-"){
            oper="sub";
        }
        else if(four_code.op=="*"){
            oper="mul";
        }

        auto reg1=reg_man.get_register(four_code.arg1);
        handle_function(reg1);
        auto reg2=reg_man.get_register(four_code.arg2);
        handle_function(reg2);

        auto reg3=reg_man.get_register(four_code.result, false);
        handle_function(reg3);

        code={block_name,oper,reg3.reg_location,reg1.reg_location,reg2.reg_location};
        now_assble_code.push_back(code);
    }

    return false;
}



void generator::show(){
    if(assble_code_list.find("main")==assble_code_list.end()){
        cout<<"cant find main function"<<endl;
        return;
    }
    cout<<".data"<<endl;

    cout<<".text"<<endl;
    for(auto &it2:assble_code_list["main"]){
        string head;
        if(!it2.header.empty()) {
            if (it2.header[0] >= '0' && it2.header[0] <= '9')
                head = jump_table[stoi(it2.header)];
            else
                head = it2.header;
        }
        if(!head.empty())
            cout<<setw(7)<<std::left<<head<<" : ";
        else
            cout<<setw(10)<<" ";
        cout<< it2;
    }
    cout<<endl;


    for(auto &it:assble_code_list){
        if(it.first=="main")
            continue;
        for(auto &it2:it.second){
            string head;
            if(!it2.header.empty()) {
                if (it2.header[0] >= '0' && it2.header[0] <= '9')
                    head = jump_table[stoi(it2.header)];
                else
                    head = it2.header;
            }
            if(!head.empty())
                cout<<setw(7)<<std::left<<head<<" : ";
            else
                cout<<setw(10)<<" ";
            cout<< it2;
        }
        cout<<endl;
    }
}

void generator::generate_assble_code(){

    string old_function;
    bool has_return=false;
    for(int i=0;i<four_tuple_list->size();i++){
        if((*four_tuple_list)[i].function){//说明是函数的开头
            if(!now_assble_code.empty()) {//处理之前的代码
                if(!has_return) {//没返回值的函数默认加一个返回
                    reg_man.return_function();
                    return_action("");
                }

                assble_code_list[now_function] = now_assble_code;
                now_assble_code.clear();

            }
            //调整栈帧位置

            now_function=(*four_tuple_list)[i].line_number;
            old_function=now_function;
            has_return=false;
            reg_man.call_function(now_function);
        }
        has_return= has_return | generate_single_assble_code((*four_tuple_list)[i],i,old_function);
        reg_man.pop_sentence((*four_tuple_list)[i]);
        old_function="";
    }

    if(!has_return) {
        reg_man.return_function();
        return_action("");
    }
    assble_code_list[now_function] = now_assble_code;
    now_assble_code.clear();
}

void generator::handle_function(const register_location& re){
    assble_code code;
    switch (re.status) {
        case REGSU:
            break;
        case SWITCH:
            code={"","lw",re.reg_location,re.var_to_menory};
            now_assble_code.push_back(code);
            break;
        case RECALL:
            code={"","sw",re.reg_location,re.var_to_menory};
            now_assble_code.push_back(code);
            break;
        case SWAP:
            code={"","sw",re.reg_location,re.var_to_menory};
            now_assble_code.push_back(code);
            code={"","lw",re.reg_location,re.var_to_reg};
            now_assble_code.push_back(code);
            break;
    }
}

void operator<<(ostream& out,const assble_code& assble_code){

    out<<setw(10)<<std::left<<assble_code.oper<<assble_code.arg1;
    out<<std::right;
    if(!assble_code.arg2.empty())
        out<<","<<setw(10)<<assble_code.arg2;

    if(!assble_code.arg3.empty())
        out<<","<<setw(10)<<assble_code.arg3;
    out<<endl;
}




//返回LRU寄存器名字
pair<int,string> register_manager::find_LRU_register(){
    int longest=0;
    string ans;
    for(const auto& i:available_list){
        auto memory_location=register_memory[i];//寄存器对应的内存位置
        if(memory_location.empty()){//空寄存器
            return {REG,i};//直接覆盖
        }
        auto varible_name=memory_variable[memory_location];//变量名

        auto active_info=(*now_active_information)[varible_name].top();
        if(register_memory[i].empty()|| active_info.active==EMPTY){//空寄存器
            return {REG,i};//直接覆盖
        }
        if(active_info.line>longest){
            longest=active_info.line;
            ans=i;
        }
    }
    return {MEM,ans};//表示需要使用，先写入内存
}


register_location register_manager:: get_register(const string& varible_name,bool read){
    auto varible_memory_location=variable_memory[varible_name];

    if(!varible_memory_location.empty()){//找到了在内存中的位置
        auto reg_location=memory_register[varible_memory_location];//寄存器位置
        if(!reg_location.empty()){//直接在寄存器
            return {REGSU,reg_location};
        }
        else//找到但不在寄存器，在内存
        {
            auto LRU_re = find_LRU_register();//LRU状态
            if (LRU_re.first == REG) {//找到了空闲的寄存器

                //清理内存映射
                auto tmp = register_memory[LRU_re.second];//之前的寄存器映射的MEM
                if (!tmp.empty())
                    memory_register.erase(tmp);//删除之前MEM->寄存器的映射

                memory_register[varible_memory_location] = LRU_re.second;//把内存的内容读入寄存器
                register_memory[LRU_re.second] = varible_memory_location;//寄存器映射更新

                if(read) {
                    //第二个参数是寄存器准备写入，第三个参数是内存准备读出
                    return {SWITCH, LRU_re.second, varible_memory_location};
                }
                else {
                    return {REGSU, LRU_re.second};
                }

            }
            else {//没找到空闲寄存器，需要将LRU寄存器写回，再从内存调入
                //删除之前MEM->寄存器的映射
                auto tmp=register_memory[LRU_re.second];//之前的寄存器映射的MEM
                if(!tmp.empty())
                    memory_register.erase(tmp);
                else
                    cout<<"error"<<endl;

                //内存读入寄存器

                memory_register[varible_memory_location] = LRU_re.second;
                register_memory[LRU_re.second] = varible_memory_location;//寄存器映射更新
                //第二个参数是需要调回的寄存器,第三个参数是调回的内存，第四个是调入的内存
                if(read) {
                    return {SWAP, LRU_re.second, tmp, varible_memory_location};
                }
                else//存入即可
                    return {RECALL, LRU_re.second, tmp};
            }

        }
    }
    else{//没找到,新变量
        //首先分配内存
        auto varibleMemoryLocation= "$" + to_string(now_top) + "(sp)";
        now_top+=4;
        //建立内存映射
        variable_memory[varible_name]=varibleMemoryLocation;
        memory_variable[varibleMemoryLocation]=varible_name;
        //找寄存器
        auto LRU_re=find_LRU_register();
        if(LRU_re.first == REG){//有空闲
            auto tmp=register_memory[LRU_re.second];
            if(!tmp.empty())//可能是空寄存器，也可能是无用的变量
                memory_register.erase(tmp);

            memory_register[varibleMemoryLocation]=LRU_re.second;
            register_memory[LRU_re.second]=varibleMemoryLocation;

            return {REGSU,LRU_re.second};
        }
        else{//无空闲，需要读出到内存，但不用从内存读
            //打断M->R映射
            auto tmp=register_memory[LRU_re.second];
            if(!tmp.empty())
                memory_register.erase(tmp);
            else
                cout<<"error"<<endl;
            //建立M->R映射
            memory_register[varibleMemoryLocation]=LRU_re.second;
            register_memory[LRU_re.second]=varibleMemoryLocation;
            //第二个参数是需要调回的寄存器,第三个参数是调回的内存
            return {RECALL,LRU_re.second,tmp};
        }

    }
}

void register_manager::call_function(string& function){//处理新函数
    now_function=function;
    now_active_information=active_table[now_function];
    memory_register.clear();
    register_memory.clear();
    variable_memory.clear();
    memory_variable.clear();
    now_top=0;

    //和旧变量建立联系
    if(!function.empty()) {
        int offset = 0;
        auto iter= (*function_table)[now_function];
        if(iter==nullptr)
            return;
        for (auto &i: *iter) {
            string variable_location = "$" + to_string(function_variable_number[now_function]*4 + offset) + "(sp)";
            variable_memory[i] = variable_location;
            memory_variable[variable_location] = i;
            offset += 4;
        }
    }
}

void register_manager::return_function()//return时的函数
{


}

void register_manager::pop_sentence(four_tuple& fourT){
    string name=fourT.arg1;
    if(name[0]=='t')
        (*now_active_information)[name].pop();
    name=fourT.arg2;
    if(name[0]=='t')
        (*now_active_information)[name].pop();
    name=fourT.result;
    if(name[0]=='t')
        (*now_active_information)[name].pop();
}

int register_manager::get_variable_num(const string& function_name){
    return function_variable_number[function_name];
}

register_location register_manager::back_to_memory(const string&reg_name){
    auto varible_memory_location=variable_memory[reg_name];
    if(varible_memory_location.empty())//无内存映射
        return {REGSU};
    auto variable_name=memory_variable[varible_memory_location];
    bool active=(*now_active_information)[variable_name].top().active;
    if(active==ACTIVE) {
        memory_register.erase(varible_memory_location);
        register_memory.erase(reg_name);
        return {RECALL, reg_name, varible_memory_location};
    }
    return {REGSU};
}

bool register_manager::attach_memory_varaible(const string &memory, const string &reg) {
    memory_register[memory]=reg;
    register_memory[reg]=memory;
    return true;
}

string register_manager::get_memory(const string & variable_name) {
    string memory_name=variable_memory[variable_name];
    if(!memory_name.empty())
        return memory_name;

    memory_name="$"+to_string(now_top)+"(sp)";
    now_top+=4;
    variable_memory[variable_name]=memory_name;
    memory_variable[memory_name]=variable_name;
    return memory_name;
}


register_manager::register_manager(vector<four_tuple>* four_tuple,map<string,vector<string>*>* function){
    four_tuple_list=four_tuple;
    function_table=function;
    now_top=0;


}



int register_manager::add_active_information(string& variablename,
                                             map<string,stack<active_information>>* now_table,
                                             int line,
                                             int active,
                                             bool ret//return特判
)
{
    if(ret){
        stack<active_information> tmp;
        (*now_table)[variablename] = tmp;
        (*now_table)[variablename].push({EMPTY,ACTIVE});
        return 1;
    }
    int ans=0;
    if(now_table->find(variablename)==now_table->end()){//找不到，需要添加，默认全空
        stack<active_information> tmp;
        (*now_table)[variablename] = tmp;
        (*now_table)[variablename].push({EMPTY, EMPTY});
        ans=1;
    }
    (*now_table)[variablename].push({line, active});
    return ans;
}

int register_manager::build_single_function_table(int begin,int end){
    string funcname=(*four_tuple_list)[begin].line_number;
    auto now_table=new map<string,stack<active_information>>;//活跃信息表
    unordered_set<string> var_table;//变量表
    for(int i=end;i>=begin;i--){
        string & operater=(*four_tuple_list)[i].op;
        if(i==end &&  operater == "return"){
            add_active_information((*four_tuple_list)[i].result,now_table,EMPTY,EMPTY,true);
        }
        else if( operater=="call"){
            add_active_information((*four_tuple_list)[i].arg2,now_table,EMPTY,EMPTY);
            auto list=(vector<string> *)stoll((*four_tuple_list)[i].result);
            for(auto &it:*list){
                add_active_information(it,now_table,i);
            }
        }
        else if(operater[0]=='j'){
            if((*four_tuple_list)[i].arg1[0]=='t'){
                add_active_information((*four_tuple_list)[i].arg1,now_table,i);
            }
            if((*four_tuple_list)[i].arg2[0]=='t'){
                add_active_information((*four_tuple_list)[i].arg2,now_table,i);
            }
        }
        else{
            if((*four_tuple_list)[i].result[0]=='t'){
                add_active_information((*four_tuple_list)[i].result,now_table,EMPTY,EMPTY);
            }
            if((*four_tuple_list)[i].arg1[0]=='t'){
                add_active_information((*four_tuple_list)[i].arg1,now_table,i);
            }
            if((*four_tuple_list)[i].arg2[0]=='t'){
                add_active_information((*four_tuple_list)[i].arg2,now_table,i);
            }
        }
    }
    active_table[funcname]=now_table;

    count_temp_variable(funcname,begin,end);
    //function_variable_number[funcname]=variable_num;
    return 0;
}

void register_manager::generate_active_table(){
    int begin=0;
    for(int i=0;i<four_tuple_list->size();i++){
        if((*four_tuple_list)[i].function){
            build_single_function_table(begin,i-1);
            begin=i;
        }
    }
    build_single_function_table(begin,four_tuple_list->size()-1);

}

void register_manager::count_temp_variable(const string& function_name,int begin,int end) {
    if(begin>end)
        return;
    int number=0;
    unordered_set<string> s;
    unordered_set<string> us;
    auto iter=(*function_table)[function_name];
    if(iter!=nullptr){
        for(auto&i:*iter){
            us.insert(i);
        }
    }
    for(int i=end;i>=begin;i--){
        string tmp[3]={(*four_tuple_list)[i].arg1,(*four_tuple_list)[i].arg2,(*four_tuple_list)[i].result};
        for(auto & j : tmp){
            if(j[0]=='t' && s.find(j)==s.end() && us.find(j)==us.end()){
                s.insert(j);
                number++;
            }
        }
    }
    function_variable_number[function_name]=number;
}
