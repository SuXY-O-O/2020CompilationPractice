#pragma once
#include <string>
#include <vector>
using namespace std;

enum class ArgType
{
	NONE,
	INT,
	CHAR,
	IDENTIFY
};

enum class Operation
{
	ADD,
	SUB,
	MULT,
	DIV,
	NEG,
	ASSIGN,
	P_STR,
	P_CHAR,
	P_INT,
	S_INT,
	S_CHAR
};

class Arg
{
private :
	ArgType type;
	int value_int;
	char value_char;
	string identify;
	int d1;
	int d2;
	int target_reg = 0;
	bool need_stack = true;
public :
	Arg(ArgType t, int value) : type(t), value_int(value)
	{
		value_char = 0;
		d1 = d2 = 0;
	};
	Arg(ArgType t, char value) : type(t), value_char(value)
	{
		value_int = 0;
		d1 = d2 = 0;
	};
	Arg(ArgType t, string id, int _d1, int _d2) : 
		type(t), identify(id), d1(_d1), d2(_d2)
	{
		value_char = 0;
		value_int = 0;
	};
	Arg()
	{
		type = ArgType::NONE;
		value_int = d1 = d2 = 0;
		value_char = 0;
	};
	ArgType get_type()
	{
		return type;
	};
	int get_value()
	{
		if (type == ArgType::INT)
			return value_int;
		if (type == ArgType::CHAR)
			return (int)value_char;
		return 0;
	}
	int get_reg()
	{
		return target_reg;
	}
	string get_id()
	{
		return identify;
	}
	string to_string();
	void set_target_reg(int reg, bool n_s)
	{
		target_reg = reg;
		need_stack = n_s;
	}
};

class VarInfo
{
public:
	int type = 0; //0:int, 1:char
	string name_in_low = "\0";
	int dimenation = 0;
	int d1 = 0;
	int d2 = 0;
	bool have_initial = false;
	int size_in_byte = 0;
	int real_offset = 0;
	bool is_func_arg = false;
};

class ConstInfo
{
public:
	int type = 0; //0:int, 1:char
	string name_in_low = "\0";
	int value_int = 0;
	char value_char = '\0';
	int get_value()
	{
		if (type == 0)
			return value_int;
		else
			return (int)value_char;
	}
};

class StringInfo
{
public:
	string name_in_low;
	string value;
};

class ConstTable
{
private:
	vector<ConstInfo*> consts;
public:
	ConstTable()
	{
		consts.clear();
	}
	void add_in(ConstInfo* info);
	bool have_name(string name);
	ConstInfo* get_info_by_name(string name);
};

class VarTable
{
private:
	vector<VarInfo*> vars;
	int tmp_count = 0;
	int total_stack_size = 0;
public:
	VarTable()
	{
		vars.clear();
	}
	string get_new_tmp();
	bool have_name(string name);
	void add_in(VarInfo* info);
	VarInfo* get_info_by_name(string name);
	string to_mips();
	void count_arg_stack();
	int get_stack_size()
	{
		return total_stack_size;
	}
};

class StringTable
{
private:
	vector<StringInfo* > strings;
	int count = 0;
public:
	string get_label();
	void add_in(StringInfo* info);
	string to_mips();
};

class MiddleSentence
{
private:
	Operation op;
	Arg arg1;
	Arg arg2;
	Arg out;
	string load_arg_to_reg
		(Arg arg, ConstTable* l_const, ConstTable* g_const, 
		VarTable* l_var, VarTable* g_var);
	string save_to_stack(Arg arg, VarTable* l_var, VarTable* g_var);
public:
	MiddleSentence(Operation o, Arg* a1, Arg* a2, Arg* a3) :
		op(o)
	{
		arg1 = *a1;
		arg2 = *a2;
		out = *a3;
	};
	string to_string();
	string to_mips(ConstTable* l_const, ConstTable* g_const, 
		VarTable* l_var, VarTable* g_var, StringTable* strings);
	Arg* get_arg1()
	{
		return &arg1;
	}
	Arg* get_arg2()
	{
		return &arg2;
	}
	Arg* get_arg_out()
	{
		return &out;
	}
	Operation get_operation()
	{
		return op;
	}
};

string op_to_str(Operation op);