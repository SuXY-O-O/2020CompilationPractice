#pragma once

#include "WordInfo.h"
#include <algorithm>
#include <map>
#include <vector>

class ParameterTable;
class ParameterValue;

inline char charToUpper(char c)
{
	return std::toupper(c);
}

enum class IdentifyType
{
	NONE,
	VOID,
	INT,
	CHAR
};

enum class IdentifyProperty
{
	NONE,
	CONST,	//const
	VAR,	//variable
	FUNC	//function
};

class IdentifyInfo
{
private:
	WordInfo* origin_id;
	string to_low;
	IdentifyProperty property;
	IdentifyType type;
	int dimension;
	ParameterTable* paras;
	vector<WordInfo> types;
	//int address;
public:
	IdentifyInfo(WordInfo* o, IdentifyProperty p, IdentifyType t, int d)
		: origin_id(o), property(p), type(t), dimension(d)
	{
		paras = NULL;
		to_low = o->get_string();
		transform(to_low.begin(), to_low.end(), to_low.begin(), ::tolower);
	}
	IdentifyInfo(WordInfo* o, IdentifyType t, ParameterTable* p);
	string get_name_in_low();
	int get_dimension();
	IdentifyType get_type();
	IdentifyProperty get_propetry();
	bool check_func_para_num(ParameterValue* value);
	bool check_func_para_type(ParameterValue* value);
};

class IdentifyBlock
{
private:
	vector<IdentifyInfo* > ids;
	string func_name;
	unsigned int func_id;
public:
	IdentifyBlock(string func, unsigned int id) : func_name(func), func_id(id)
	{
		ids.clear();
	};
	void add_paras(ParameterTable* paras);
	bool add_identify(IdentifyInfo* new_one);
	bool have_identity(string name);
	bool check_func_para_num(WordInfo* func_id, ParameterValue* values);
	bool check_func_para_type(WordInfo* func_id, ParameterValue* values);
	IdentifyProperty get_property_by_name(string name);
	IdentifyType get_type_by_name(string name);
	string get_func_name();
};

class IdentifyTable
{
private:
	static vector<IdentifyBlock* > blocks;
	static unsigned int active_block;
	static unsigned int func_count;
public:
	static void init();

	static bool add_func(WordInfo* id, IdentifyType ret_type, ParameterTable* paras);
	static bool add_const(WordInfo* id, WordInfo* type);
	static bool add_var(WordInfo* id, WordInfo* type, int dimension);

	static bool have_var_const(WordInfo* id);
	static IdentifyType get_type_by_name(WordInfo* id);
	static void set_active_block_by_name(string name);
	static IdentifyProperty get_property_by_name(WordInfo* id);

	static bool have_func(WordInfo* func_id);
	static bool have_return(WordInfo* func_id);
	static bool check_func_para_num(WordInfo* func_id, ParameterValue* values);
	static bool check_func_para_type(WordInfo* func_id, ParameterValue* values);
	static IdentifyType get_return_type(WordInfo* func_id);
};