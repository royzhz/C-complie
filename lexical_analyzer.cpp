#include "lexical_analyzer.h"
#include <fstream>

using namespace std;

// define line location 
int now_line = 1;
int now_col = 1;



static inline bool IsLetter(const char& ch)
{
	if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
		return true;
	else
		return false;
}

static inline bool IsUnderline(const char& ch)
{
	if(ch=='_')
		return true;
	else
		return false;
}

static inline bool IsDigit(const char& ch)
{
	if(ch >='0' && ch <='9')
		return true;
	else
		return false;
}

static inline bool IsSpace(const char& ch)
{
	if(ch=='\t'||ch==' '||ch=='\n'||ch=='\r')
		return true;
	else
		return false;
}

static inline bool IsDelim(const char& ch)
{
	string key;
	key.push_back(ch);
	if(dl_kv.count(key))
		return true;
	else
		return false;
}

static inline bool IsSingleOpt(const char& ch)
{
	string key;
	key.push_back(ch);
	if(op_kv.count(key))
		return true;
	else
		return false;
}

static inline bool IsMultiOpt(const string& str)
{
	if(op_kv.count(str))
		return true;
	else
		return false;
}

static inline bool IsKeyword(const string& str)
{
	if(kw_kv.count(str))
		return true;
	else
		return false;
}

int lex_analyzer::ReadBuf(const std::string& filename)
{
	// check source file status
	if(!src_file.is_open())
	{
		cerr << "can't open file: " << filename << endl;
		return -1;
	}

	// read 10 lines from source file into scan buffer
	int line_cnt=0;
	string scan_buf[BUFLINES];	//	scan buffer

	while(!src_file.eof() && line_cnt<BUFLINES)
	{
		string tmp_line;
		getline(src_file,tmp_line);
		scan_buf[line_cnt] = tmp_line;
		scan_buf[line_cnt] += "\n";
		line_cnt++;
	}

	// pre-process the comment and put into the real buffer
	for(int i=0;i<line_cnt;i++)
	{
		for(int j=0;j<scan_buf[i].size();j++)
		{
			// check if encount the "//" comment
			char coms[3]={0};
			strncpy(coms,scan_buf[i].c_str()+j,2);
			if(!strcmp(coms,"//"))
				break;

			// put the scanned char into process buf
			process_buf.push_back(coms[0]);
		}
	}

	if(src_file.eof())
		return 1;	// finish read file
	else
		return 0;	// not finish read file
}

int lex_analyzer::open_src(const std::string& filename)
{
	src_file.open(filename, ios::in|ios::binary);
	if(!src_file.is_open())
	{
		cerr << "can't open file: " << filename << endl;
		return -1;		
	}

	return 0;
}

int lex_analyzer::LexAnalyze(const string& filename)
{
	//	open source file
	open_src(filename);

	// start lexical analyze
	int ret;
	while(1)
	{
		ret=ReadBuf(filename);
		if(ret == 0)
			GenToken();
		else if(ret==1)
		{
			GenToken();
			break;
		}
		else
			break;
	}

	src_file.close();
	return ret;
}

void lex_analyzer::GenToken()
{
	int str_len=process_buf.size();
	int str_cnt=0;

	while(1)
	{
		if(str_cnt>=str_len)
			break;

		int word_ptr=str_cnt;
		int push_flag=0;
		int token_value = 0 ; 
		string token_str;

		while(1)
		{
			char now_char=process_buf[word_ptr];

			if(IsSpace(now_char))
			{
				if(now_char == '\n')
				{
					now_line++;
					now_col = 1;
				}
				else
				{
					now_col++;
				}

				word_ptr++;
				break;
			}
			else if(IsDelim(now_char))
			{
				token_str.push_back(now_char);
				token_value=dl_kv[token_str];
				word_ptr++;
				now_col++;
				break;
			}
			else if(IsSingleOpt(now_char))
			{
				token_str.push_back(now_char);

				// advance search
				if(word_ptr+1<str_len)
				{
					token_str.push_back(process_buf[word_ptr+1]);
					if(IsMultiOpt(token_str))
					{
						token_value=op_kv[token_str];
						word_ptr+=2;
						now_col+=2;
					}
					else
					{	// only single char operator
						token_str.pop_back();
						token_value=op_kv[token_str];
						word_ptr++;
						now_col++;
					}
				}

				break;
			}
			else if(IsLetter(now_char)||IsUnderline(now_char))
			{
				token_str.push_back(now_char);

				int word_cnt=1;
				while(word_ptr+word_cnt<str_len)
				{
					char next_char=process_buf[word_ptr+word_cnt];
					if(IsLetter(next_char)||IsUnderline(next_char)||IsDigit(next_char))
					{
						token_str.push_back(next_char);
						word_cnt++;
						continue;	
					}
					else
						break;
				}

				if(IsKeyword(token_str))
					token_value=kw_kv[token_str];
				else
					token_value=TYPE_ID;

				word_ptr+=word_cnt;
				now_col+=word_cnt;
				break;
			}
			else if(IsDigit(now_char))
			{
				token_str.push_back(now_char);
				token_value=TYPE_IN;

				int word_cnt=1;
				while(word_ptr+word_cnt<str_len)
				{
					char next_char=process_buf[word_ptr+word_cnt];
					if(IsDigit(next_char))
					{
						token_str.push_back(next_char);
						word_cnt++;
						continue;
					}
					else if(next_char=='.')
					{
						token_value=TYPE_FL;
						token_str.push_back(next_char);
						word_cnt++;
						continue;
					}
					else if(IsLetter(next_char)||IsUnderline(next_char))
					{
					//cerr<<"line:"<< setw(4) << now_line <<"  col:"<< setw(2) << now_col<< "  lexcial error: bad source file!"<<endl;
						token_value=TYPE_ER;
						break;
					}
					else
						break;
				}

				word_ptr+=word_cnt;
				now_col+=word_cnt;
				break;
			}
			else if(now_char=='\'')
			{
				token_str.push_back(now_char);

				if(word_ptr+2>=str_len)
				{
					//cerr<<"line:"<< setw(4) << now_line <<"  col:"<< setw(2) << now_col<< "  lexcial error: bad source file!"<<endl;
					token_value=TYPE_ER;
					word_ptr++;
					now_col++;
				}
				else
				{
					char next_char=process_buf[word_ptr+1];
					char nnext_char=process_buf[word_ptr+2];
					if(nnext_char!='\'')
					{
					//cerr<<"line:"<< setw(4) << now_line <<"  col:"<< setw(2) << now_col<< "  lexcial error: bad source file!"<<endl;
						token_value=TYPE_ER;
						word_ptr++;
						now_col++;
					}
					else
					{
						token_value = TYPE_CH;
						token_str.push_back(process_buf[word_ptr+1]);
						token_str.push_back(process_buf[word_ptr+2]);
						word_ptr+=3;
						now_col+=3;
					}
				}

				break;
			}
			else if(now_char=='\"')
			{
				token_str.push_back(now_char);

				int word_cnt=1;
				while(word_ptr+word_cnt<str_len)
				{
					char next_char=process_buf[word_ptr+word_cnt];
					token_str.push_back(next_char);
					word_cnt++;
					if(next_char=='\"')
						break;
				}

				if((word_cnt>1)&&(process_buf[word_ptr+word_cnt-1]=='\"'))
				{
					token_value=TYPE_ST;
					word_ptr+=word_cnt;
					now_col+=word_cnt;
				}
				else
				{
					//cerr<<"line:"<< setw(4) << now_line <<"  col:"<< setw(2) << now_col<< "  lexcial error: bad source file!"<<endl;
					token_value=TYPE_ER;
					token_str="\"";
					word_ptr++;
					now_col++;
				}

				break;
			}
			else
			{
					//cerr<<"line:"<< setw(4) << now_line <<"  col:"<< setw(2) << now_col<< "  lexcial error: bad source file!"<<endl;

				word_ptr++;
				now_col++;
				break;
			}
		}

		str_cnt=word_ptr;
		if(token_str.size()!=0)
			lex_res.push_back(La_Token{token_value/100,token_value,token_str});
	}

	/* clean buf */
	this->process_buf="";
}

//lex_analyzer analyzer;
//
//int main(int argc, char* argv[]) 
//{
//	
////	ifstream src_file(argv[1], ios::in|ios::binary);
////	if (!src_file.is_open())
////	{
////		cout << "can't open source file" << endl;
////		return -1;
////	}
//
//	analyzer.LexAnalyze(argv[1]);
//	analyzer.PutRes();
//    analyzer.GetSyntaxInputBuffer();
//    analyzer.PutSyntaxInputBuffer();
//
////    src_file.close();
//
//	return 0;
//}
