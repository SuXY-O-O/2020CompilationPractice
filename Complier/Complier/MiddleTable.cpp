#include "MiddleTable.h"
#include "Mips.h"

void ConstTable::add_in(ConstInfo* info)
{
	consts.push_back(info);
}

bool ConstTable::have_name(string name)
{
	unsigned int i;
	for (i = 0; i < consts.size(); i++)
	{
		if (consts[i]->name_in_low == name)
			return true;
	}
	return false;
}

ConstInfo* ConstTable::get_info_by_name(string name)
{
	unsigned int i;
	for (i = 0; i < consts.size(); i++)
	{
		if (consts[i]->name_in_low == name)
			return consts[i];
	}
}

string VarTable::get_new_tmp()
{
	while (1)
	{
		tmp_count++;
		string name = "_tmp_" + std::to_string(tmp_count);
		if (have_name(name))
			continue;
		else
			return name;
	}
}

bool VarTable::have_name(string name)
{
	unsigned int i;
	for (i = 0; i < vars.size(); i++)
	{
		if (vars[i]->name_in_low == name)
			return true;
	}
	return false;
}

void VarTable::add_in(VarInfo* info)
{
	vars.push_back(info);
}

VarInfo* VarTable::get_info_by_name(string name)
{
	unsigned int i;
	for (i = 0; i < vars.size(); i++)
	{
		if (vars[i]->name_in_low == name)
			return vars[i];
	}
}

string VarTable::to_mips()
{
	string for_return;
	for_return.clear();
	unsigned int i;
	for (i = 0; i < vars.size(); i++)
	{
		for_return += vars[i]->name_in_low;
		for_return += " : ";
		for_return += ".space ";
		for_return += std::to_string(vars[i]->size_in_byte);
		for_return += "\n";
	}
	return for_return;
}

void VarTable::count_arg_stack()
{
	unsigned int i;
	unsigned int local_offset = 0;
	//TODO: to check
	unsigned int para_offset = -8;
	for (i = 0; i < vars.size(); i++)
	{
		if (vars[i]->is_func_arg)
		{
			vars[i]->real_offset = para_offset;
			para_offset -= vars[i]->size_in_byte;
		}
		else
		{
			vars[i]->real_offset = local_offset;
			local_offset += vars[i]->size_in_byte;
		}
	}
	total_stack_size = local_offset;
}

string StringTable::get_label()
{
	count++;
	string s = "_str_";
	s += std::to_string(count);
	//printf("aaaaaaa %s\n", s.c_str());
	return s;
}

void StringTable::add_in(StringInfo* info)
{
	strings.push_back(info);
}

string StringTable::to_mips()
{
	string for_return;
	for_return.clear();
	unsigned int i;
	for (i = 0; i < strings.size(); i++)
	{
		for_return += strings[i]->name_in_low;
		for_return += ": .asciiz \"";
		for_return += strings[i]->value;
		for_return += "\"\n";
	}
	return for_return;
}

string MiddleSentence::load_arg_to_reg
	(Arg arg, ConstTable* l_const, ConstTable* g_const, 
	VarTable* l_var, VarTable* g_var)
{
	if (arg.get_type() == ArgType::NONE)
		return string();
	string for_return;
	for_return.clear();
	if (arg.get_type() == ArgType::INT || arg.get_type() == ArgType::CHAR)
	{
		for_return += Mips::li(arg.get_reg(), arg.get_value());
	}
	else if (l_const->have_name(arg.get_id()))
	{
		ConstInfo* info = l_const->get_info_by_name(arg.get_id());
		for_return += Mips::li(arg.get_reg(), info->get_value());
	}
	else if (g_const->have_name(arg.get_id()))
	{
		ConstInfo* info = g_const->get_info_by_name(arg.get_id());
		for_return += Mips::li(arg.get_reg(), info->get_value());
	}
	else if (g_var->have_name(arg.get_id()))
	{
		for_return += Mips::la(arg.get_reg(), arg.get_id());
		for_return += Mips::lw(arg.get_reg(), 0, arg.get_reg());
	}
	else if (l_var->have_name(arg.get_id()))
	{
		VarInfo* info = l_var->get_info_by_name(arg.get_id());
		if (info->is_func_arg)
			for_return += Mips::lw(arg.get_reg(), info->real_offset, 30);//fp
		else
			for_return += Mips::lw(arg.get_reg(), info->real_offset, 29);//sp
	}
	return for_return;
}

string MiddleSentence::save_to_stack(Arg arg, VarTable* l_var, VarTable* g_var)
{
	string for_return;
	for_return.clear();
	if (l_var->have_name(arg.get_id()))
	{
		VarInfo* info = l_var->get_info_by_name(arg.get_id());
		for_return += Mips::sw(arg.get_reg(), info->real_offset, 29);
	}
	else
	{
		for_return += Mips::sw(arg.get_reg(), arg.get_id());
	}
	return for_return;
}

string MiddleSentence::to_string()
{
	string s;
	s.clear();
	s += op_to_str(op);
	s += "\t";
	s += arg1.to_string();
	s += "\t";
	s += arg2.to_string();
	s += "\t";
	s += out.to_string();
	s += "\n";
	return s;
	//TODO
}

string MiddleSentence::to_mips
	(ConstTable* l_const, ConstTable* g_const, 
	VarTable* l_var, VarTable* g_var, StringTable* strings)
{
	string for_return;
	for_return.clear();
	switch (op)
	{
		case Operation::ADD:
		{
			for_return += load_arg_to_reg(arg1, l_const, g_const, l_var, g_var);
			for_return += load_arg_to_reg(arg2, l_const, g_const, l_var, g_var);
			for_return += Mips::add(out.get_reg(), arg1.get_reg(), arg2.get_reg());
			for_return += save_to_stack(out, l_var, g_var);
			break;
		}
		case Operation::SUB:
		{
			for_return += load_arg_to_reg(arg1, l_const, g_const, l_var, g_var);
			for_return += load_arg_to_reg(arg2, l_const, g_const, l_var, g_var);
			for_return += Mips::sub(out.get_reg(), arg1.get_reg(), arg2.get_reg());
			for_return += save_to_stack(out, l_var, g_var);
			break;
		}
		case Operation::MULT:
		{
			for_return += load_arg_to_reg(arg1, l_const, g_const, l_var, g_var);
			for_return += load_arg_to_reg(arg2, l_const, g_const, l_var, g_var);
			for_return += Mips::mul(out.get_reg(), arg1.get_reg(), arg2.get_reg());
			for_return += save_to_stack(out, l_var, g_var);
			break;
		}
		case Operation::DIV:
		{
			for_return += load_arg_to_reg(arg1, l_const, g_const, l_var, g_var);
			for_return += load_arg_to_reg(arg2, l_const, g_const, l_var, g_var);
			for_return += Mips::div(out.get_reg(), arg1.get_reg(), arg2.get_reg());
			for_return += save_to_stack(out, l_var, g_var);
			break;
		}
		case Operation::NEG:
		{
			for_return += load_arg_to_reg(arg1, l_const, g_const, l_var, g_var);
			for_return += Mips::neg(out.get_reg(), arg1.get_reg());
			for_return += save_to_stack(out, l_var, g_var);
			break;
		}
		case Operation::ASSIGN:
		{
			for_return += load_arg_to_reg(arg1, l_const, g_const, l_var, g_var);
			for_return += Mips::move(out.get_reg(), arg1.get_reg());
			for_return += save_to_stack(out, l_var, g_var);
			break;
		}
		case Operation::P_CHAR:
		{
			for_return += load_arg_to_reg(arg1, l_const, g_const, l_var, g_var);
			for_return += Mips::move(4, arg1.get_reg());
			for_return += Mips::syscall(11);
			break;
		}
		case Operation::P_INT:
		{
			for_return += load_arg_to_reg(arg1, l_const, g_const, l_var, g_var);
			for_return += Mips::move(4, arg1.get_reg());
			for_return += Mips::syscall(1);
			break;
		}
		case Operation::P_STR:
		{
			for_return += Mips::la(arg1.get_reg(), arg1.get_id());
			for_return += Mips::move(4, arg1.get_reg());
			for_return += Mips::syscall(4);
			break;
		}
		case Operation::S_INT:
		{
			for_return += Mips::syscall(5);
			for_return += Mips::move(out.get_reg(), 2);
			for_return += save_to_stack(out, l_var, g_var);
			break;
		}
		case Operation::S_CHAR:
		{
			for_return += Mips::syscall(12);
			for_return += Mips::move(out.get_reg(), 2);
			for_return += save_to_stack(out, l_var, g_var);
			break;
		}
	}
	return for_return;
}

string Arg::to_string()
{
	if (type == ArgType::INT)
		return std::to_string(value_int);
	if (type == ArgType::CHAR)
		return std::to_string((int)value_char) + "_c";
	if (type == ArgType::IDENTIFY)
		return identify;
	return "_/_";
}

string op_to_str(Operation op)
{
	switch (op)
	{
	case Operation::ADD:
		return "ADD";
	case Operation::SUB:
		return "SUB";
	case Operation::MULT:
		return "MULT";
	case Operation::DIV:
		return "DIV";
	case Operation::NEG:
		return "NEG";
	case Operation::P_CHAR:
		return "P_CHAR";
	case Operation::ASSIGN:
		return "ASSIGN";
	case Operation::P_INT:
		return "P_INT";
	case Operation::P_STR:
		return "P_STR";
	case Operation::S_INT:
		return "S_INT";
	case Operation::S_CHAR:
		return "S_CHAR";
	}
}
