#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "LR_analyse.h"
#include "lexical_analyzer.h"
#include "syntactic_analyzer.h"
#include "generator.h"
bool isFileExists_fopen(string name) {
    if (FILE* file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    }
    else {
        return false;
    }
}

int main() {

    if (!isFileExists_fopen("source/my_rule.gr.goto") || !isFileExists_fopen("source/my_rule.gr.act") || !isFileExists_fopen("source/my_rule.gr.i"))
    {
        cout << "文件不全,重新生成action表" << endl;
        Grammar g;
        //string ifile = "source/my_rule.gr.i";
        string ifile = "source/my_rule.gr.i";
        //auto tmp = freopen(ifile.c_str(), "w", stdout);
        //g.Init("my_rule.gr");
        g.Init("my_rule.gr");
        g.GenTransFiles("my_rule.gr");
    }
    else {
        cout << "找到action表" << endl;
    }
    cout << "开始分析" << endl;
    LR_analyse l;//要更换action.goto,文法，去load_LR_table（）函数，有具体操作步骤
    lex_analyzer analyzer;
    analyzer.LexAnalyze("test.c");
    auto origin = analyzer.PutRes();
    analyzer.GetSyntaxInputBuffer();
    l.analyse_sentence(init_input(origin,analyzer.syntax_input_buf));//开始分析，参数就是要分析的句子，返回语法树progamma_tree指针，儿子的顺序是反的，画图请注意
    register_manager regm(l.get_four_tuple(), l.get_function_table());
    regm.generate_active_table();

    generator a(l.get_four_tuple(), l.get_function_table(), regm);
    a.generate_assble_code();
    a.show();
    return 0;
}

//int main(){
//    vector<four_tuple> four_tuple_list;
//    four_tuple_list.push_back(four_tuple{"1","+","t3","t2","t1"});
//    four_tuple_list.push_back(four_tuple{"1","=","t5","t2","t4"});
//    four_tuple_list.push_back(four_tuple{"1","=","t4","t1","t6"});
//    four_tuple_list.push_back(four_tuple{"1","=","t4","t6","t7"});
//    generator a(&four_tuple_list,NULL);
//    a.generate_active_table();
//
//    return 0;
//}

