#include "MiddleTable.h"
#include "MiddleCode.h"
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
	return nullptr;
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
	return NULL;
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
	unsigned int para_offset = 0;
	for (i = 0; i < vars.size(); i++)
	{
		if (vars[i]->is_func_arg)
		{
			vars[i]->real_offset = para_offset;
			para_offset += vars[i]->size_in_byte;
		}
		else
		{
			vars[i]->real_offset = local_offset;
			local_offset += vars[i]->size_in_byte;
		}
	}
	total_stack_size = local_offset;
	para_stack_size = para_offset;
}

string StringTable::get_label()
{
	count++;
	string s = "_str_";
	s += std::to_string(count);
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
	(Arg* arg, ConstTable* l_const, ConstTable* g_const, 
	VarTable* l_var, VarTable* g_var)
{
	if (arg->get_type() == ArgType::NONE || !arg->check_need_to_stack())
		return string();
	string for_return;
	for_return.clear();
	if (arg->get_type() == ArgType::INT || arg->get_type() == ArgType::CHAR)
	{
		for_return += Mips::li(arg->get_reg(), arg->get_value());
	}
	else if (arg->get_type() == ArgType::IDENTIFY)
	{
		if (l_const->have_name(arg->get_id()))
		{
			ConstInfo* info = l_const->get_info_by_name(arg->get_id());
			for_return += Mips::li(arg->get_reg(), info->get_value());
		}
		else if (l_var->have_name(arg->get_id()))
		{
			VarInfo* info = l_var->get_info_by_name(arg->get_id());
			if (info->is_func_arg)
				for_return += Mips::lw(arg->get_reg(), info->real_offset, 30);//fp
			else
				for_return += Mips::lw(arg->get_reg(), info->real_offset, 29);//sp
		}
		else if (g_var->have_name(arg->get_id()))
		{
			for_return += Mips::la(arg->get_reg(), arg->get_id());
			for_return += Mips::lw(arg->get_reg(), 0, arg->get_reg());
		}
		else if (g_const->have_name(arg->get_id()))
		{
			ConstInfo* info = g_const->get_info_by_name(arg->get_id());
			for_return += Mips::li(arg->get_reg(), info->get_value());
		}
	}
	else if (arg->get_type() == ArgType::ARRAY)// is from array
	{
		Arg* array_offset = arg->get_offset();
		if (array_offset->get_type() == ArgType::INT)
		{
			int of = array_offset->get_value();
			if (l_var->have_name(arg->get_id()))
			{
				VarInfo* info = l_var->get_info_by_name(arg->get_id());
				// should not be a func para
				for_return += Mips::lw(arg->get_reg(), of + info->real_offset, 29);//sp
			}
			else if (g_var->have_name(arg->get_id()))
			{
				for_return += Mips::la(arg->get_reg(), arg->get_id());
				for_return += Mips::lw(arg->get_reg(), of, arg->get_reg());
			}
		}
		else
		{
			for_return += load_arg_to_reg(array_offset, l_const, g_const, l_var, g_var);
			int reg_offset = array_offset->get_reg();
			if (l_var->have_name(arg->get_id()))
			{
				VarInfo* info = l_var->get_info_by_name(arg->get_id());
				// should not be a func para
				for_return += Mips::add(arg->get_reg(), 29, reg_offset);
				for_return += Mips::lw(arg->get_reg(), info->real_offset, arg->get_reg());//sp
			}
			else if (g_var->have_name(arg->get_id()))
			{
				for_return += Mips::la_l_r(arg->get_reg(), arg->get_id(), reg_offset);
				for_return += Mips::lw(arg->get_reg(), 0, arg->get_reg());
			}
		}
	}
	return for_return;
}

string MiddleSentence::save_to_stack
	(Arg* arg, ConstTable* l_const, ConstTable* g_const, 
	VarTable* l_var, VarTable* g_var)
{
	string for_return;
	for_return.clear();
	if (arg->get_type() == ArgType::NONE || !arg->check_need_to_stack())
		return for_return;
	bool from_local = true;
	VarInfo* info;
	if (l_var->have_name(arg->get_id()))
	{
		from_local = true;
		info = l_var->get_info_by_name(arg->get_id());
	}
	else
	{
		from_local = false;
		info = g_var->get_info_by_name(arg->get_id());
	}
	if (info->dimenation == 0)
	{
		if (from_local)
		{
			if (info->is_func_arg)
				for_return += Mips::sw(arg->get_reg(), info->real_offset, 30);
			else
				for_return += Mips::sw(arg->get_reg(), info->real_offset, 29);
		}	
		else
			for_return += Mips::sw(arg->get_reg(), arg->get_id());
	}
	else
	{
		//dimenation >= 1, cannot be a para for func
		for_return += load_arg_to_reg(arg->get_offset(), l_const, g_const, l_var, g_var);
		int reg_offset = arg->get_offset()->get_reg();
		if (from_local)
		{
			for_return += Mips::add(4, 29, reg_offset);
			for_return += Mips::sw(arg->get_reg(), info->real_offset, 4);//sp
		}
		else
		{
			for_return += Mips::la_l_r(4, arg->get_id(), reg_offset);
			for_return += Mips::sw(arg->get_reg(), 0, 4);
		}
	}
	return for_return;
}

string MiddleSentence::to_string()
{
	string s;
	s.clear();
	s += op_to_str(op);
	s += "\t";
	if (arg1 != NULL)
		s += arg1->to_string();
	else
		s += "_/_";
	s += "\t";
	if (arg2 != NULL)
		s += arg2->to_string();
	else
		s += "_/_";
	s += "\t";
	if (out != NULL)
		s += out->to_string();
	else
		s += "_/_";
	s += "\n";
	return s;
	//TODO
}

string MiddleSentence::to_mips
	(ConstTable* l_const, ConstTable* g_const, 
	VarTable* l_var, VarTable* g_var, StringTable* strings,
	MiddleCode* all_code, int& func_para_size)
{
	string for_return;
	for_return.clear();
	switch (op)
	{
		case Operation::ADD:
		{
			for_return += load_arg_to_reg(arg1, l_const, g_const, l_var, g_var);
			for_return += load_arg_to_reg(arg2, l_const, g_const, l_var, g_var);
			for_return += Mips::add(out->get_reg(), arg1->get_reg(), arg2->get_reg());
			for_return += save_to_stack(out, l_const, g_const, l_var, g_var);
			break;
		}
		case Operation::ADDI:
		{
			for_return += load_arg_to_reg(arg1, l_const, g_const, l_var, g_var);
			for_return += Mips::addi(out->get_reg(), arg1->get_reg(), arg2->get_value());
			for_return += save_to_stack(out, l_const, g_const, l_var, g_var);
			break;
		}
		case Operation::SUB:
		{
			for_return += load_arg_to_reg(arg1, l_const, g_const, l_var, g_var);
			for_return += load_arg_to_reg(arg2, l_const, g_const, l_var, g_var);
			for_return += Mips::sub(out->get_reg(), arg1->get_reg(), arg2->get_reg());
			for_return += save_to_stack(out, l_const, g_const, l_var, g_var);
			break;
		}
		case Operation::SUBI:
		{
			for_return += load_arg_to_reg(arg1, l_const, g_const, l_var, g_var);
			for_return += Mips::subi(out->get_reg(), arg1->get_reg(), arg2->get_value());
			for_return += save_to_stack(out, l_const, g_const, l_var, g_var);
			break;
		}
		case Operation::MULT:
		{
			for_return += load_arg_to_reg(arg1, l_const, g_const, l_var, g_var);
			for_return += load_arg_to_reg(arg2, l_const, g_const, l_var, g_var);
			for_return += Mips::mul(out->get_reg(), arg1->get_reg(), arg2->get_reg());
			for_return += save_to_stack(out, l_const, g_const, l_var, g_var);
			break;
		}
		case Operation::MULI:
		{
			for_return += load_arg_to_reg(arg1, l_const, g_const, l_var, g_var);
			for_return += Mips::muli(out->get_reg(), arg1->get_reg(), arg2->get_value());
			for_return += save_to_stack(out, l_const, g_const, l_var, g_var);
			break;
		}
		case Operation::DIV:
		{
			for_return += load_arg_to_reg(arg1, l_const, g_const, l_var, g_var);
			for_return += load_arg_to_reg(arg2, l_const, g_const, l_var, g_var);
			for_return += Mips::div(out->get_reg(), arg1->get_reg(), arg2->get_reg());
			for_return += save_to_stack(out, l_const, g_const, l_var, g_var);
			break;
		}
		case Operation::DIVI:
		{
			for_return += load_arg_to_reg(arg1, l_const, g_const, l_var, g_var);
			for_return += Mips::divi(out->get_reg(), arg1->get_reg(), arg2->get_value());
			for_return += save_to_stack(out, l_const, g_const, l_var, g_var);
			break;
		}
		case Operation::NEG:
		{
			for_return += load_arg_to_reg(arg1, l_const, g_const, l_var, g_var);
			for_return += Mips::neg(out->get_reg(), arg1->get_reg());
			for_return += save_to_stack(out, l_const, g_const, l_var, g_var);
			break;
		}
		case Operation::ASSIGN:
		{
			for_return += load_arg_to_reg(arg1, l_const, g_const, l_var, g_var);
			for_return += Mips::move(out->get_reg(), arg1->get_reg());
			for_return += save_to_stack(out, l_const, g_const, l_var, g_var);
			break;
		}
		case Operation::INIT:
		{
			for_return += Mips::li(out->get_reg(), arg1->get_value());
			for_return += save_to_stack(out, l_const, g_const, l_var, g_var);
			break;
		}
		case Operation::P_CHAR:
		{
			for_return += load_arg_to_reg(arg1, l_const, g_const, l_var, g_var);
			//for_return += Mips::move(4, arg1->get_reg());
			for_return += Mips::syscall(11);
			break;
		}
		case Operation::P_INT:
		{
			for_return += load_arg_to_reg(arg1, l_const, g_const, l_var, g_var);
			//for_return += Mips::move(4, arg1->get_reg());
			for_return += Mips::syscall(1);
			break;
		}
		case Operation::P_STR:
		{
			for_return += Mips::la(arg1->get_reg(), arg1->get_id());
			//for_return += Mips::move(4, arg1->get_reg());
			for_return += Mips::syscall(4);
			break;
		}
		case Operation::S_INT:
		{
			for_return += Mips::syscall(5);
			for_return += Mips::move(out->get_reg(), 2);
			for_return += save_to_stack(out, l_const, g_const, l_var, g_var);
			break;
		}
		case Operation::S_CHAR:
		{
			for_return += Mips::syscall(12);
			for_return += Mips::move(out->get_reg(), 2);
			for_return += save_to_stack(out, l_const, g_const, l_var, g_var);
			break;
		}
		case Operation::START_FUNC:
		{
			for_return += "# function diao yong begin\n";
			break;
		}
		case Operation::SAVE_PARA:
		{
			func_para_size += 4;
			for_return += load_arg_to_reg(arg1, l_const, g_const, l_var, g_var);
			for_return += Mips::sw(arg1->get_reg(), -func_para_size, 29);
			break;
		}
		case Operation::JAL:			// goto func only
		{
			if (func_para_size != 0)
				for_return += Mips::addi(29, 29, -func_para_size);
			for_return += Mips::sw(30, -4, 29);								//save $fp
			for_return += Mips::move(30, 29);								//$fp = $sp
			for_return += Mips::addi(29, 29, -4);							//$sp ready
			for_return += Mips::jal("func_" + arg1->get_id() + "_begin");	//jump
			for_return += Mips::lw(30, 0, 29);								//get $fp back
			for_return += Mips::addi(29, 29, 4 + all_code->get_para_size_by_name(arg1->get_id()));
																			//pop paras and $fp from stack
			func_para_size -= all_code->get_para_size_by_name(arg1->get_id());
			break;
		}
		case Operation::JUMP:
		{
			for_return += Mips::jump(arg1->get_id());
			break;
		}
		case Operation::LOAD_RET:
		{
			for_return += Mips::move(arg1->get_reg(), 2);	//load from $v0
			for_return += save_to_stack(arg1, l_const, g_const, l_var, g_var);
			break;
		}
		case Operation::SAVE_RET:
		{
			for_return += load_arg_to_reg(arg1, l_const, g_const, l_var, g_var);
			for_return += Mips::move(2, arg1->get_reg());	//save to $v0
			break;
		}
		case Operation::BEQI:
		{
			for_return += load_arg_to_reg(arg1, l_const, g_const, l_var, g_var);
			for_return += Mips::beqi(arg1->get_reg(), arg2->get_value(), out->get_id());
			break;
		}
		case Operation::BEQ:
		{
			for_return += load_arg_to_reg(arg1, l_const, g_const, l_var, g_var);
			for_return += load_arg_to_reg(arg2, l_const, g_const, l_var, g_var);
			for_return += Mips::beq(arg1->get_reg(), arg2->get_reg(), out->get_id());
			break;
		}
		case Operation::BNE:
		{
			for_return += load_arg_to_reg(arg1, l_const, g_const, l_var, g_var);
			for_return += load_arg_to_reg(arg2, l_const, g_const, l_var, g_var);
			for_return += Mips::bne(arg1->get_reg(), arg2->get_reg(), out->get_id());
			break;
		}
		case Operation::BGE:
		{
			for_return += load_arg_to_reg(arg1, l_const, g_const, l_var, g_var);
			for_return += load_arg_to_reg(arg2, l_const, g_const, l_var, g_var);
			for_return += Mips::bge(arg1->get_reg(), arg2->get_reg(), out->get_id());
			break;
		}
		case Operation::BGT:
		{
			for_return += load_arg_to_reg(arg1, l_const, g_const, l_var, g_var);
			for_return += load_arg_to_reg(arg2, l_const, g_const, l_var, g_var);
			for_return += Mips::bgt(arg1->get_reg(), arg2->get_reg(), out->get_id());
			break;
		}
		case Operation::BLE:
		{
			for_return += load_arg_to_reg(arg1, l_const, g_const, l_var, g_var);
			for_return += load_arg_to_reg(arg2, l_const, g_const, l_var, g_var);
			for_return += Mips::ble(arg1->get_reg(), arg2->get_reg(), out->get_id());
			break;
		}
		case Operation::BLT:
		{
			for_return += load_arg_to_reg(arg1, l_const, g_const, l_var, g_var);
			for_return += load_arg_to_reg(arg2, l_const, g_const, l_var, g_var);
			for_return += Mips::blt(arg1->get_reg(), arg2->get_reg(), out->get_id());
			break;
		}
		case Operation::LABEL:
		{
			for_return += arg1->get_id() + ":\n";
			break;
		}
	}
	return for_return;
}

Arg::Arg(ArgType t, string id, Arg* _offset) :
	type(t), identify(id), offset(_offset)
{
	value_char = 0;
	value_int = 0;
}

string Arg::to_string()
{
	if (type == ArgType::INT)
		return std::to_string(value_int);
	if (type == ArgType::CHAR)
		return std::to_string((int)value_char) + "_c";
	if (type == ArgType::IDENTIFY)
		return identify;
	if (type == ArgType::ARRAY)
	{
		string for_return;
		for_return.clear();
		for_return += identify;
		for_return += "[";
		for_return += offset->to_string();
		for_return += "]";
		return for_return;
	}
	return "_/_";
}

bool Arg::operator==(const Arg& a)
{
	if (this->type != a.type)
		return false;
	else if (this->type == ArgType::ARRAY)
	{
		return (this->identify == a.identify)
			&& (*(this->offset) == *(a.offset));
	}
	else if (this->type == ArgType::INT)
		return this->value_int == a.value_int;
	else if (this->type == ArgType::CHAR)
		return this->value_char == a.value_char;
	else if (this->type == ArgType::NONE)
		return false;
	else
		return this->identify == a.identify;
}

string op_to_str(Operation op)
{
	switch (op)
	{
	case Operation::ADD:
		return "ADD";
	case Operation::ADDI:
		return "ADDI";
	case Operation::SUB:
		return "SUB";
	case Operation::SUBI:
		return "SUBI";
	case Operation::MULT:
		return "MULT";
	case Operation::MULI:
		return "MULI";
	case Operation::DIV:
		return "DIV";
	case Operation::DIVI:
		return "DIVI";
	case Operation::NEG:
		return "NEG";
	case Operation::ASSIGN:
		return "ASSIGN";
	case Operation::INIT:
		return "INIT";
	case Operation::P_CHAR:
		return "P_CHAR";
	case Operation::P_INT:
		return "P_INT";
	case Operation::P_STR:
		return "P_STR";
	case Operation::S_INT:
		return "S_INT";
	case Operation::S_CHAR:
		return "S_CHAR";
	case Operation::START_FUNC:
		return "FUNC_BEGIN";
	case Operation::SAVE_PARA:
		return "SAVE";
	case Operation::JAL:
		return "J_FUNC";
	case Operation::JUMP:
		return "JUMP";
	case Operation::LOAD_RET:
		return "L_RET";
	case Operation::SAVE_RET:
		return "S_RET";
	case Operation::BEQI:
	case Operation::BEQ:
		return "BEQ";
	case Operation::BNE:
		return "BNE";
	case Operation::BGE:
		return "BGE";
	case Operation::BGT:
		return "BGT";
	case Operation::BLE:
		return "BLE";
	case Operation::BLT:
		return "BLT";
	case Operation::LABEL:
		return "LABEL";
	}
	return string();
}
