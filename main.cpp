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
        cout << "找不到action表" << endl;
        return 0;
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
}
