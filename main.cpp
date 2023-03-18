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
        cout << "�ļ���ȫ,��������action��" << endl;
        Grammar g;
        //string ifile = "source/my_rule.gr.i";
        string ifile = "source/my_rule.gr.i";
        //auto tmp = freopen(ifile.c_str(), "w", stdout);
        //g.Init("my_rule.gr");
        g.Init("my_rule.gr");
        g.GenTransFiles("my_rule.gr");
    }
    else {
        cout << "�ҵ�action��" << endl;
    }
    cout << "��ʼ����" << endl;
    LR_analyse l;//Ҫ����action.goto,�ķ���ȥload_LR_table�����������о����������
    lex_analyzer analyzer;
    analyzer.LexAnalyze("test.c");
    auto origin = analyzer.PutRes();
    analyzer.GetSyntaxInputBuffer();
    l.analyse_sentence(init_input(origin,analyzer.syntax_input_buf));//��ʼ��������������Ҫ�����ľ��ӣ������﷨��progamma_treeָ�룬���ӵ�˳���Ƿ��ģ���ͼ��ע��
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

