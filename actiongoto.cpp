#include "actiongoto.h"
#include "lexical_analyzer.h"

ACTIONGO_table::ACTIONGO_table(int state_sum, const vector<string>& terminator) {
    terminator_list = terminator;
    all_state_sum = state_sum;
    vector<vector<state>>vec(terminator.size(), vector<state>(state_sum, state()));
    content = vec;
}

const state ACTIONGO_table::get(int state_number, string terminat) {
    int ter_index = get_index(terminat);
    if (state_number >= all_state_sum || ter_index == -1)
        return state();

    return content[ter_index][state_number];
}

void ACTIONGO_table::change(int state_number, string terminat, const state& new_state) {
    content[get_index(terminat)][state_number] = new_state;
}

int ACTIONGO_table::get_index(const string& terminat) {
    for (int i = 0; i < terminator_list.size(); i++) {
        if (terminat == terminator_list[i])
            return i;
    }
    return -1;
}

parsetoken::parsetoken(const string& init) {
    symbol = init;
    if (symbol == "id")
        tag = ID;
    else if (kw_kv.find(symbol) != kw_kv.end() || dl_kv.find(symbol) != dl_kv.end() || symbol=="@" || symbol == "$")
        tag = TERMINAL;
    else if (op_kv.find(symbol) != op_kv.end())
        tag = OPERATOR;
    else if (symbol == "const_int")
        tag = CONST_INT;
    else
        tag = NONTERMINAL;
}

ostream& operator<<(ostream& out, parsetoken& tmp) {
    out << tmp.symbol;
    return out;
}

parsetoken::operator string() const{
    return symbol;
}


string variable_table_class::lookup(const string& variable_name)
{
    string re;
    for (auto it = table.rbegin(); it != table.rend(); it++)
        if ((*it).find(variable_name)!=(*it).end()) {
            re = (*it)[variable_name];
            break;
        }
    return re;
}

string variable_table_class::newvar(const string& variable_name) {
    string re = "t" + to_string(temp_count.back());
    temp_count.back()++;
    table.back()[variable_name] = re;
    return re;
}
string variable_table_class::newtemp() {
    string re = "t" + to_string(temp_count.back());
    temp_count.back()++;
    return re;
}

string parsetoken::get(const string& para_name) {
    return parameter[para_name];
}

void parsetoken::set(const string& para_name, string content)
{
    parameter[para_name] = content;
}

void variable_table_class::newlayer()
{
    current_layer+=1;
    table.push_back(map<string, string>());
    temp_count.push_back(temp_count.back());
}
void variable_table_class::poplayer()
{
    current_layer-=1;
    table.pop_back();
    temp_count.pop_back();
}

variable_table_class::variable_table_class(){
    current_layer+=1;
    temp_count.push_back(0);
    table.push_back(map<string, string>());
}

void variable_table_class::new_function(){
    *this=variable_table_class();
}