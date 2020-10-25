#pragma once
//#include"WordInfo.h"
#include"Lexer.h"
#include"Tables.h"

//
class BuChang;
class ChangLiang;
class ConditionCase;
class ConditionDefault;
class ConditionTable;
class ConstDingYi;
class ConstShuoMing;
class Expression;
class Factor;
class FunctionDingYi;
class FunctionMain;
class Grammar;
class Item;
class ParameterTable;
class ParameterValue;
class Sentence;
class SentenceDiaoYong;
class SentenceFuHe;
class SentenceFuZhi;
class SentenceLie;
class SentencePrint;
class SentenceQingKuang;
class SentenceRead;
class SentenceReturn;
class SentenceTiaoJian;
class SentenceXunHuan;
class ShengMingHead;
class TiaoJian;
class VariableDingYi;
class VariableShuoMing;
class WuFuHaoShu;
class ZhengShu;
class ZiFuChuan;
//

// <字符串> ::= "{十进制编码为32,33,35-126的ASCII字符}"
class ZiFuChuan
{
private:
	WordInfo zi_fu_chuan;
	int word_pos = 0;
public:
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	string to_string();
};

// <无符号整数> ::= <数字>{<数字>}
class WuFuHaoShu
{
private:
	WordInfo wu_fu_hao_shu;
	int word_pos = 0;
public:
	WuFuHaoShu() {};
	WuFuHaoShu(WordInfo i) : wu_fu_hao_shu(i) {};
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	int get_number()
	{
		return stoi(wu_fu_hao_shu.get_string());
	}
	string to_string();
};

// <整数> ::= [+｜-]<无符号整数>
class ZhengShu
{
private:
	WordInfo sign;
	bool have_sign = false;
	WuFuHaoShu number;
	int word_pos = 0;
public:
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	string to_string();
};

// <常量定义> ::= int<标识符>＝<整数>{,<标识符>＝<整数>} | char<标识符>＝<字符>{,<标识符>＝<字符> }
class ConstDingYi
{
private:
	WordInfo type;
	int word_pos = 0;
	vector<WordInfo> ids;
	vector<WordInfo> puncs;
	vector<ZhengShu> ints;
	vector<WordInfo> chars;
public:
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	string to_string();
};

// <常量说明> ::= const<常量定义>;{const<常量定义>;}
class ConstShuoMing
{
private:
	int word_pos = 0;
	vector<WordInfo> consts;
	vector<ConstDingYi> ding_yi_s;
	vector<WordInfo> semicns;
public:
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	string to_string();
};

// <常量> ::= <整数>|<字符>
class ChangLiang
{
private:
	//0: none; 1: int; 2: char.
	int type = 0;		
	int word_pos = 0;
	WordInfo zi_fu;
	ZhengShu zheng_shu;
public:
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	IdentifyType get_type()
	{
		if (type == 1)
			return IdentifyType::INT;
		else
			return IdentifyType::CHAR;
	}
	string to_string();
};

// <变量定义> ::= <变量定义无初始化>|<变量定义及初始化>	
// <变量定义无初始化> ::=	<类型标识符>(<标识符> | <标识符>'['<无符号整数>']' 
//		| <标识符>'['<无符号整数>']''['<无符号整数>']'){,(<标识符>|<标识符>'['<无符号整数>']'
//		| <标识符>'['<无符号整数>']''['<无符号整数>']' )}
// <变量定义及初始化> ::=	<类型标识符><标识符>=<常量> 
//		| <类型标识符><标识符>'['<无符号整数>']'='{'<常量>{,<常量>}'}'
//		| <类型标识符><标识符>'['<无符号整数>']''['<无符号整数>']'='{''{'<常量>{,<常量>}'}'{, '{'<常量>{,<常量>}'}'}'}'
class VariableDingYi
{
private:
	//0: none; 1: without initialize; 2: with initialize
	int type = 0;		
	//0: int; 1: int[]; 2: int[][]
	int dimension = 0;	
	int word_pos = 0;
	vector<WordInfo> define;
	vector<WordInfo> initial;
	vector<WuFuHaoShu> zheng_shu_s;
	vector<ChangLiang> chang_liang_s;
	void add_to_table();
	bool check_initial();
public:
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	string to_string();
};

// <变量说明>  ::= <变量定义>;{<变量定义>;} 
class VariableShuoMing
{
private:
	int word_pos = 0;
	vector<WordInfo> semicns;
	vector<VariableDingYi> ding_yi_s;
public:
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	string to_string();
};

// <声明头部>   ::=  int<标识符> |char<标识符> 
class ShengMingHead
{
private:
	WordInfo type;
	WordInfo id;
	int word_pos = 0;
public:
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	WordInfo get_id()
	{
		return id;
	}
	string to_string();
};

// <因子> ::= <标识符> | <标识符>'['<表达式>']' | <标识符>'['<表达式>']''['<表达式>']'
// | '('<表达式>')' | <整数> | <字符> | <有返回值函数调用语句>
class Factor
{
private:
	int word_pos = 0;
	int type = 0;
	//1, <标识符> | <标识符>'['<表达式>']' | <标识符>'['<表达式>']''['<表达式>']' | '('<表达式>')' 
	vector<WordInfo> ids;
	vector<Expression> exps;
	//2, zheng shu
	ZhengShu zheng_shu;
	//3, zi fu
	WordInfo zi_fu;
	//4, diao yong
	SentenceDiaoYong* diao_yong = NULL;
public:
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	IdentifyType check_type();
	string to_string();
};

// <项> ::= <因子>{<乘法运算符><因子>} 
class Item
{
private:
	int word_pos = 0;
	vector<Factor> factors;
	vector<WordInfo> signs;
public:
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	IdentifyType check_type();
	string to_string();
};

// <表达式> ::= [+|-]<项>{<加法运算符><项>} 
class Expression
{
private:
	int word_pos = 0;
	bool have_pre_sign = false;
	vector<Item> items;
	vector<WordInfo> signs;
public:
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	IdentifyType check_type();
	string to_string();
};

// <参数表> ::= <类型标识符><标识符>{,<类型标识符><标识符>} | <空>
class ParameterTable
{
private:
	int word_pos = 0;
	int count = 0;
	bool is_empty = true;
	vector<WordInfo> items;
	vector<WordInfo> types;
	vector<WordInfo> ids;
public:
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	bool check_empty()
	{
		return is_empty;
	}
	vector<WordInfo> get_types()
	{
		return types;
	}
	vector<WordInfo> get_ids()
	{
		return ids;
	}
	int get_count()
	{
		return count;
	}
	string to_string();
};

// <复合语句> ::= [<常量说明>][<变量说明>]<语句列>
class SentenceFuHe
{
private:
	int word_pos = 0;
	//0: none; 1: const only; 2: variable only; 3: const and variable
	int type = 0;
	ConstShuoMing consts;
	VariableShuoMing vars;
	SentenceLie* yu_ju_lie = NULL;
public:
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	vector<SentenceReturn* > get_all_return();
	string to_string();
};

// <有返回值函数定义>  ::=  <声明头部>'('<参数表>')' '{'<复合语句>'}'
// <无返回值函数定义>  ::= void<标识符>'('<参数表>')''{'<复合语句>'}' 
class FunctionDingYi
{
private:
	int word_pos = 0;
	bool have_return = false;
	ShengMingHead head;
	WordInfo void_tk;
	WordInfo id;
	WordInfo lparent;
	WordInfo rparent;
	WordInfo lbrace;
	WordInfo rbrace;
	ParameterTable table;
	SentenceFuHe fu_he;
public:
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	string to_string();
};

// <主函数> ::= void main'('')' '{'<复合语句>'}'  
class FunctionMain
{
private:
	int word_pos = 0;
	WordInfo void_tk;
	WordInfo main_tk;
	WordInfo lparent;
	WordInfo rparent;
	WordInfo lbrace;
	WordInfo rbrace;
	SentenceFuHe fu_he;
public:
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	string to_string();
};

// <赋值语句> ::= <标识符>＝<表达式> | <标识符>'['<表达式>']'=<表达式> | <标识符>'['<表达式>']''['<表达式>']'=<表达式>
class SentenceFuZhi
{
private:
	int word_pos = 0;
	vector<WordInfo> items;
	vector<Expression> exps;
public:
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	vector<SentenceReturn* > get_all_return();
	string to_string();
};
 
// <条件> ::=  <表达式><关系运算符><表达式>
class TiaoJian
{
private:
	int word_pos = 0;
	Expression e_left;
	Expression e_right;
	WordInfo relation;
public:
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	string to_string();
};

// <条件语句>::= if '('<条件>')'<语句>[else<语句>]
class SentenceTiaoJian
{
private:
	int word_pos = 0;
	bool have_else = false;
	//0:if	1:(	2:)	3:else
	vector<WordInfo> items;
	TiaoJian tiao_jian;
	Sentence* s_if = NULL;
	Sentence* s_else = NULL;
public:
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	vector<SentenceReturn* > get_all_return();
	string to_string();
};

// <步长>::= <无符号整数>  
class BuChang
{
private:
	int word_pos = 0;
	WuFuHaoShu zheng_shu;
public:
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	string to_string();
};

// <循环语句> ::= while'('<条件>')'<语句> | for'('<标识符>＝<表达式>;<条件>;<标识符>＝<标识符>(+|-)<步长>')'<语句>
class SentenceXunHuan
{
private:
	int word_pos = 0;
	//0: none; 1: while; 2: for.
	int type = 0;
	Sentence* sentence = NULL;
	TiaoJian tiao_jian;
	/*
	* while:
	* 0-while	1-(	2-)
	* for:
	* 0-for		1-(			2-<标识符>	3-=		4-;		5-;		6-<标识符>
	* 7-=		8-<标识符>	9-(+|-)		10-)
	*/
	vector<WordInfo> items;
	Expression expression;
	BuChang bu_chang;
public:
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	vector<SentenceReturn* > get_all_return();
	string to_string();
};

// <情况子语句> ::= case<常量>:<语句>
class ConditionCase
{
private:
	int word_pos = 0;
	WordInfo case_tk;
	WordInfo colon;
	ChangLiang chang_liang;
	Sentence* sentence = NULL;
public:
	int read_in(Lexer& lexer, IdentifyType target);
	int get_word_pos()
	{
		return word_pos;
	}
	vector<SentenceReturn* > get_all_return();
	string to_string();
};

// <缺省> ::= default:<语句>
class ConditionDefault
{
private:
	int word_pos = 0;
	WordInfo default_tk;
	WordInfo colon;
	Sentence* sentence = NULL;
public:
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	vector<SentenceReturn* > get_all_return();
	string to_string();
};

// <情况表> ::= <情况子语句>{<情况子语句>}
class ConditionTable
{
private:
	int word_pos = 0;
	vector<ConditionCase> cases;
public:
	int read_in(Lexer& lexer, IdentifyType target);
	int get_word_pos()
	{
		return word_pos;
	}
	vector<SentenceReturn* > get_all_return();
	string to_string();
};

// <情况语句> ::= switch'('<表达式>')''{'<情况表><缺省>'}' 
class SentenceQingKuang
{
private:
	int word_pos = 0;
	vector<WordInfo> items;
	Expression expression;
	ConditionTable table;
	ConditionDefault def;
public:
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	vector<SentenceReturn* > get_all_return();
	string to_string();
};

// <值参数表> ::= <表达式>{,<表达式>}｜<空>
class ParameterValue
{
private:
	int word_pos = 0;
	vector<Expression> expressions;
	vector<WordInfo> commas;
public:
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	vector<Expression> get_expressions()
	{
		return expressions;
	}
	string to_string();
};

// <有返回值函数调用语句> ::= <标识符>'('<值参数表>')' 
// <无返回值函数调用语句> ::= <标识符>'('<值参数表>')' 
class SentenceDiaoYong
{
private:
	int word_pos = 0;
	//0: empty; 1: have return; 2: none return
	int type = 0;
	vector<WordInfo> items;
	ParameterValue paras;
public:
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	WordInfo* get_func_id()
	{
		return &(items[0]);
	}
	string to_string();
};

// <写语句> ::= printf'('<字符串>,<表达式>')' | printf'('<字符串>')' | printf'('<表达式>')' 
class SentencePrint
{
private:
	//0: none; 1: string and expression; 2: string only; 3: expression only
	int type = 0;
	int word_pos = 0;
	vector<WordInfo> items;
	ZiFuChuan zi_fu;
	Expression expression;
public:
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	vector<SentenceReturn* > get_all_return();
	string to_string();
};

// <返回语句> ::=  return['('<表达式>')']
class SentenceReturn
{
private:
	int word_pos = 0;
	//0: none; 1: have return; 2: none return.
	int type = 0;
	vector<WordInfo> items;
	Expression expression;
public:
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	int get_word_line()
	{
		return items[0].get_line();
	}
	IdentifyType get_return_type()
	{
		if (type == 0)
			return IdentifyType::NONE;
		if (type == 2)
			return IdentifyType::VOID;
		else
		{
			return expression.check_type();
		}
	}
	vector<SentenceReturn* > get_all_return();
	string to_string();
};

// <读语句> ::= scanf'('<标识符>')' 
class SentenceRead
{
private:
	int word_pos = 0;
	vector<WordInfo> items;
public:
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	vector<SentenceReturn* > get_all_return();
	string to_string();
};

// <语句> ::= <循环语句> | <条件语句> | <有返回值函数调用语句>; | <无返回值函数调用语句>; ｜ <赋值语句>; 
// ｜ <读语句>; ｜ <写语句>; ｜ <情况语句> ｜ <空>; | <返回语句>; | '{'<语句列>'}' 
class Sentence
{
private:
	int word_pos = 0;
	int type = 0;					//0, empty
	SentenceXunHuan xun_huan;		//1, xun huan
	SentenceTiaoJian tiao_jian;		//2, tiao jian
	SentenceDiaoYong diao_yong;		//3, with & without return, diao yong
	SentenceFuZhi fu_zhi;			//4, fu zhi
	SentenceRead read;				//5, read
	SentencePrint print;			//6, print
	SentenceReturn fan_hui;			//7, fan hui
	SentenceQingKuang qing_kuang;	//8, qing kuang(case)
	SentenceLie* yu_ju_lie = NULL;	//9, yu ju lie
	WordInfo semicn;				//store ";"
	WordInfo lbrace;				//store "{"
	WordInfo rbrace;				//store "}"
public:
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	vector<SentenceReturn* > get_all_return();
	string to_string();
};

// <语句列> ::= {<语句>}
class SentenceLie
{
private:
	int word_pos = 0;
	vector<Sentence> sentences;
public:
	int read_in(Lexer& lexer);
	int get_word_pos()
	{
		return word_pos;
	}
	vector<SentenceReturn* > get_all_return();
	string to_string();
};


//<程序> ::= [<常量说明>] [<变量说明>] {<有返回值函数定义>|<无返回值函数定义>} <主函数>
class Grammar
{
private:
	//0: none; 1: have const; 2: have var; 4: have func
	int type = 0;
	ConstShuoMing consts;
	VariableShuoMing vars;
	vector<FunctionDingYi> funcs;
	FunctionMain main_func;
public:
	int read_in(Lexer& lexer);
	void print_to_file(string out_file_name);
};
