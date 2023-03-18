#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <set>
#include <map>

using namespace std;

/***********************************
 *
 *  if the member body is empty, 
 *  the body is a epsilon
 *
 ***********************************/

const int ACT_ERR = 0;
const int ACT_SHIFT = 1;
const int ACT_REDUCE = 2;
const int ACT_ACCEPT = 3;

const string EPSILON = "@"; //  epsilon symbol

struct Production{
    string head;
    vector<string> body;
    int no;
    bool operator<(const Production &p)const;// used for iterator compare
    bool operator==(const Production &p)const// used for iterator compare
    {
        if(head != p.head)
            return false;

        if(this->body.size()!=p.body.size())
            return false;

        for(int i=0;i<p.body.size();i++){
            if(body[i]!=p.body[i])
                return false;
        }

        return true;
    }
};

struct Token{
    string token;   // either terminal or nonterminal
    bool has_first; // true if the token has get its first set     
    bool has_follow;// true if the token has get its follow set     
    Token(string t, bool ft, bool fw){
        token = t;
        has_first = ft;
        has_follow = fw;
    }
    bool operator<(const Token &t)const 
    {// used for iterator compare
        return this->token < t.token;
    }
};

struct LR1Item{
    int pdc_no;     //  the production no in the pdcs member
    int dot_pos;    //  the dot position in the production body, 0 refer to the left end 
    string lookahead;   //  the lookahead symbol 

    LR1Item(int a, int b, string l){
        pdc_no = a;
        dot_pos = b;
        lookahead = l;
    }

    bool operator<(const LR1Item& rhs)const
    {
        if(pdc_no<rhs.pdc_no)
            return true;
        else if(pdc_no == rhs.pdc_no && dot_pos < rhs.dot_pos)
            return true;
        else if(pdc_no == rhs.pdc_no && dot_pos == rhs.dot_pos && lookahead < rhs.lookahead)
            return true;

        return false;
    }

    bool operator==(const LR1Item& rhs)const
    {
        if(pdc_no == rhs.pdc_no && dot_pos == rhs.dot_pos && lookahead == rhs.lookahead)
            return true;
        else
            return false;
    }
};

struct Action{
    int type;           //  action type 0:err, 1:shift, 2:reduce, 3:accept
    int status_pdcno;   //  the status after execute action
    Action(){
        type = ACT_ERR;
        status_pdcno = -1;
    }
};

class Grammar{
private:
    vector<Production> pdcs;    // grammar production sets
    set<Token> tokens;          // grammar symbols of all terminals and nonterminals
    set<string> nonterms;       // grammar symbols of all nonterminals
    set<string> terms;          // grammar symbols of all terminals 
    const string start = "AAA"; // grammar start symbol

    map<string, int> term_map;      // get a terminal's sequential no
    map<string, int> nonterm_map;   // get a nonterminal's sequential no

    vector<vector<Action> > action_table; // action table
    vector<vector<int> > goto_table;      // goto table, row:state col:tokens(include $)

    map<string, bool> token_derive_eps; 
    map<string, set<string> > first;    // first sets of grammar
    map<string, set<string> > follow;   // follow sets of grammar

    vector<set<LR1Item> > CC;    // canonical collection of sets of LR1 closure items

    void InitSymbolMap();
    void InitCC();
    void Closure(set<LR1Item>& s);
    set<LR1Item> GoTo(const set<LR1Item>& s, string token);

public:
    void GenTransFiles(const char*);

    void Init(const char* rule_path);
    void Get_first(string token);
    void Get_follow();
    bool derive_eps(string token);
    set<Token>::iterator Find_token(string token);
    void FillTable();

    bool NextSymbolGenEps(const string& nonterm);
    bool RecurGenEps(const string& nonterm);
    int FindPrevActStat(int now_stat, const string& act_term);

    
    int GetTermNo(string term){
        return term_map[term];
    }
    int GetNonTermNo(string nonterm){
        return nonterm_map[nonterm];
    }
    int GetPdcNo(const Production& p){
        int p_count = 0;
        for(auto& pb:pdcs){
            if(pb == p)
                break;
            p_count++;
        }
        return p_count;
    }

};

