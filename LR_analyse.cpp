#include <iostream>
#include <iomanip>
#include<string>
#include<sstream>
#include <fstream>
#include <utility>
#include "LR_analyse.h"

#define statelength 20
#define stacklength 20
#define sentencelength 70

void language::add(const string& start, const vector<string>& end,int do_list) {
    this->sentences.push_back(make_pair(start, end));
    this->do_function_list.push_back(do_list);
}

pair<int,pair<string, vector<string>>> language::get(int number) {
    return make_pair(do_function_list[number],sentences[number]);
}


LR_analyse::LR_analyse(){
    load_languange();
    load_action();
    load_goto();
}

progamma_tree* LR_analyse::analyse_sentence(vector<parsetoken> sentence){
    sentence.push_back(string("$"));
    vector<int> state_stack;
    vector<parsetoken> string_stack;

    vector<progamma_tree*> stack_node;

    out_state_stack.clear();
    out_symbol_stack.clear();
    left_symbol.clear();
    out_commond.clear();

    int sentence_pointer=0;
    state_stack.push_back(0);
    string_stack.push_back(parsetoken("$"));

    auto show=[&](){
        string out_state;
        for (auto i : state_stack)
            out_state += to_string(i)+" ";
        out_state_stack.push_back(out_state);

        out_state.clear();
        for (auto i : string_stack)
            out_state +="<"+string(i)+">";
        out_symbol_stack.push_back(out_state);
        left_symbol.push_back(show_sentence(sentence, sentence_pointer));
    };
    int at=0;
    show();
    while(true) {
        int now_state = state_stack.back();
        parsetoken now_string=parsetoken("");
        if(at==0)
            now_string = sentence[sentence_pointer];
        else{
            now_string=parsetoken("@");
            at=0;
        }
        const state &new_state = ACTION->get(now_state, now_string);
        string why = (new_state.swift == SHIFT ? "s" : "r") + to_string(new_state.nextstate) + ',';

        if (new_state.swift == ACC) {
            out_commond.push_back("success");
            for (int show_int = 0; show_int < out_symbol_stack.size(); show_int++) {
                cout << out_state_stack[show_int] << out_symbol_stack[show_int] << left_symbol[show_int]
                     << out_commond[show_int] << endl;
            }

            for(auto i : Intermediate_code)
                cout<<i<<endl;
            return stack_node.back();
        }

        if (new_state.swift == REDUCE) {

            pair<int, pair<string, vector<string>>> reduce_sentence_pair = lang.get(new_state.nextstate);
            int oper = reduce_sentence_pair.first;
            auto reduce_sentence = reduce_sentence_pair.second;
            why += reduce_sentence.first + "->" + show_sentence(reduce_sentence.second) + ",";

            auto *father = new progamma_tree{reduce_sentence.first};

            for (int j = 0; j < reduce_sentence.second.size(); j++) {
                father->son.push_back(stack_node.back());
                state_stack.pop_back();
                stack_node.pop_back();
            }
            if(do_function(oper, string_stack, reduce_sentence)==ERR){
                cout<<"语义错误"<<endl;
                return nullptr;
            }//执行规约时的函数
            state temp_state = GO->get(state_stack.back(), reduce_sentence.first);

            if (temp_state.nextstate == -1) {
                cout << "error" << endl;
                return nullptr;
            }

            state_stack.push_back(temp_state.nextstate);
            stack_node.push_back(father);

            why += "GO[" + to_string(state_stack.back()) + ",<" + reduce_sentence.first + ">]=" +
                   to_string(temp_state.nextstate) + "入栈";
            show();
            out_commond.push_back(why);
        } else if (new_state.swift == SHIFT) {

            state_stack.push_back(new_state.nextstate);
            if(string(now_string)!="@")
                sentence_pointer++;

            string_stack.push_back(now_string);
            auto *temp = new progamma_tree{now_string};
            stack_node.push_back(temp);

            why += "状态" + to_string(new_state.nextstate) + ",终结符<" + string(now_string) + ">入栈";
            show();
            out_commond.push_back(why);
        } else {
            const state &null_state = ACTION->get(now_state, "@");
            if (null_state.swift == ERR) {

                cout << "语法错误" << endl;
                for (int show_int = 0; show_int < out_symbol_stack.size(); show_int++) {
                    cout << out_state_stack[show_int] << out_symbol_stack[show_int]
                         << endl;// left_symbol[show_int] << out_commond[show_int] << endl;
                }
                return nullptr;
            } else {
//                now_state = null_state.nextstate;
//                string_stack.push_back(parsetoken("@"));
//                state_stack.push_back(now_state);
//                auto *temp = new progamma_tree{"@"};
//                stack_node.push_back(temp);
                at=1;
            }
        }
    }
}

int LR_analyse::do_function(int oper, vector<parsetoken>& string_stack, const pair<string, vector<string>>& reduce_sentence) {
    vector<parsetoken> item;//记录弹出的符号给函数使用
    parsetoken ready_to_push = reduce_sentence.first;//准备加入的非终结符
    //从符号栈中弹出需要规约的所有符号，加入item中
    for (int j = 0; j < reduce_sentence.second.size(); j++) {
        item.push_back(string_stack.back());
        string_stack.pop_back();
    }
    if (oper == 1) { //继承 A -> B时，A继承B的全部属性
        auto E = item[0];
        ready_to_push.parameter = E.parameter;
    }
    else if (oper == 2) {//A -> ( B ) A继承B的全部属性
        auto E = item[1];
        ready_to_push.parameter = E.parameter;
    }
    else if (oper == 3) { //赋值语句 A -> B op C ,op=+/-/*// 双目运算符
        auto E0 = item[2];
        auto E1 = item[0];
        auto op = item[1];
        string operater;
        if (op.symbol == "relop")
            operater = op.get("oper");
        else
            operater = op.symbol;
        ready_to_push.set("place", variable_table.newtemp());//找个新写入地址，然后写入
        //类型检查（未实现）
        emit(operater, E0.get("place"), E1.get("place"), ready_to_push.get("place"));
    }
    else if (oper == 4) { //赋值 A-> id = E ;
        auto E = item[1];
        auto id = item[3];
        string p = variable_table.lookup(id.get("name"));
        if (p.empty()) {
            cout << "未知的变量" << id.get("name") << endl;
            return ERR;
        }
        emit("=", E.get("place"), "-", p);
    }
    else if (oper == 5) { //声明 A-> type id ;
        auto id = item[1];
        auto type = item[2];
        id.set("type", type.symbol);
        variable_table.newvar(id.get("name"));//变量表加入
    }

    else if (oper == 6) { //声明 A-> type id = E ;
        auto E = item[1];
        auto id = item[3];
        auto type = item[4];
        id.set("type", type.symbol);
        auto new_location = variable_table.newvar(id.get("name"));//变量表加入
        //类型检查
        //赋值直接去找place
        emit("=", E.get("place"), "-", new_location);
    }
    else if (oper == 7) { //赋值规约 E -> id;
        auto id = item[0];
        string p = variable_table.lookup(id.get("name"));
        if (p.empty()) {
            cout << "未知的变量" << id.get("name") << endl;
            return ERR;
        }
        ready_to_push.set("place", p);
    }
    else if (oper == 8) {//relop->></>等
        auto operater = item[0];
        ready_to_push.set("oper", operater.symbol);
    }

    else if (oper == 9) {//控制中的布尔表达式A-> E or M E
        auto E2 = item[0];
        auto M = item[1];
        auto E1 = item[3];
        backpatch(E1.get("falselist"), M.get("quad"));
        ready_to_push.set("truelist", merge(E1.get("truelist"), E2.get("truelist")));
        ready_to_push.set("falselist", E2.get("falselist"));
    }
    else if (oper == 10) {//控制中的布尔表达式A-> E and M E
        auto E2 = item[0];
        auto M = item[1];
        auto E1 = item[3];
        backpatch(E1.get("truelist"), M.get("quad"));
        ready_to_push.set("truelist", E2.get("truelist"));
        ready_to_push.set("falselist", merge(E1.get("falselist"), E2.get("falselist")));
    }
    else if (oper == 11) {//控制中的布尔表达式A-> not E
        auto E1 = item[0];
        ready_to_push.set("truelist", E1.get("falselist"));
        ready_to_push.set("falselist", E1.get("truelist"));
    }
    else if (oper == 12) {//N->@
        ready_to_push.set("nextlist", to_string(nextquad));
        emit("j", "-", "-", "0");
    }
    else if (oper == 13) {//M->@
        ready_to_push.set("quad",to_string(nextquad));
    }
    else if (oper == 14) {//控制E中的relop三元式
        auto id2 = item[0];
        auto id1 = item[2];
        auto relop = item[1];
        string operater = relop.get("oper");
        ready_to_push.set("truelist", to_string(nextquad));
        ready_to_push.set("falselist", to_string(nextquad + 1));
        emit("j"+operater, id1.get("place"), id2.get("place"), "0");//result为0自动作为链表
        emit("j", "-", "-", "0");
    }
    else if (oper == 15) {//E->const_int
        auto const_int = item[0];
        //ready_to_push.parameter = const_int.parameter;
        auto new_temp=variable_table.newtemp();
        emit("=", const_int.get("value"), "-", new_temp);
        ready_to_push.set("place", new_temp);
    }

    else if (oper == 16) {//布尔表达式中E->id操作(此次id改成非终结符C_ADD,支持加减乘除等操作
        auto id = item[0];
        ready_to_push.set("truelist", to_string(nextquad));
        ready_to_push.set("falselist", to_string(nextquad + 1));
        emit("jnz", id.get("place"), "-", "0");
        emit("j", "-", "-", "0");
    }
    else if (oper == 17){//控制s->if ( E ) M Statement_block
        auto E = item[3];
        auto M = item[1];
        auto Statement_block = item[0];
        backpatch(E.get("truelist"), M.get("quad"));
        ready_to_push.set("nextlist", merge(E.get("falselist"), Statement_block.get("nextlist")));
    }
    else if (oper == 18){//控制s -> if ( E ) M Statement_block N else M Statement_block
        auto E = item[7];
        auto M1 = item[5];
        auto M2 = item[1];
        auto Statements = item[4];
        auto N = item[3];
        auto Statement_block = item[0];
        backpatch(E.get("truelist"), M1.get("quad"));
        backpatch(E.get("falselist"), M2.get("quad"));
        ready_to_push.set("nextlist", merge(merge(Statements.get("nextlist"), N.get("nextlist")), Statement_block.get("nextlist")));
    }
    else if (oper == 19){//控制s -> while M ( E ) M Statement_block
        auto Statement_block = item[0];
        auto M2 = item[1];
        auto E = item[3];
        auto M1 = item[5];
        backpatch(E.get("truelist"), M2.get("quad"));
        backpatch(Statement_block.get("nextlist"), M1.get("quad"));
        ready_to_push.set("nextlist", E.get("falselist"));
        emit("j", "-", "-", M1.get("quad"));
    }
    else if(oper==20){//s->L M S
        auto s=item[0];
        auto l=item[2];
        auto M=item[1];
        backpatch(l.get("nextlist"),M.get("quad"));
        ready_to_push.set("nextlist",s.get("nextlist"));
    }
    else if(oper==21)//f->s
    {
        auto s=item[0];
        backpatch(s.get("nextlist"),to_string(nextquad));
    }
    else if(oper==22){//Create_variable,新函数新建变量表,输出函数名称
        emit("func","-","-",string_stack[string_stack.size()-4].get("name"));
        variable_table.new_function();
    }
    else if(oper==23){//进入代码块，新建变量层
        variable_table.newlayer();
    }
    else if(oper==24){//s-> { variable_new_layer Statements }完成一层，销毁变量层
        auto Statements=item[1];
        variable_table.poplayer();
        ready_to_push.set("nextlist",Statements.get("nextlist"));
    }
    string_stack.push_back(ready_to_push);
    return ACC;
}

vector<parsetoken> init_input(const vector<string>& origin,const vector<string>& syntax_input_buf){
    vector<parsetoken> res;
    for (int i=0;i<origin.size();i++){
        parsetoken temp=syntax_input_buf[i];
        if(temp.symbol=="id"){
            temp.set("name",origin[i]);
        }
        else if(temp.symbol=="const_int"){
            temp.set("value",origin[i]);
        }
        res.push_back(temp);
    }
    return res;
}

void LR_analyse::load_languange(){//加载文法
    fstream in("source/my_rule.gr.i", ios::in);
    if (in.is_open() == 0) {
        cout << "无法打开my_rule.gr.i" << endl;
    }
    string begin;
    getline(in, begin);

    string line;

    while (getline(in, line)) {
        std::stringstream ss(line);
        string starter, change;
        ss >> starter >> change;
        if (change != "->")
            break;
        vector<string>end;
        while (true) {
            string temp;
            ss >> temp;
            if (temp.empty())
                break;
            end.push_back(temp);
            if (ss.eof() == 1)
                break;
        }
        int do_item=stoi(end.back());//获取执行函数代码
        end.pop_back();
        lang.add({ starter }, end,do_item);
    }
}

void LR_analyse::load_action() {//加载action表
    fstream in("source/my_rule.gr.act", ios::in);
    if (in.is_open() == 0) {
        cout << "无法打开my_rule.gr.act" << endl;
    }
    int statenum;
    in >> statenum;
    string termin_line;
    vector<string>termin;
    getline(in, termin_line);
    getline(in, termin_line);
    stringstream ss(termin_line);
    while (true) {
        string temp;
        ss >> temp;
        if (temp.empty())
            break;
        termin.push_back(temp);
        if (ss.eof() == 1)
            break;
    }

    ACTION = new ACTIONGO_table(statenum, termin);

    string line;
    while (getline(in, line)) {
        string ter, s_state;
        int begin_state, end_state;
        stringstream nss(line);
        nss >> ter >> begin_state >> s_state >> end_state;
        int change_state = s_state == "s" ? SHIFT :
                           s_state == "r" ? REDUCE :
                           ACC;
        ACTION->change(begin_state, ter, state{ change_state,end_state });
    }
}

void LR_analyse::load_goto() {//加载goto表
    fstream in("source/my_rule.gr.goto", ios::in);
    if (in.is_open() == 0) {
        cout << "无法打开my_rule.gr.goto" << endl;
    }
    int statenum;
    in >> statenum;
    string termin_line;
    vector<string>termin;
    getline(in, termin_line);
    getline(in, termin_line);
    stringstream ss(termin_line);
    while (true) {
        string temp;
        ss >> temp;
        if (temp.empty())
            break;
        termin.push_back(temp);
        if (ss.eof() == 1)
            break;
    }

    GO = new ACTIONGO_table(statenum, termin);

    string line;
    while (getline(in, line)) {
        string ter, s_state;
        int begin_state, end_state;
        stringstream nss(line);
        nss >> ter >> begin_state >> end_state;
        GO->change(begin_state, ter, state{ ERR,end_state });
    }
}


string show_sentence(const vector<parsetoken>& sentence, int location) {//展示
    if (location >= sentence.size())
        return "error";
    string ans;
    for (int i= location;i<sentence.size();i++){
        ans += "<" + string(sentence[i]) + ">";
    }
    return ans;
}

string show_sentence(const vector<string>& sentence, int location) {//展示
    if (location >= sentence.size())
        return "error";
    string ans;
    for (int i= location;i<sentence.size();i++){
        ans += "<" + sentence[i] + ">";
    }
    return ans;
}

ostream& operator<<(ostream& out, four_tuple& tmp)//输出四元式
{
    cout<<setw(3)<<tmp.op<<","<<setw(3)<<tmp.arg1<<","<<setw(3)<<tmp.arg2<<","<<setw(3)<<tmp.result;
    return out;
}

void LR_analyse::emit(const string& op, const string& arg1, const string& arg2, const string& result)//发射四元式
{
    nextquad += 1;
    four_tuple temp{ op,arg1,arg2,result };
    Intermediate_code.push_back(temp);
    //cout<<setw(3)<<op<<","<<setw(3)<<arg1<<","<<setw(3)<<arg2<<","<<setw(3)<<result<<endl;
}

void LR_analyse::backpatch(const string & merge_father, const string & content)
{
    if(merge_father.empty() or content.empty())
        return;
    int begin=stoi(merge_father);
    int tmp;
    while (Intermediate_code[begin].result!="0") {
        tmp=stoi(Intermediate_code[begin].result);
        Intermediate_code[begin].result = content;
        begin = tmp;
    }
    Intermediate_code[begin].result = content;
}

string LR_analyse::merge(const string & merge_father, const string & merge_son)
{
    if(merge_father.empty() or merge_son.empty())
        return "";
    //首先找到merge_father链表的尾部
    auto begin= merge_father ;
    while(true){
        if(Intermediate_code[stoi(begin)].result=="0") {//找到结尾
            Intermediate_code[stoi(begin)].result = merge_son;//接上
            return merge_father;
        }
        else
            begin = Intermediate_code[stoi(begin)].result;
    }
}



