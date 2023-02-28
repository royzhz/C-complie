#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "LR_analyse.h"
#include "lexical_analyzer.h"

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
        cout << "�Ҳ���action��" << endl;
        return 0;
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
}
