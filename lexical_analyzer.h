#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <map>
#include <string.h>

enum WORD_TYPE{
	TYPE_KY = 0,	//	keyword
	TYPE_OP = 100,	//	operator
	TYPE_DL = 200, 	//	delimiter
	TYPE_ID = 300,	//	identifier
	TYPE_IN = 400,	//	integer
	TYPE_FL = 500,	//	fractional number
	TYPE_CH = 600,	//	character
	TYPE_ST = 700,	//	string
	TYPE_SP = 800,	// 	space
	TYPE_ER = 900 	//  error type
};

const std::string word_type[] = {
	"keyword",
	"operator",
	"delimiter",
	"identifier",
	"integer",
	"frac_num",
	"char",
	"string",
	"space",
	"error"
};

/* 定义单词（关键字、运算符、界符）字面值 */
static std::map<std::string, int> kw_kv = {
	/* keyword */
	{"begin",	1},
	{"if",		2},
	{"then",	3},
	{"while",	4},
	{"do",		5},
	{"end",		6},
	//{"main",	7},
	{"return",	8},
	{"int",		9},
	{"char",	10},
	{"float",	11},
	{"double",	12},
	{"case",	13},
	{"for",		14},
	{"auto",	15},
	{"else",	16},
	{"void",	17},
	{"static",	18},
	{"break",	19},
	{"struct",	20},
	{"const",	21},
	{"union",	22},
	{"switch",	23},
	{"typedef",	24},
	{"enum",	25},
	{"to",	    26},
	{"class",   27},
	{"and",     28},
	{"or",      29},
	{"not",     30}
};

static std::map<std::string, int>  op_kv = {
	/* operator */
	{"+",	101},
	{"-",	102},
	{"*",	103},
	{"/", 	104},
	{"%",	105},
	{"++",	106},
	{"--",	107},
	{"=",	108},
	{"+=",	109},
	{"-=",	110},
	{"*=",	111},
	{"/=",	112},
	{"|",	113},
	{"^",	114},
	{"&",	115},
	{"!",	116},
	{"==",	117},
	{"!=",	118},
	{"<=",	119},
	{">=",	120},
	{"<",	122},
	{">",	123},
	{"&&",	124},
	{"||",	125}
};

/* delimiter */
static std::map<std::string, int> dl_kv = {
	{"(",	201},
	{")",	202},
	{"{",	203},
	{"}",	204},
	{"[",	205},
	{"]",	206},
	{";",	207},
	{":",	208},
	{",",	209},
	{"?",	210},
	{"\\",	211}
};

// 词类结构
struct La_Token
{
	int	token_type;			//  单词种类 
	int token_value; 		//	单词类值
	std::string token_str; 	//  单词字面值

	friend std::ostream& operator<<(std::ostream& out, const La_Token& t)
	{
		out << std::left << std::setw(20) << word_type[t.token_type] << std::setw(10) << t.token_value << std::setw(20) << t.token_str << std::endl; 

		return out;
	}
};

//	词法分析器
class lex_analyzer
{
public:
    std::vector<std::string> syntax_input_buf;

public:
	int LexAnalyze(const std::string& filename);					//	execute lexical analysis
	int open_src(const std::string& filename);

	std::vector<std::string> PutRes()
	{
	    //std::cout << std::left << std::setfill(' ') << std::setw(17) << "token_type" << "token_value  "<<"token_str" << std::endl;
        std::vector<std::string> res;
		for(int i=0;i<lex_res.size();i++)
            res.push_back(lex_res[i].token_str) ;
        return res;
	}

    void PutSyntaxInputBuffer(){
//        for(int i=0;i<syntax_input_buf.size();i++){
//            std::cout << syntax_input_buf[i] << "  ";
//        }
    }

    void GetSyntaxInputBuffer(){
        for(int i=0;i<lex_res.size();i++){
            int t = lex_res[i].token_type*100; 
            if(t == TYPE_KY || t==TYPE_OP||t==TYPE_DL){
                syntax_input_buf.push_back(lex_res[i].token_str);
            }
            else if(t==TYPE_ID){
                syntax_input_buf.push_back("id");
            }
            else if(t==TYPE_IN){
                syntax_input_buf.push_back("const_int");
            }
            else if(t==TYPE_FL){
                syntax_input_buf.push_back("const_float");
            }
            else if(t==TYPE_CH){
                syntax_input_buf.push_back("const_char");
            }
            else if(t==TYPE_FL){
                syntax_input_buf.push_back("const_string");
            }
        }
    }

private:
	enum{
		BUFLINES = 10
	};
	std::ifstream src_file;			//	compiler read file
	std::string process_buf;		//  preprocessed buf
	std::vector<La_Token> lex_res;		// 	lexical analyized result 
	
	void GenToken();				//	decompose string to token
	int ReadBuf(const std::string& filename);	//	read source file
};
