#include "Tables.h"
#include "Grammar.h"
#include "Error.h"

IdentifyInfo::IdentifyInfo(WordInfo* o, IdentifyType t, ParameterTable* p)
	: origin_id(o), type(t), paras(p)
{
	if (p != NULL)
		types = p->get_types();
	dimension = -1;
	property = IdentifyProperty::FUNC;
	to_low = o->get_string();
	transform(to_low.begin(), to_low.end(), to_low.begin(), ::tolower);
}

string IdentifyInfo::get_name_in_low()
{
	return to_low;
}

int IdentifyInfo::get_dimension()
{
	return dimension;
}

IdentifyType IdentifyInfo::get_type()
{
	return type;
}

IdentifyProperty IdentifyInfo::get_propetry()
{
	return property;
}

bool IdentifyInfo::check_func_para_num(ParameterValue* value)
{
	if (paras == NULL)
	{
		if (value->get_expressions().size() != 0)
		{
			return false;
		}
		return true;
	}
	if (types.size() != value->get_expressions().size())
		return false;
	return true;
}

bool IdentifyInfo::check_func_para_type(ParameterValue* value)
{
	if (paras == NULL)
		return true;	//checked num beforehand
	unsigned int i;
	for (i = 0; i < types.size(); i++)
	{
		if (types[i].get_type() == TypeEnum::CHARTK)
		{
			if (value->get_expressions()[i].check_type() != IdentifyType::CHAR)
				return false;
		}
		else
		{
			if (value->get_expressions()[i].check_type() != IdentifyType::INT)
				return false;
		}
	}
	return true;
}

void IdentifyBlock::add_paras(ParameterTable* paras)
{
	if (paras == NULL)
	{
		return;
	}
	if (paras->check_empty())
	{
		return;
	}
	vector<WordInfo> types = paras->get_types();
	vector<WordInfo> id_names = paras->get_ids();
	unsigned int i;
	for (i = 0; i < paras->get_count(); i++)
	{
		IdentifyInfo* this_info;
		if (types[i].get_type() == TypeEnum::INTTK)
		{
			 this_info =
				new IdentifyInfo(&(id_names[i]), IdentifyProperty::VAR,
					IdentifyType::INT, 0);
		}
		else
		{
			this_info =
				new IdentifyInfo(&(id_names[i]), IdentifyProperty::VAR,
					IdentifyType::CHAR, 0);
		}
		ids.push_back(this_info);
	}
}

bool IdentifyBlock::add_identify(IdentifyInfo* new_one)
{
	unsigned int i;
	for (i = 0; i < ids.size(); i++)
	{
		if (ids[i]->get_name_in_low() == new_one->get_name_in_low())
		{
			return false;
		}
	}
	ids.push_back(new_one);
	return true;
}

bool IdentifyBlock::have_identity(string name)
{
	string to_low = name;
	transform(to_low.begin(), to_low.end(), to_low.begin(), ::tolower);
	unsigned int i;
	for (i = 0; i < ids.size(); i++)
	{
		if (ids[i]->get_name_in_low() == to_low)
		{
			return true;
		}
	}
	return false;
}

bool IdentifyBlock::check_func_para_num(WordInfo* func_id, ParameterValue* values)
{
	string to_low = func_id->get_string();
	transform(to_low.begin(), to_low.end(), to_low.begin(), ::tolower);
	unsigned int i;
	for (i = 0; i < ids.size(); i++)
	{
		if (ids[i]->get_name_in_low() == to_low)
		{
			return ids[i]->check_func_para_num(values);
		}
	}
	// did not found the func, must have log Wei Ding Yi
	return true;
}

bool IdentifyBlock::check_func_para_type(WordInfo* func_id, ParameterValue* values)
{
	string to_low = func_id->get_string();;
	transform(to_low.begin(), to_low.end(), to_low.begin(), ::tolower);
	unsigned int i;
	for (i = 0; i < ids.size(); i++)
	{
		if (ids[i]->get_name_in_low() == to_low)
		{
			return ids[i]->check_func_para_type(values);
		}
	}
	// did not found the func, must have log Wei Ding Yi
	return true;
}

IdentifyProperty IdentifyBlock::get_property_by_name(string name)
{
	string to_low = name;
	transform(to_low.begin(), to_low.end(), to_low.begin(), ::tolower);
	unsigned int i;
	for (i = 0; i < ids.size(); i++)
	{
		if (ids[i]->get_name_in_low() == to_low)
		{
			return ids[i]->get_propetry();
		}
	}
	return IdentifyProperty::NONE;
}

IdentifyType IdentifyBlock::get_type_by_name(string name)
{
	string to_low = name;
	transform(to_low.begin(), to_low.end(), to_low.begin(), ::tolower);
	unsigned int i;
	for (i = 0; i < ids.size(); i++)
	{
		if (ids[i]->get_name_in_low() == to_low)
		{
			return ids[i]->get_type();
		}
	}
	return IdentifyType::NONE;
}

string IdentifyBlock::get_func_name()
{
	return func_name;
}

void IdentifyTable::init()
{
	func_count = 0;
	active_block = 0;
	IdentifyBlock* new_one = new IdentifyBlock("global", func_count);
	func_count++;
	blocks.clear();
	blocks.push_back(new_one);
}

bool IdentifyTable::add_func
	(WordInfo* id, IdentifyType ret_type, ParameterTable* paras)
{
	IdentifyInfo* this_func = new IdentifyInfo(id, ret_type, paras);
	IdentifyBlock* this_block =
		new IdentifyBlock(this_func->get_name_in_low(), func_count);
	blocks.push_back(this_block);
	active_block = func_count;
	printf("func name: %s, active_block: %d\n", id->get_string().c_str(), active_block);
	func_count++;
	this_block->add_paras(paras);
	if (!blocks[0]->add_identify(this_func))
	{
		printf("wrong func name: %s\n", this_func->get_name_in_low().c_str());
		return false;
	}
	return true;
}

bool IdentifyTable::add_const(WordInfo* id, WordInfo* type)
{
	IdentifyInfo* this_info;
	if (type->get_type() == TypeEnum::INTTK)
	{
		this_info = new IdentifyInfo
			(id, IdentifyProperty::CONST, IdentifyType::INT, 0);
	}
	else
	{
		this_info = new IdentifyInfo
			(id, IdentifyProperty::CONST, IdentifyType::CHAR, 0);
	}
	if (!blocks[active_block]->add_identify(this_info))
	{
		printf("Wrong const identify %s\n", this_info->get_name_in_low().c_str());
		return false;
	}
	return true;
}

bool IdentifyTable::add_var(WordInfo* id, WordInfo* type, int dimension)
{
	IdentifyInfo* this_info;
	if (type->get_type() == TypeEnum::INTTK)
	{
		this_info = new IdentifyInfo
		(id, IdentifyProperty::VAR, IdentifyType::INT, dimension);
	}
	else
	{
		this_info = new IdentifyInfo
		(id, IdentifyProperty::VAR, IdentifyType::CHAR, dimension);
	}
	if (!blocks[active_block]->add_identify(this_info))
	{
		printf("Wrong const identify %s\n", this_info->get_name_in_low().c_str());
		return false;
	}
	return true;
}

bool IdentifyTable::have_var_const(WordInfo* id)
{
	if (blocks[active_block]->have_identity(id->get_string()))
	{
		printf("%s, in block %d\n", id->get_string().c_str(), active_block);
		IdentifyProperty p = 
			blocks[active_block]->get_property_by_name(id->get_string());
		if (p == IdentifyProperty::CONST || p == IdentifyProperty::VAR)
			return true;
		else
		{
			printf("finding var/const name but found a function: %s\n", id->get_string().c_str());
			return true;
		}
	}
	else if (blocks[0]->have_identity(id->get_string()))
	{
		printf("%s, in block 0\n", id->get_string().c_str());
		IdentifyProperty p =
			blocks[0]->get_property_by_name(id->get_string());
		if (p == IdentifyProperty::CONST || p == IdentifyProperty::VAR)
			return true;
		else
		{
			printf("finding var/const name but found a function: %s\n", id->get_string().c_str());
			return true;
		}
	}
	else
	{
		return false;
	}
}

IdentifyType IdentifyTable::get_type_by_name(WordInfo* id)
{
	// use have_var_const to check first
	if (blocks[active_block]->have_identity(id->get_string()))
	{
		return blocks[active_block]->get_type_by_name(id->get_string());
	}
	else
	{
		return blocks[0]->get_type_by_name(id->get_string());
	}
}

IdentifyProperty IdentifyTable::get_property_by_name(WordInfo* id)
{
	// use have_var_const to check first
	if (blocks[active_block]->have_identity(id->get_string()))
	{
		return blocks[active_block]->get_property_by_name(id->get_string());
	}
	else
	{
		return blocks[0]->get_property_by_name(id->get_string());
	}
}

bool IdentifyTable::have_func(WordInfo* func_id)
{
	if (blocks[0]->have_identity(func_id->get_string()))
	{
		IdentifyProperty p =
			blocks[0]->get_property_by_name(func_id->get_string());
		if (p == IdentifyProperty::FUNC)
			return true;
		else
		{
			printf("finding func name but found a var/const: %s\n", func_id->get_string().c_str());
			return true;
		}
	}
	return false;
}

bool IdentifyTable::have_return(WordInfo* func_id)
{
	// use have_func to check first
	string to_low = func_id->get_string();
	transform(to_low.begin(), to_low.end(), to_low.begin(), ::tolower);
	IdentifyType return_type = blocks[0]->get_type_by_name(to_low);
	if (return_type == IdentifyType::VOID)
		return false;
	else
		return true;
}

bool IdentifyTable::check_func_para_num(WordInfo* func_id, ParameterValue* values)
{
	return blocks[0]->check_func_para_num(func_id, values);
}

bool IdentifyTable::check_func_para_type(WordInfo* func_id, ParameterValue* values)
{
	return blocks[0]->check_func_para_type(func_id, values);
}

IdentifyType IdentifyTable::get_return_type(WordInfo* func_id)
{
	return blocks[0]->get_type_by_name(func_id->get_string());
}

vector<IdentifyBlock* > IdentifyTable::blocks;
unsigned int IdentifyTable::active_block;
unsigned int IdentifyTable::func_count;
