#include "MiddleCode.h"
#include "Mips.h"

void MiddleFunction::read_yu_ju(vector<Sentence> sent, StringTable& strings)
{
	unsigned int i;
	for (i = 0; i < sent.size(); i++)
	{
		if (sent[i].get_type() == 9)
		{
			SentenceLie* tmp = dynamic_cast<SentenceLie*>(sent[i].get_sentence());
			read_yu_ju(tmp->get_all_sentence(), strings);
		}
		else if (sent[i].get_type() == 4)
		{
			SentenceFuZhi* f = dynamic_cast<SentenceFuZhi*>(sent[i].get_sentence());
			vector<MiddleSentence> m = f->add_to_middle(*local_var, *global_var);
			m_sentences.insert(m_sentences.end(), m.begin(), m.end());
		}
		else if (sent[i].get_type() == 5)
		{
			SentenceRead* r = dynamic_cast<SentenceRead*>(sent[i].get_sentence());
			vector<MiddleSentence> m = r->add_to_middle(local_var, global_var);
			m_sentences.insert(m_sentences.end(), m.begin(), m.end());
		}
		else if (sent[i].get_type() == 6)
		{
			SentencePrint* p = dynamic_cast<SentencePrint*>(sent[i].get_sentence());
			vector<MiddleSentence> m = p->add_to_middle(strings, *local_var, *global_var);
			m_sentences.insert(m_sentences.end(), m.begin(), m.end());
		}
	}
}

string MiddleFunction::mips_begin_func()
{
	string for_return;
	for_return.clear();
	for_return += "\n";
	// func name
	for_return += "func_";
	for_return += name_in_low;
	for_return += "_begin:\n";
	// save $ra
	for_return += Mips::sw(31, -4, 30);
	// save regs
	unsigned int i;
	for (i = 0; i < 32; i++)
	{
		if (reg_need_save[i])
		{
			for_return += Mips::addi(29, 29, -4);//$sp = $sp - 4
			for_return += Mips::sw(i, 0, 29);
		}
	}
	// give space to local var
	for_return += Mips::addi(29, 29, -local_var->get_stack_size());
	return for_return;
}

string MiddleFunction::mips_end_func()
{
	string for_return;
	for_return.clear();
	// turn $sp back
	for_return += Mips::addi(29, 29, local_var->get_stack_size());
	// turn regs back
	int i;
	for (i = 31; i >= 0; i--)
	{
		if (reg_need_save[i])
		{
			for_return += Mips::lw(i, 0, 29);
			for_return += Mips::addi(29, 29, 4);
		}
	}
	// turn $ra back
	for_return += Mips::lw(31, -4, 30);
	for_return += Mips::jr();
	// label for func end
	for_return += "func_";
	for_return += name_in_low;
	for_return += "_end:\n";
	return for_return;
}

MiddleFunction::MiddleFunction(string name, ConstTable* gconst, VarTable* gvar) :
	name_in_low(name), global_const(gconst), global_var(gvar)
{
	m_sentences.clear();
	local_const = new ConstTable();
	local_var = new VarTable();
	int i;
	for (i = 0; i < 32; i++)
	{
		reg_need_save[i] = false;
	}
}

string MiddleFunction::get_name()
{
	return name_in_low;
}

void MiddleFunction::pre_orgnaize_reg()
{
	local_var->count_arg_stack();
	unsigned int i;
	for (i = 0; i < m_sentences.size(); i++)
	{
		switch (m_sentences[i].get_operation())
		{
			case Operation::ADD:
			case Operation::SUB:
			case Operation::MULT:
			case Operation::DIV:
			{
				m_sentences[i].get_arg1()->set_target_reg(8, true);
				m_sentences[i].get_arg2()->set_target_reg(9, true);
				m_sentences[i].get_arg_out()->set_target_reg(10, true);
				reg_need_save[8] = true;
				reg_need_save[9] = true;
				reg_need_save[10] = true;
				break;
			}
			case Operation::NEG:
			case Operation::ASSIGN:
			{
				m_sentences[i].get_arg1()->set_target_reg(8, true);
				m_sentences[i].get_arg_out()->set_target_reg(9, true);
				reg_need_save[8] = true;
				reg_need_save[9] = true;
				break;
			}
			case Operation::P_STR:
			case Operation::P_INT:
			case Operation::P_CHAR:
			{
				m_sentences[i].get_arg1()->set_target_reg(4, true);
				reg_need_save[4] = true;
			}
			case Operation::S_CHAR:
			case Operation::S_INT:
			{
				m_sentences[i].get_arg_out()->set_target_reg(8, true);
				break;
			}
		}
	}
}

void MiddleFunction::read_in_sentences(SentenceFuHe sent, StringTable& strings)
{
	int sent_type = sent.get_type();
	if (sent_type & 1)
	{
		sent.get_consts().add_to_middle_table(local_const);
	}
	if (sent_type & 2)
	{
		vector<MiddleSentence> m = sent.get_vars().add_to_middle_table(local_var);
		m_sentences.insert(m_sentences.end(), m.begin(), m.end());
	}
	read_yu_ju(sent.get_sentence(), strings);
	//printf("%d", m_sentences.size());
	unsigned int i;
	for (i = 0; i < m_sentences.size(); i++)
	{
		printf("%s", m_sentences[i].to_string().c_str());
	}
}

string MiddleFunction::to_mips(StringTable* strings)
{
	pre_orgnaize_reg();
	string for_return;
	for_return.clear();
	for_return += mips_begin_func();
	for_return += "\n";
	unsigned int i;
	for (i = 0; i < m_sentences.size(); i++)
	{
		for_return += m_sentences[i].to_mips
			(local_const, global_const, local_var, global_var, strings);
	}
	for_return += "\n";
	for_return += mips_end_func();
	return for_return;
}

string MiddleCode::mips_goto_func(string func_name)
{
	string for_return;
	for_return.clear();
	// save $fp
	for_return += Mips::addi(29, 29, -4);
	for_return += Mips::sw(30, -4, 29);
	// $fp = $sp
	for_return += Mips::move(30, 29);
	// pre space for $ra
	for_return += Mips::addi(29, 29, -4);
	// TODO: add paras to stack
	// jump 
	for_return += Mips::jal("func_" + func_name + "_begin");
	return for_return;
}

string MiddleCode::mips_exit_func()
{
	string for_return;
	for_return.clear();
	//TODO: pop paras from stack
	// pop $ra
	for_return += Mips::addi(29, 29, 4);
	// turn back $fp
	for_return += Mips::lw(30, 0, 29);
	for_return += Mips::addi(29, 29, 4);
	return for_return;
}

void MiddleCode::read_in(Grammar grammar)
{
	int g_type = grammar.get_type();
	if (g_type & 1)
	{
		grammar.get_const().add_to_middle_table(global_const);
	}
	if (g_type & 2)
	{
		vector<MiddleSentence> m = grammar.get_vars().add_to_middle_table(global_var);
		m_sentences.insert(m_sentences.end(), m.begin(), m.end());
	}
	if (g_type & 4)
	{
		//TODO not this time
	}
	MiddleFunction* f = new MiddleFunction("main", global_const, global_var);
	f->read_in_sentences(grammar.get_main().get_sentence(), *all_string);
	functions.push_back(f);
	unsigned int i;
	//printf("%d", m_sentences.size());
	for (i = 0; i < m_sentences.size(); i++)
	{
		printf("%s", m_sentences[i].to_string().c_str());
	}
}

void MiddleCode::print_mips_to_file(string file_name)
{
	global_var->count_arg_stack();
	ofstream out;
	out.open(file_name);
	// .data
	out << ".data\n";
	out << global_var->to_mips();
	out << all_string->to_mips();
	out << "\n";
	// .text
	out << ".text\n";
	unsigned int i;
	for (i = 0; i < m_sentences.size(); i++)
	{
		out << m_sentences[i].to_mips
			(new ConstTable(), global_const, 
			new VarTable(), global_var, all_string);
	}
	out << mips_goto_func("main");
	out << mips_exit_func();
	out << Mips::syscall(10);

	// functions
	for (i = 0; i < functions.size(); i++)
	{
		out << functions[i]->to_mips(all_string);
	}

	out.close();
}
