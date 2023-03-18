#include "syntactic_analyzer.h"
#include <fstream>

bool IsInteger(string buf)
{
    for(auto& p:buf){
        if(p<'0'||p>'9')
            return false;
    }

    return true;
}

bool Grammar::NextSymbolGenEps(const string& nonterm)
{
    for(auto& p:pdcs){
        for(int i =0;i<p.body.size()-1;i++){
            if(p.body[i]==nonterm){
                for(auto& cp:pdcs){
                    if(p.body[i+1] == cp.head && cp.body[0]==EPSILON)
                        return true;
                }
            }
        }
    }

    return false;
}

bool Grammar::RecurGenEps(const string& nonterm)
{
//    cout << nonterm <<endl;
    if(NextSymbolGenEps(nonterm))
        return true;
    for(auto & p: pdcs){
        if(p.body[p.body.size()-1] == nonterm && p.head != nonterm)
        {
            if(p.head == "AAA")
                return false;
            else if(RecurGenEps(p.head))
                return true;
        }
    }

    return false;
}

int Grammar::FindPrevActStat(int now_stat, const string& act_term)
{
    for(auto i = 0;i<CC.size();i++){
        Action& act = action_table[i][GetTermNo(act_term)];
        if(act.type == ACT_SHIFT && act.status_pdcno == now_stat)
            return i;
    }

    return -1;
}

bool Production::operator<(const Production &p)const//constÒ»¶¨ÒªÐ´
{
    if(this->head<p.head)
        return true;
    else if(this->head == p.head){
        for(int i=0;i<this->body.size();i++){
            if(i<p.body.size()&&(this->body)[i]<(p.body)[i])
                return true;
        }
    }
    else{
        return false;
    }
    return false;
}

bool Inbody(const string& token, const Production& p){
    for(auto & b:p.body){
        if(token == b)
            return true;
    }

    return false;
}

bool InSet(const string& token, const set<string>& s){
    for(auto & e:s){
        if(token == e)
            return true;
    }

    return false;
}

bool BelongItemSet(const set<LR1Item>& sub, const set<LR1Item>& main){
    for(auto sub_iter=sub.begin();sub_iter!=sub.end();sub_iter++){
        if(main.count(*sub_iter)<1)
            return false;
    }

    return true;
}

void Grammar::Closure(set<LR1Item>& s){

    set<LR1Item> old_item = s;
    while(1){
        for(auto& i:s){
            Production& p = pdcs[i.pdc_no];
            if(i.dot_pos < p.body.size() && nonterms.count(p.body[i.dot_pos])){
                for(auto& pb:pdcs){
                    if(pb.head == p.body[i.dot_pos]){
                        int pb_no = GetPdcNo(pb);
                        string la;
                        if(i.dot_pos == p.body.size()-1){
                            la = i.lookahead;
                            LR1Item item = LR1Item(pb_no, 0, la); 
                            s.insert(item);
                        }
                        else{
                            string next = p.body[i.dot_pos+1];
                            for(auto& t:first[next]){
                                if(t==EPSILON)
                                    continue;

                                la = t;
                                LR1Item item = LR1Item(pb_no, 0, la); 
                                s.insert(item);
                            }
                            
                            if(derive_eps(next)){
                                la = i.lookahead;
                                LR1Item item = LR1Item(pb_no, 0, la); 
                                s.insert(item);
                            }
                        }
                    }
                }
            }

        }

        //  if s is not changing, then break
        if(old_item == s)
            break;
        else
            old_item = s;
    }

}

set<LR1Item> Grammar::GoTo(const set<LR1Item>& s, string token){
    set<LR1Item> moved;
    
    for(auto& i:s){
        Production& p = pdcs[i.pdc_no];
        if(i.dot_pos < p.body.size() && p.body[i.dot_pos] == token){
            moved.insert(LR1Item(i.pdc_no, i.dot_pos+1, i.lookahead));
        }
    }

    Closure(moved);
    return std::move(moved); 
}

void Grammar::InitCC(){
    vector<bool> mark_set;

    //  set CC0 attributes
    set<LR1Item> CC0;
    LR1Item start0(0, 0, "$");
    CC0.insert(start0);
    Closure(CC0);

    CC.push_back(CC0);
    mark_set.push_back(false);
    goto_table.push_back(vector<int>(tokens.size(), -1));

    int old_cc_size = 1;

    while(1)
    {
        for(int i=0;i<mark_set.size();i++){
            if(!mark_set[i]){
                mark_set[i] = true;

                int iter_cnt = 0;
                for(auto item=CC[i].begin();item!=CC[i].end();item++, iter_cnt++){
                //for(auto& item : CC[i]){ 
                    if(item->dot_pos<pdcs[item->pdc_no].body.size()){
                    //if(item.dot_pos<pdcs[item.pdc_no].body.size()){

                        string new_la = pdcs[item->pdc_no].body[item->dot_pos];
                        set<LR1Item> temp = GoTo(CC[i], new_la);

                        //  find whether temp is in CC or not
                        int temp_pos = -1;
                        for(int cc=0;cc<CC.size();cc++){
                            if(CC[cc]==temp){
                                temp_pos = cc;
                                break;
                            }
                        }

                        //  add temp to CC
                        if(temp_pos==-1){
                            temp_pos = CC.size();
                            CC.push_back(temp);
                            goto_table.push_back(vector<int>(tokens.size(), -1));
                            mark_set.push_back(false);
                        }

                        if(terms.count(new_la))
                            goto_table[i][GetTermNo(new_la)] = temp_pos;
                        else
                            goto_table[i][GetNonTermNo(new_la)+terms.size()] = temp_pos;
                    }

                    item = CC[i].begin();
                    for(int k=0;k<iter_cnt;k++){
                        item++;
                    }

                }
            }
        }

        //  if there is no new sets added to CC
        //  stop Construct CC
        if(old_cc_size == CC.size())
            break;
        else
            old_cc_size = CC.size(); 
    }

    //  put CC of grammar 
    //cout <<"================================= LR1 sets of Items ================================="<<endl;
    //int cc_count = 0;
    //for(auto& c:CC){
    //    cout <<"------------------------------------------------------------------"<<endl;
    //    cout << "CC" << cc_count++ <<":"<<endl;
    //    for(auto& s:c){
    //        Production& p=pdcs[s.pdc_no];
    //        cout  <<left << setw(20) << p.head << "->"<<setw(10)<<" ";

    //        int pos_count = 0;
    //        for(auto& b:p.body){
    //            if(pos_count == s.dot_pos){
    //                cout << "^ ";
    //            }
    //            cout << b << " ";
    //            pos_count++;
    //        }
    //        if(s.dot_pos == p.body.size())
    //            cout << "^ ";

    //        cout << ", " << s.lookahead;
    //        cout <<endl;
    //    }
    //}

    //  put Goto table
//    cout <<"================================= Goto Table ================================="<<endl;
//
//    //  put col name
//    cout << left;
//    cout << "     ";
//    for(auto& t:terms){
//        cout <<setw(12) << t;
//    }
//    for(auto& n:nonterms){
//        cout <<setw(12) << n;
//    }
//    cout << endl;
//    cout << "     ";
//    for(auto& t:tokens){
//        cout << "------------";
//    }
//    cout <<endl;
//
//    int s_cnt = 0;
//    for(auto& g:goto_table){
//        //  put row name
//        cout << "S" << setw(2) << s_cnt++ << " |";
//        for(auto& t:terms){
//            if(g[GetTermNo(t)]==-1){
//                ////cout << setw(12) <<"null";
//                cout << setw(12) <<" ";
//            }
//            else{
//                cout << setw(12) <<g[GetTermNo(t)];
//            }
//        }
//        for(auto& n:nonterms){
//            if(g[GetNonTermNo(n)+terms.size()]==-1){
//                //cout << setw(12) <<"null";
//                cout << setw(12) <<" ";
//            }
//            else{
//                cout << setw(12) <<g[GetNonTermNo(n)+terms.size()];
//            }
//        }
//        cout << endl;
//    }
}

void Grammar::FillTable(){
    //  init action table
    //  the length of each row is terms.size()+1, the last refers to $ action 
    for(int i = 0;i<CC.size();i++){
        action_table.push_back(vector<Action>(terms.size()+1));
    }

    for(auto i = 0;i<CC.size();i++){
        for(auto item = CC[i].begin();item!=CC[i].end();item++){
            const Production& p = pdcs[item->pdc_no]; 
            //  if I is A->beta ^ c gama, a  and goto(CC[i] ,c) = CC[j] 
            //  then Action[i, c] = "shift j"
            if(item->dot_pos < p.body.size() && terms.count(p.body[item->dot_pos]) \
                    && goto_table[i][GetTermNo(p.body[item->dot_pos])] != -1 ){
                Action& act=action_table[i][GetTermNo(p.body[item->dot_pos])]; 
                act.type= ACT_SHIFT;
                act.status_pdcno = goto_table[i][GetTermNo(p.body[item->dot_pos])]; 
            }
            //  else if I is A->beta ^, a , a is either $ or terms 
            //  then Action[i ,a] = "reduce A->beta"
            else if(item->dot_pos == p.body.size() && item->pdc_no!=0 && (terms.count(item->lookahead) || item->lookahead=="$")){
                int la_pos;
                if(terms.count(item->lookahead))
                    la_pos=GetTermNo(item->lookahead);
                else
                    la_pos=terms.size();

                Action& act=action_table[i][la_pos]; 
                act.type =ACT_REDUCE;
                act.status_pdcno=item->pdc_no;
            }
            //  else if I is S' -> S^, $ then accept
            else if(item->dot_pos==1 && item->pdc_no==0 && item->lookahead=="$"){
                Action& act=action_table[i][terms.size()]; 
                act.type =ACT_ACCEPT;
            }
        }
    }

    // fill the special empty case
    for(auto i = 0;i<CC.size();i++){
        for(auto item = CC[i].begin();item!=CC[i].end();item++){
            const Production& p = pdcs[item->pdc_no]; 
            //if(item->dot_pos==p.body.size() && InSet(p.body[item->dot_pos-1], terms)){
            if(item->dot_pos==p.body.size()){
                if(RecurGenEps(p.body[p.body.size()-1])){
                    // insert new item
                    //cout <<"!!! "<< i << terms.size()<<endl;
                    LR1Item eps_item = *item;
                    eps_item.lookahead = "@";
                    CC[i].insert(eps_item);

                    // modify action table
                    Action& act=action_table[i][GetTermNo(EPSILON)];
                    act.type = ACT_REDUCE;
                    act.status_pdcno = GetPdcNo(p);
                    break;
                }
            }
        }
    }
    
//    set<string> gen_epss;
//    for(auto & p:pdcs){
//        if(p.body[0]==EPSILON){
//            gen_epss.insert(p.head);
//        }
//    }

    // fist sets 
    for(auto i = 0;i<CC.size();i++){
        for(auto item = CC[i].begin();item!=CC[i].end();item++){
            const Production& p = pdcs[item->pdc_no]; 
            if(p.body[0]==EPSILON && item->dot_pos==1){
                int prev_state = FindPrevActStat(i, EPSILON);
                if(prev_state<0){
                    //cout << "state: "<< i <<" | find prev stat err!" <<endl;
                    continue;
                    exit(1);
                }
                else {
                    //cout << "state: "<< i <<" | find prev stat succ! : " << prev_state <<endl;
                }
                const set<LR1Item>& PrevItemSet = CC[prev_state];
                for(auto& previtem:PrevItemSet){
                    const Production& pp = pdcs[previtem.pdc_no]; 
                    if(pp.body[previtem.dot_pos]==p.head && previtem.dot_pos<pp.body.size()-1){
                        const set<string>& next_firstSet = first[pp.body[previtem.dot_pos+1]];
                        for(auto& nf:next_firstSet){
                            cout << nf <<endl;
                            Action& act = action_table[i][GetTermNo(nf)];
                            if(act.type!=ACT_REDUCE){
                                act.type = ACT_REDUCE;
                                act.status_pdcno=item->pdc_no;
                            }
                        }
                    }
                }
            }
        }
    }

    // solve shift / reduce conflict in the if/ if else Statement
    Production p_if;
    p_if.head = "If_sentence";
    p_if.body.push_back("if");
    p_if.body.push_back("(");
    p_if.body.push_back("E");
    p_if.body.push_back(")");
    p_if.body.push_back("M");
    p_if.body.push_back("Statement_block");

    int next_stat;
    for(auto i = 0;i<CC.size();i++){
        for(auto item = CC[i].begin();item!=CC[i].end();item++){
            const Production& p = pdcs[item->pdc_no]; 
            if(p.head == "N" && p.body[0] == EPSILON && item->dot_pos==1)
            {
                next_stat = i;
                break;
            }
        }
    }

    for(auto i = 0;i<CC.size();i++){
        for(auto item = CC[i].begin();item!=CC[i].end();item++){
            const Production& p = pdcs[item->pdc_no]; 
            if(p_if == p && item->lookahead == EPSILON){
                Action& act = action_table[i][GetTermNo(EPSILON)];
                act.type = ACT_SHIFT;
                act.status_pdcno = next_stat;
                break;
            }
        }
    }

    

    //  put Action table
//    cout <<"================================= Action Table ================================="<<endl;
//
//    //  put col name
//    cout << "     ";
//    for(auto& t:terms){
//        cout <<setw(12) << t;
//    }
//    cout << setw(12) << "$";
//    cout << endl;
//
//    cout << "     ";
//    for(int i=0;i<=terms.size();i++){
//        cout << "------------";
//    }
//    cout <<endl;
//
//    int s_cnt = 0;
//    for(auto& a:action_table){
//        //  put row name
//        cout << "S" << setw(2) << s_cnt++ << " |";
//        for(auto& t:terms){
//            if(a[GetTermNo(t)].type==ACT_ERR){
//                cout << setw(12) <<" ";
//                //cout << setw(12) <<"null";
//            }
//            else if(a[GetTermNo(t)].type==ACT_SHIFT){
//                cout << "s " << setw(6) << a[GetTermNo(t)].status_pdcno << "    ";
//            }
//            else if(a[GetTermNo(t)].type==ACT_REDUCE){
//                cout << "r " << setw(6) << a[GetTermNo(t)].status_pdcno << "    ";
//            }
//            else{
//                cout << setw(12) <<"acc";
//            }
//        }
//        // put the $ colnum
//        if(a[terms.size()].type==ACT_ERR){
//            cout << setw(12) <<" ";
//            //cout << setw(12) <<"null";
//        }
//        else if(a[terms.size()].type==ACT_SHIFT){
//            cout << "s " << setw(6) << a[terms.size()].status_pdcno << "    ";
//        }
//        else if(a[terms.size()].type==ACT_REDUCE){
//            cout << "r " << setw(6) << a[terms.size()].status_pdcno << "    ";
//        }
//        else{
//            cout << setw(12) <<"acc";
//        }
//
//        cout << endl;
//    }
//
//    //  put CC of grammar
//    cout <<"================================= LR1 sets of Items ================================="<<endl;
//    int cc_count = 0;
//    for(auto& c:CC){
//        cout <<"------------------------------------------------------------------"<<endl;
//        cout << "CC" << cc_count++ <<":"<<endl;
//        for(auto& s:c){
//            Production& p=pdcs[s.pdc_no];
//            cout  <<left << setw(20) << p.head << "->"<<setw(10)<<" ";
//
//            int pos_count = 0;
//            for(auto& b:p.body){
//                if(pos_count == s.dot_pos){
//                    cout << "^ ";
//                }
//                cout << b << " ";
//                pos_count++;
//            }
//            if(s.dot_pos == p.body.size())
//                cout << "^ ";
//
//            cout << ", " << s.lookahead;
//            cout <<endl;
//        }
//    }
}

/***************************************************************
 *  the empty body must appear at the 
 *  first place
 *
 *  func purpose:
 *      1.  read grammar file and extract 
 *          grammar elements
 *      2.  generate first and follow sets 
 *          by the productions 
 **************************************************************/
void Grammar::Init(const char* rule_path){
    //  open grammar rule file
    ifstream rulefile(rule_path, ios::in|ios::binary);
    ofstream ifile(string("source/") + rule_path + ".i");
    if(!rulefile.is_open()){
        cerr << "can't open rule file!" << endl;
    }
    
    string token_buf;
    string prev_buf;

    Production tmp_pdc = Production();
    tmp_pdc.no = 0;

    bool start_flag = true;

    //  read file, extract tokens and find nonterms
    while(rulefile >> token_buf){
        if(token_buf[token_buf.size()-1] == ':'){
            nonterms.insert(token_buf.substr(0, token_buf.size()-1));
            if(!start_flag){
                //  insert last-read production into pdcs
                if(IsInteger(prev_buf)){
                    tmp_pdc.no = std::stoi(prev_buf);
                    tmp_pdc.body.pop_back();
                }

                if(tmp_pdc.body.size()){
                    pdcs.push_back(tmp_pdc);
                }
            }
            else{
                //  Get augmented Start symol and production
                string prog_head = token_buf.substr(0, token_buf.size()-1);
                Production augmented_p;
                augmented_p.head = start;
                augmented_p.body.push_back(prog_head);
                augmented_p.no = 0;

                //  Add augmented Start symol and production
                nonterms.insert(start);
                tokens.insert(Token(start, false, false));
                pdcs.push_back(augmented_p);

                //  set start_flag to false
                start_flag = false;
            }
            // clear tmp production
            tmp_pdc = Production();

            //  Add head symbol
            tmp_pdc.head = token_buf.substr(0, token_buf.size()-1);
            tokens.insert(Token(tmp_pdc.head, false, false));

            prev_buf = token_buf;
        }
        else if(token_buf == "|"){
            // find a empty production
            if(prev_buf[prev_buf.size()-1]==':'){
                tmp_pdc.body = vector<string>();
                tmp_pdc.no = 0;
                tmp_pdc.body.push_back(EPSILON);
                terms.insert(EPSILON);
                tokens.insert(Token(EPSILON, false,false));
            }

            if(IsInteger(prev_buf) && tmp_pdc.body.size()==1){
                tmp_pdc.body = vector<string>();
                tmp_pdc.no = std::stoi(prev_buf);
                tmp_pdc.body.push_back(EPSILON);
                terms.insert(EPSILON);
                tokens.insert(Token(EPSILON, false,false));
            }

            else if(IsInteger(prev_buf)){
                tmp_pdc.no = std::stoi(prev_buf);
                tmp_pdc.body.pop_back();
            }

            // insert new production
            pdcs.push_back(tmp_pdc);

            // clear *body* and *no* of tmp production
            tmp_pdc.body = vector<string>();
            tmp_pdc.no = 0;

            prev_buf = token_buf;
        }
        else{
            if(!IsInteger(token_buf))
                tokens.insert(Token(token_buf, false, false));
            tmp_pdc.body.push_back(token_buf);
            prev_buf = token_buf;
        }
    }

    //  push the last production
    if(IsInteger(prev_buf)){
        tmp_pdc.no = std::stoi(prev_buf);
        tmp_pdc.body.pop_back();
    }
    pdcs.push_back(tmp_pdc);

    //  put pdcs
    ifile <<"================================= Productions ================================="<<endl;
    for(auto& p:pdcs){
        ifile <<left << setw(28) << p.head << "->" << setw(10) << " ";
        for(auto& b:p.body){
            ifile << b << " ";
        }
        ifile << p.no << endl;
        //ifile <<endl;
    }
    ifile << "pdc num: "<< pdcs.size()<<endl;

    //  put tokens 
    ifile <<"================================= tokens ================================="<<endl;
    for(auto& t:tokens){
        ifile << t.token << endl;
    }
    ifile << "token num: "<< tokens.size()<<endl;

    
    //  Init terminal set
    for(auto& t:tokens){
        if(nonterms.find(t.token)==nonterms.end()){
            terms.insert(t.token);
        }
    }
    //  Init Symbol Map
    InitSymbolMap();

    ifile <<"================================= NonTerminals No ================================="<<endl;
    for(auto& n:nonterms){
        ifile <<left << setw(20) << n << ": " << setw(10) << " ";
        ifile << nonterm_map[n] << " ";
        ifile << endl;
    }
    ifile << "nonterm num: "<< nonterms.size()<<endl;

    ifile <<"================================= Terminals No ================================="<<endl;
    for(auto& t:terms){
        ifile <<left << setw(20) << t << ": " << setw(10) << " ";
        ifile << term_map[t] << " ";
        ifile << endl;
    }
    ifile << "term num: "<< terms.size()<<endl;

    //  get and put first sets 
    for(auto t=tokens.begin();t!=tokens.end();t++){
        Get_first(t->token);
    }

    ifile <<"================================= First Sets ================================="<<endl;
    for(auto& f:first){
        ifile <<left << setw(20) << f.first << ": " << setw(10) << " ";
        for(auto& s:f.second){
            ifile << s << " ";
        }
        ifile << endl;
    }

    //  get and put follow sets 
    Get_follow();

    ifile <<"================================= Follow Sets ================================="<<endl;
    for(auto& f:follow){
        ifile <<left << setw(20) << f.first << ": " << setw(10) << " ";
        for(auto& s:f.second){
            ifile << s << " ";
        }
        ifile << endl;
    }

    InitCC();

    FillTable();

    rulefile.close();
}
//void Grammar::Init(const char* rule_path){
//    //  open grammar rule file
//    ifstream rulefile(rule_path, ios::in|ios::binary);
//    ofstream ifile(string("source/") + rule_path + ".i");
//    if(!rulefile.is_open()){
//        cerr << "can't open rule file!" << endl;
//    }
//    
//    string token_buf;
//    string prev_buf;
//
//    Production tmp_pdc;
//
//    bool start_flag = true;
//
//    //  read file, extract tokens and find nonterms
//    while(rulefile >> token_buf){
//        if(token_buf[token_buf.size()-1] == ':'){
//            if(!start_flag){
//            //  insert p into production
//                if (tmp_pdc.body.size()!=0)
//                    pdcs.push_back(tmp_pdc);
//                nonterms.insert(tmp_pdc.head);
//            }
//            else{
//                //  add augmented Start symol and production
//                string prog_head = token_buf.substr(0, token_buf.size()-1);
//                Production augmented_p;
//                augmented_p.head = start;
//                augmented_p.body.push_back(prog_head);
//                pdcs.push_back(augmented_p);
//                tokens.insert(Token(start, false, false));
//                nonterms.insert(start);
//
//                //  set start_flag to false
//                start_flag = false;
//            }
//
//            tmp_pdc.head = token_buf.substr(0, token_buf.size()-1);
//            tokens.insert(Token(tmp_pdc.head, false, false));
//            tmp_pdc.body = vector<string>();
//
//            prev_buf = token_buf;
//        }
//        else if(token_buf == "|"){
//            if(prev_buf[prev_buf.size()-1]==':'){
//                tmp_pdc.body = vector<string>();
//                tmp_pdc.body.push_back(EPSILON);
//                terms.insert(EPSILON);
//                tokens.insert(Token(EPSILON, false,false));
//            }
//
//            pdcs.push_back(tmp_pdc);
//            tmp_pdc.body = vector<string>();
//
//            nonterms.insert(tmp_pdc.head);
//
//            prev_buf = token_buf;
//        }
//        else{
//            tokens.insert(Token(token_buf, false, false));
//            tmp_pdc.body.push_back(token_buf);
//            prev_buf = token_buf;
//        }
//    }
//
//    //  put the last production
//    pdcs.push_back(tmp_pdc);
//
//    //  put pdcs
//    ifile <<"================================= Productions ================================="<<endl;
//    for(auto& p:pdcs){
//        ifile <<left << setw(28) << p.head << "->" << setw(10) << " ";
//        for(auto& b:p.body){
//            ifile << b << " ";
//        }
//        ifile <<endl;
//    }
//    ifile << "pdc num: "<< pdcs.size()<<endl;
//
//    //  put tokens 
//    ifile <<"================================= tokens ================================="<<endl;
//    for(auto& t:tokens){
//        ifile << t.token << endl;
//    }
//    ifile << "token num: "<< tokens.size()<<endl;
//
//    
//    //  Init terminal set
//    for(auto& t:tokens){
//        if(nonterms.find(t.token)==nonterms.end()){
//            terms.insert(t.token);
//        }
//    }
//    //  Init Symbol Map
//    InitSymbolMap();
//
//    ifile <<"================================= NonTerminals No ================================="<<endl;
//    for(auto& n:nonterms){
//        ifile <<left << setw(20) << n << ": " << setw(10) << " ";
//        ifile << nonterm_map[n] << " ";
//        ifile << endl;
//    }
//    ifile << "nonterm num: "<< nonterms.size()<<endl;
//
//    ifile <<"================================= Terminals No ================================="<<endl;
//    for(auto& t:terms){
//        ifile <<left << setw(20) << t << ": " << setw(10) << " ";
//        ifile << term_map[t] << " ";
//        ifile << endl;
//    }
//    ifile << "term num: "<< terms.size()<<endl;
//
//    //  get and put first sets 
//    for(auto t=tokens.begin();t!=tokens.end();t++){
//        Get_first(t->token);
//    }
//
//    ifile <<"================================= First Sets ================================="<<endl;
//    for(auto& f:first){
//        ifile <<left << setw(20) << f.first << ": " << setw(10) << " ";
//        for(auto& s:f.second){
//            ifile << s << " ";
//        }
//        ifile << endl;
//    }
//
//    //  get and put follow sets 
//    Get_follow();
//
//    ifile <<"================================= Follow Sets ================================="<<endl;
//    for(auto& f:follow){
//        ifile <<left << setw(20) << f.first << ": " << setw(10) << " ";
//        for(auto& s:f.second){
//            ifile << s << " ";
//        }
//        ifile << endl;
//    }
//
//    InitCC();
//
//    FillTable();
//
//    rulefile.close();
//}

void Grammar::InitSymbolMap(){
    //  init nonterm map
    int nonterm_count = 0;
    for(auto& n:nonterms){
        nonterm_map.insert(pair<string, int>(n,nonterm_count));
        nonterm_count++;
    }
    //  init term map
    int term_count = 0;
    for(auto& t:terms){
        term_map.insert(pair<string, int>(t,term_count));
        term_count++;
    }
}
/***************************************************************
 *  func purpose:
 *      1.  get a token's first set
 **************************************************************/
void Grammar::Get_first(string token){
    //  if token has already get first get, return immediately
    auto tt = Find_token(token);
    if(tt->has_first)
        return;

    //  define single first set 
    pair<string, set<string> > ft;
    ft.first = token;

    //  If X is a terminal, then FIRST(X) = {X}.
    if(terms.find(token) != terms.end()){
        ft.second.insert(token);
    }
    //  If X is a nonterminal
    else{
        for(auto & p:pdcs){
            //  find the token's productions
            if(p.head == token){
                string body_head;
                if (p.body.size()==0)
                    body_head = EPSILON;
                else
                    body_head = (p.body)[0];
                //  If X -> epsilon is a production, then add epsilon to FIRST(X) 
                if(body_head == EPSILON){
                    ft.second.insert(EPSILON);
                }
                //  for production X -> Y1 Y2 ~ Yk 
                //  If Y1 does not derive epsilon then we adi First(Y1) and nothing more to FIRST(X), 
                //  but if Y1 derives epsilon, then we add FIRST(Y2), and so on.
                else if(body_head == token){
                    continue;
                }
                else{
                    for(auto& v:p.body){
                        auto t = Find_token(v);
                        if(!t->has_first){
                            Get_first(v);
                        }
                        set<string>& t_first = first[v];
                        for(auto& s:t_first){
                            ft.second.insert(s);
                        }
                        if(!derive_eps(v))
                            break;
                    }
                }
            }
        }
    }

    //  insert ft into first sets 
    first.insert(ft);
    
    //  set token's first flag to true
    const_cast<Token&>(*tt).has_first = true;
//    Token new_t = *tt;
//    new_t.has_first = true;
//    tokens.erase(tt);
//    tokens.insert(new_t);
}

/***************************************************************
 *  func purpose:
 *      1.  get all nonterm's follow set
 **************************************************************/
void Grammar::Get_follow(){
    //  first set each follow set of empty, two copies for compare purpose
    vector<set<string> > old_follow(nonterms.size());
    vector<set<string> > new_follow(nonterms.size());

    // set follow(S) to {$}
    new_follow[0].insert("$");

    while(1){
        for(auto& p:pdcs){
            set<string> trailer = new_follow[nonterm_map[p.head]];
            for(int i = p.body.size()-1;i >= 0;i--){
                string beta = p.body[i];
                if(nonterms.count(beta)){
                    for(auto& t:trailer){
                        new_follow[nonterm_map[beta]].insert(t);
                    }

                    if(InSet(EPSILON, first[beta])){
                        for(auto& f:first[beta]){
                            if(f!=EPSILON)
                                trailer.insert(f); 
                        }
                    }
                    else{
                        trailer = first[beta];
                    }
                }
                else{
                    trailer = first[beta];
                }
            }
        }

        // if FOLLOW sets are not changing, finish compute follow sets
        // else copy the new to the old 
        bool stop_flag = true;
        for(auto& n:nonterms){
            if(old_follow[nonterm_map[n]] != new_follow[nonterm_map[n]]){
                stop_flag = false;
                old_follow[nonterm_map[n]] = new_follow[nonterm_map[n]];
            }
        }

        if(stop_flag)
            break;
    }
    
    //  insert all follow sets into member follow
    int count = 0;
    for(auto & n:nonterms){
        pair<string, set<string> > p;
        p.first = n;
        p.second = new_follow[count];

        follow.insert(p);
        count++;
    }

}

//  !!!the mistaken version of Get_follow !!!
/*
void Grammar::Get_follow(string nonterm){
    //if the nonterm has already get follow set, return immediately
    auto nn = Find_token(nonterm);
    if(nn->has_follow)
        return;

    follow_flag[nonterm] = true;

    //  define single follow set 
    pair<string, set<string> > fw;
    fw.first = nonterm;

    // Place $ in FOLLOW(S), where S is the start symbol, 
    // and $ is the input right endmarker. 
    if(nonterm == start){
        fw.second.insert("$");
    }
    else{
        for(auto& p:pdcs){
            if(Inbody(nonterm, p)){
                int pos = 0;
                while(pos<p.body.size()){
                    if(p.body[pos] != nonterm){
                        pos++;
                        continue;
                    }

                    if(pos+1<p.body.size()){
                        //   If there is a production A -> aBb, 
                        //   then everything in FIRST(b) except eps is in FOLLOW(B).
                        string next = p.body[pos+1];
                        for(auto& f:first[next]){
                            if(f != EPSILON)
                                fw.second.insert(f);
                        }
                        //   If there is a production A->aBb,
                        //   where FIRST(b) contains eps, 
                        //   then everything in FOLLOW(A) is in FOLLOW(B).
                        if(InSet(EPSILON, first[next]) && p.head!=nonterm && !follow_flag[p.head]){
                            auto t = Find_token(p.head);
                            if(!t->has_follow){
                                Get_follow(p.head);
                            }
                            
                            for(auto& f:follow[p.head]){
                                fw.second.insert(f);
                            }
                        }
                    }
                    //   If there is a production A->aB,
                    //   then everything in FOLLOW(A) is in FOLLOW(B).
                    //   take care of getfollow's 
                    else if(pos+1==p.body.size() && p.head!=nonterm && !follow_flag[p.head]){
                        auto t = Find_token(p.head);
                        if(!t->has_follow){
                            Get_follow(p.head);
                        }
                        
                        for(auto& f:follow[p.head]){
                            fw.second.insert(f);
                        }
                    }

                    pos++;
                }
            }
        }
    }

    //  insert fw into follow sets 
    follow.insert(fw);
    
    //  set token's first flag to true
    Token new_n = *nn;
    new_n.has_follow = true;
    tokens.erase(nn);
    tokens.insert(new_n);
}
*/

/***************************************************************
 *  func purpose:
 *      1.  if token can derive to epsilon, return true,
 *          else return false
 **************************************************************/
bool Grammar::derive_eps(string token){
    if(token == EPSILON)
        return true;
    else if(token_derive_eps.count(token)){
        return token_derive_eps[token];
    }
    else{
        bool can_derive_eps=false;

        //  return true if we find the epsion production directly
        for(auto& p:pdcs){
            if(p.head == token && p.body[0]==EPSILON){
                can_derive_eps = true;
                break;
            } 
        }

        if(!can_derive_eps){
            //  recursively find eps
            for(auto& p:pdcs){
                if(p.head == token && p.body[0] != token){
                    if(derive_eps((p.body)[0])){
                        can_derive_eps = true;
                        break;
                    }
                }
            }
        }

        token_derive_eps.insert(pair<string,bool>(token, can_derive_eps));

        return can_derive_eps;
    }
}

/***************************************************************
 *  func purpose:
 *      1. find the iterator of 'token' in the tokens set 
 **************************************************************/
set<Token>::iterator Grammar::Find_token(string token){
    for(set<Token>::iterator token_iter = tokens.begin();token_iter != tokens.end();token_iter++){
        if(token_iter->token == token){
            return token_iter;
        }
    }
}

void Grammar::GenTransFiles(const char* rule_name){
    ofstream gotofile(string("source/")+rule_name + string(".goto"), ios::out | ios::binary);
    ofstream actionfile(string("source/") + rule_name+string(".act"), ios::out|ios::binary);
    
    //  generate goto file
    gotofile << CC.size() << endl;
    for(auto& t:terms){
        gotofile << t << " ";
    }
    for(auto& n:nonterms){
        gotofile << n << " ";
    }
    gotofile << endl;

    int g_cnt = 0;
    for(auto& g:goto_table){
        for(auto& t:terms){
            if(g[GetTermNo(t)]==-1){
                continue;
            }

            gotofile << t << " " << g_cnt << " " << g[GetTermNo(t)] << endl;  
        }

        for(auto& n:nonterms){
            if(g[GetNonTermNo(n)+terms.size()]==-1){
                continue;
            }

            gotofile << n << " " << g_cnt << " " << g[GetNonTermNo(n)+terms.size()] << endl;  
        }

        g_cnt++;
    }

    //  generate action file
    int a_cnt=0;
    actionfile << CC.size() << endl;
    for(auto& t:terms){
        actionfile << t << " ";
    }
    actionfile << "$" <<endl;

    for(auto a:action_table){

        for(auto& t:terms){
            if(a[GetTermNo(t)].type==ACT_ERR){
                continue;
            }
            else if(a[GetTermNo(t)].type==ACT_SHIFT){
                actionfile << t << " " << a_cnt << " s " << a[GetTermNo(t)].status_pdcno << endl;  
            }
            else if(a[GetTermNo(t)].type==ACT_REDUCE){
                actionfile << t << " " << a_cnt << " r " << a[GetTermNo(t)].status_pdcno << endl;  
            }
        }

        // put the $ acition 
        if(a[terms.size()].type==ACT_ERR){
            ;
        }
        else if(a[terms.size()].type==ACT_SHIFT){
            actionfile << "$" << " " << a_cnt << " s " << a[terms.size()].status_pdcno << endl;  
        }
        else if(a[terms.size()].type==ACT_REDUCE){
            actionfile << "$" << " " << a_cnt << " r " << a[terms.size()].status_pdcno << endl;  
        }
        else{
            actionfile << "$" << " " << a_cnt << " acc" <<endl;
        }

        a_cnt++;
        
    }

    gotofile.close();
    actionfile.close();
}

Grammar g;

//int main(int argc, char* argv[]){
//
//
//    string ifile = "my_rule.gr";
//    ifile+=".i";
//    freopen(ifile.c_str(), "w", stdout);
//
//    g.Init("my_rule.gr");
//
//    g.GenTransFiles("my_rule.gr");
//
//    return 0;
//}
