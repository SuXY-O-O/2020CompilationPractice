#pragma once
#include "MiddleTable.h"
#include "Grammar.h"

class MiddleCode;
class MiddleFunction
{
private :
	string name_in_low;
	vector<MiddleSentence> m_sentences;
	ConstTable* global_const;
	ConstTable* local_const;
	VarTable* global_var;
	VarTable* local_var;
	bool reg_need_save[32];
	Arg* now_arg[32];
	void read_yu_ju(vector<Sentence> sent, StringTable& strings);
	string mips_begin_func();
	string mips_end_func();
	int check_exit_arg(Arg* a, bool add_opt);
	void clear_now_arg();
	int get_empty_reg();
	unsigned int reg_count = 0;
	int get_chang_reg();
	void orignaize_one_reg(Arg* a, bool is_save, bool add_opt);
	vector<MiddleSentence> optimize_immediate_block(vector<MiddleSentence> old);
public :
	MiddleFunction(string name, ConstTable* gconst, VarTable* gvar);
	string get_name();
	void pre_orgnaize_reg(vector<MiddleSentence> m_sent, bool add_opt);
	void add_func_parameter(ParameterTable table);
	void read_in_sentences(SentenceFuHe sent, StringTable& strings);
	string to_mips(StringTable* strings, MiddleCode* all_code, bool add_opt);
	int get_para_size();
	void optimize_immediate_all();
};

class MiddleCode
{
private:
	ConstTable* global_const;
	VarTable* global_var;
	StringTable* all_string;
	vector<MiddleFunction*> functions;
	vector<MiddleSentence> m_sentences;
	string mips_goto_func(string func_name);
	string mips_exit_func();
public:
	MiddleCode()
	{
		global_const = new ConstTable();
		global_var = new VarTable();
		all_string = new StringTable();
		functions.clear();
		m_sentences.clear();
	}
	void read_in(Grammar grammar);
	void print_mips_to_file(string file_name, bool add_opt);
	int get_para_size_by_name(string name);
};

namespace jump_label
{
	static unsigned int for_count = 0;
	static unsigned int while_count = 0;
	static unsigned int if_count = 0;
	static unsigned int switch_count = 0;

	string get_for_label();
	string get_while_label();
	string get_if_label();
	string get_switch_label();

	// Case Sensitive Implementation of starts_with()
	static bool starts_With(const string& src, const string& target); 
	static std::string for_label = "for";
	static std::string while_label = "while";

	static int log2(int value);
}