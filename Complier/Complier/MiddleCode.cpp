#include "MiddleCode.h"
#include "Mips.h"

void MiddleFunction::read_yu_ju(vector<Sentence> sent, StringTable& strings)
{
	unsigned int i;
	for (i = 0; i < sent.size(); i++)
	{
		switch (sent[i].get_type())
		{
			case 1:		//xun huan
			{
				SentenceXunHuan* x = dynamic_cast<SentenceXunHuan*>(sent[i].get_sentence());
				if (x->get_type() == 1) //while
				{
					string while_label = jump_label::get_while_label();
					//
					MiddleSentence while_begin(Operation::LABEL, new Arg(ArgType::IDENTIFY, while_label + "_begin"), NULL, NULL);
					m_sentences.push_back(while_begin);
					//
					vector<MiddleSentence> tmp = x->get_tiao_jian().add_to_middle(*local_var, *global_var, while_label + "_end");
					m_sentences.insert(m_sentences.end(), tmp.begin(), tmp.end());
					//
					vector<Sentence> while_sents;
					while_sents.clear();
					while_sents.push_back(*(x->get_sentence()));
					read_yu_ju(while_sents, strings);
					//
					MiddleSentence jump(Operation::JUMP, new Arg(ArgType::IDENTIFY, while_label + "_begin"), NULL, NULL);
					m_sentences.push_back(jump);
					//
					MiddleSentence while_end(Operation::LABEL, new Arg(ArgType::IDENTIFY, while_label + "_end"), NULL, NULL);
					m_sentences.push_back(while_end);
				}
				else //for
				{
					string for_label = jump_label::get_for_label();
					//
					vector<MiddleSentence> tmp = x->get_for_init(*local_var, *global_var);
					m_sentences.insert(m_sentences.end(), tmp.begin(), tmp.end());
					//
					MiddleSentence for_begin(Operation::LABEL, new Arg(ArgType::IDENTIFY, for_label + "_begin"), NULL, NULL);
					m_sentences.push_back(for_begin);
					//
					tmp = x->get_tiao_jian().add_to_middle(*local_var, *global_var, for_label + "_end");
					m_sentences.insert(m_sentences.end(), tmp.begin(), tmp.end());
					//
					vector<Sentence> for_sents;
					for_sents.clear();
					for_sents.push_back(*(x->get_sentence()));
					read_yu_ju(for_sents, strings);
					//
					tmp = x->get_for_update();
					m_sentences.insert(m_sentences.end(), tmp.begin(), tmp.end());
					//
					MiddleSentence for_end(Operation::LABEL, new Arg(ArgType::IDENTIFY, for_label + "_end"), NULL, NULL);
					m_sentences.push_back(for_end);
				}
				break;
			}
			case 2:		//tiao jian
			{
				SentenceTiaoJian* t = dynamic_cast<SentenceTiaoJian*>(sent[i].get_sentence());
				string if_label = jump_label::get_if_label();
				//
				MiddleSentence if_begin(Operation::LABEL, new Arg(ArgType::IDENTIFY, if_label + "_begin"), NULL, NULL);
				m_sentences.push_back(if_begin);
				//
				bool have_else = t->check_have_else();
				if (have_else)
				{
					vector<MiddleSentence> tmp = t->get_tiao_jian().add_to_middle(*local_var, *global_var, if_label + "_else");
					m_sentences.insert(m_sentences.end(), tmp.begin(), tmp.end());
				}
				else
				{
					vector<MiddleSentence> tmp = t->get_tiao_jian().add_to_middle(*local_var, *global_var, if_label + "_end");
					m_sentences.insert(m_sentences.end(), tmp.begin(), tmp.end());
				}
				//
				vector<Sentence> s_if;
				s_if.clear();
				s_if.push_back(*(t->get_s_if()));
				read_yu_ju(s_if, strings);
				//
				if (have_else)
				{
					MiddleSentence jump_begin(Operation::JUMP, new Arg(ArgType::IDENTIFY, if_label + "_end"), NULL, NULL);
					m_sentences.push_back(jump_begin);
					//
					MiddleSentence else_begin(Operation::LABEL, new Arg(ArgType::IDENTIFY, if_label + "_else"), NULL, NULL);
					m_sentences.push_back(else_begin);
					//
					vector<Sentence> s_else;
					s_else.clear();
					s_else.push_back(*(t->get_s_else()));
					read_yu_ju(s_else, strings);
				}
				//
				MiddleSentence if_end(Operation::LABEL, new Arg(ArgType::IDENTIFY, if_label + "_end"), NULL, NULL);
				m_sentences.push_back(if_end);
				break;
			}
			case 3:		//diao yong
			{
				SentenceDiaoYong* d = dynamic_cast<SentenceDiaoYong*>(sent[i].get_sentence());
				d->add_to_middle(m_sentences, *local_var, *global_var);
				break;
			}
			case 4:		//fu zhi
			{
				SentenceFuZhi* f = dynamic_cast<SentenceFuZhi*>(sent[i].get_sentence());
				vector<MiddleSentence> m = f->add_to_middle(*local_var, *global_var);
				m_sentences.insert(m_sentences.end(), m.begin(), m.end());
				break;
			}
			case 5:		//read
			{
				SentenceRead* r = dynamic_cast<SentenceRead*>(sent[i].get_sentence());
				vector<MiddleSentence> m = r->add_to_middle(local_var, global_var);
				m_sentences.insert(m_sentences.end(), m.begin(), m.end());
				break;
			}
			case 6:		//print
			{
				SentencePrint* p = dynamic_cast<SentencePrint*>(sent[i].get_sentence());
				vector<MiddleSentence> m = p->add_to_middle(strings, *local_var, *global_var);
				m_sentences.insert(m_sentences.end(), m.begin(), m.end());
				break;
			}
			case 7:		//fan hui
			{
				SentenceReturn* r = dynamic_cast<SentenceReturn*>(sent[i].get_sentence());
				vector<MiddleSentence> m = r->add_to_middle(*local_var, *global_var, this->get_name());
				m_sentences.insert(m_sentences.end(), m.begin(), m.end());
				break;
			}
			case 8:		//switch
			{
				SentenceQingKuang* q = dynamic_cast<SentenceQingKuang*>(sent[i].get_sentence());
				string switch_label = jump_label::get_switch_label();
				//
				Arg compare = q->get_expression().add_to_middle(m_sentences, *local_var, *global_var);
				vector<ConditionCase> cases = q->get_all_cases();
				unsigned int i;
				for (i = 0; i < cases.size(); i++)
				{
					Arg* chang_shu = new Arg(ArgType::INT, cases[i].get_case_num());
					Arg* label = new Arg(ArgType::IDENTIFY, switch_label + "_" + std::to_string(i));
					MiddleSentence beq(Operation::BEQI, &compare, chang_shu, label);
					m_sentences.push_back(beq);
				}
				//
				vector<Sentence> default_sent;
				default_sent.clear();
				default_sent.push_back(*(q->get_default().get_sentence()));
				read_yu_ju(default_sent, strings);
				//
				Arg* end = new Arg(ArgType::IDENTIFY, switch_label + "_end");
				MiddleSentence jump_out(Operation::JUMP, end, NULL, NULL);
				m_sentences.push_back(jump_out);
				//
				for (i = 0; i < cases.size(); i++)
				{
					Arg* label = new Arg(ArgType::IDENTIFY, switch_label + "_" + std::to_string(i));
					MiddleSentence c_label(Operation::LABEL, label, NULL, NULL);
					//
					m_sentences.push_back(c_label);
					vector<Sentence> c_sent;
					c_sent.clear();
					c_sent.push_back(*(cases[i].get_sentence()));
					read_yu_ju(c_sent, strings);
					//
					if (i != cases.size() - 1)
						m_sentences.push_back(jump_out);
				}
				//
				MiddleSentence end_label(Operation::LABEL, end, NULL, NULL);
				m_sentences.push_back(end_label);
				break;
			}
			case 9:		//yu ju lie
			{
				SentenceLie* tmp = dynamic_cast<SentenceLie*>(sent[i].get_sentence());
				read_yu_ju(tmp->get_all_sentence(), strings);
				break;
			}
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
	// for_return += Mips::sw(31, -4, 30);
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
	// label for func end
	for_return += "func_";
	for_return += name_in_low;
	for_return += "_end:\n";
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
	for_return += Mips::jr();
	for_return += "\n";
	return for_return;
}

int MiddleFunction::check_exit_arg(Arg a)
{
	unsigned int i;
	for (i = 4; i <= 25; i++)	//a0-a3, t0-t9, s0-s7
	{
		if (now_arg[i] == a)
			return i;
	}
	return 0;
}

int MiddleFunction::get_empty_reg()
{
	unsigned int fisrt_empty = 0;
	unsigned int i;
	for (i = 4; i <= 25; i++)
	{
		if (now_arg[i].get_type() == ArgType::NONE)
			return i;
	}
	return 0;
}

int MiddleFunction::get_chang_reg()
{
	int for_return = 4 + reg_count;	//4 ~ 25
	reg_count = (reg_count + 1) % 22;
	return for_return;
}

void MiddleFunction::orignaize_one_reg(Arg* a, bool is_save)
{
	if (a->get_type() == ArgType::ARRAY)
		orignaize_one_reg(a->get_offset(), false);
	int target_reg = check_exit_arg(*a);
	if (target_reg != 0)
	{
		if (is_save)
			a->set_target_reg(target_reg, true);
		else
			a->set_target_reg(target_reg, false);
		now_arg[target_reg] = *a;
		reg_need_save[target_reg] = true;
		return;
	}
	target_reg = get_empty_reg();
	if (target_reg == 0)
		target_reg = get_chang_reg();
	a->set_target_reg(target_reg, true);
	now_arg[target_reg] = *a;
	reg_need_save[target_reg] = true;
	return;
}

MiddleFunction::MiddleFunction(string name, ConstTable* gconst, VarTable* gvar) :
	name_in_low(name), global_const(gconst), global_var(gvar)
{
	m_sentences.clear();
	local_const = new ConstTable();
	local_var = new VarTable();
	int i;
	Arg empty(ArgType::NONE, '\0');
	for (i = 0; i < 32; i++)
	{
		reg_need_save[i] = false;
		now_arg[i] = empty;
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
				orignaize_one_reg(m_sentences[i].get_arg1(), false);
				orignaize_one_reg(m_sentences[i].get_arg2(), false);
				orignaize_one_reg(m_sentences[i].get_arg_out(), true);
				break;
			}
			case Operation::ADDI:
			case Operation::SUBI:
			case Operation::MULI:
			case Operation::DIVI:
			{
				orignaize_one_reg(m_sentences[i].get_arg1(), false);
				orignaize_one_reg(m_sentences[i].get_arg_out(), true);
				break;
			}
			case Operation::NEG:
			case Operation::ASSIGN:
			{
				orignaize_one_reg(m_sentences[i].get_arg1(), false);
				orignaize_one_reg(m_sentences[i].get_arg_out(), true);
				break;
			}
			case Operation::INIT:
			{
				orignaize_one_reg(m_sentences[i].get_arg_out(), true);
				break;
			}
			case Operation::P_STR:
			case Operation::P_INT:
			case Operation::P_CHAR:
			{
				if (m_sentences[i].get_arg1()->get_type() == ArgType::ARRAY)
					orignaize_one_reg(m_sentences[i].get_arg1()->get_offset(), false);
				m_sentences[i].get_arg1()->set_target_reg(4, true);
				now_arg[4] = *(m_sentences[i].get_arg1());
				reg_need_save[4] = true;
				break;
			}
			case Operation::S_CHAR:
			case Operation::S_INT:
			{
				orignaize_one_reg(m_sentences[i].get_arg_out(), true);
				break;
			}
			case Operation::SAVE_PARA:
			{
				orignaize_one_reg(m_sentences[i].get_arg1(), false);
				break;
			}
			case Operation::SAVE_RET:
			case Operation::LOAD_RET:
			{
				orignaize_one_reg(m_sentences[i].get_arg1(), false);
				break;
			}
			case Operation::BEQI:
			{
				orignaize_one_reg(m_sentences[i].get_arg1(), false);
				break;
			}
			case Operation::BEQ:
			case Operation::BNE:
			case Operation::BGE:
			case Operation::BGT:
			case Operation::BLE:
			case Operation::BLT:
			{
				orignaize_one_reg(m_sentences[i].get_arg1(), false);
				orignaize_one_reg(m_sentences[i].get_arg2(), false);
				break;
			}
			case Operation::START_FUNC:
			case Operation::JAL:
			case Operation::JUMP:
			case Operation::LABEL:
			{
				break;
			}
		}
	}
	// always save $ra
	reg_need_save[31] = true;
}

void MiddleFunction::add_func_parameter(ParameterTable table)
{
	if (table.check_empty()) //is empty
		return;
	vector<WordInfo> ids = table.get_ids();
	vector<WordInfo> types = table.get_types();
	unsigned int i = 0;
	for (i = 0; i < ids.size(); i++)
	{
		VarInfo* tmp = new VarInfo();
		tmp->dimenation = 0;
		tmp->d1 = 0;
		tmp->d2 = 0;
		tmp->have_initial = false;
		tmp->is_func_arg = true;
		tmp->name_in_low = ids[i].get_string_in_low();
		if (types[i].get_type() == TypeEnum::INTTK)
			tmp->type = 0;
		else
			tmp->type = 1;
		tmp->size_in_byte = 4;
		this->local_var->add_in(tmp);
	}
	return;
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

string MiddleFunction::to_mips(StringTable* strings, MiddleCode* all_code)
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
			(local_const, global_const, local_var, global_var, strings, all_code);
	}
	for_return += "\n";
	for_return += mips_end_func();
	return for_return;
}

int MiddleFunction::get_para_size()
{
	return local_var->get_para_size();
}

string MiddleCode::mips_goto_func(string func_name)
{
	string for_return;
	for_return.clear();
	// TODO: add paras from down to top
	// save $fp
	for_return += Mips::sw(30, -4, 29);
	// $fp = $sp + 4
	for_return += Mips::move(30, 29);
	for_return += Mips::addi(29, 29, -4);
	// jump 
	for_return += Mips::jal("func_" + func_name + "_begin");
	return for_return;
}

string MiddleCode::mips_exit_func()
{
	string for_return;
	for_return.clear();
	// turn back $fp
	for_return += Mips::lw(30, 0, 29);
	//for_return += Mips::addi(29, 29, 4);
	// TODO: pop paras of the func
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
		vector<FunctionDingYi> funcs = grammar.get_funcs();
		unsigned int i = 0;
		for (i = 0; i < funcs.size(); i++)
		{
			MiddleFunction* f = new MiddleFunction(funcs[i].get_func_name(), global_const, global_var);
			f->add_func_parameter(funcs[i].get_para_table());
			f->read_in_sentences(funcs[i].get_sentences(), *all_string);
			functions.push_back(f);
		}
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
			new VarTable(), global_var, all_string, this);
	}
	out << mips_goto_func("main");
	out << mips_exit_func();
	out << Mips::syscall(10);

	// functions
	for (i = 0; i < functions.size(); i++)
	{
		out << functions[i]->to_mips(all_string, this);
	}

	out.close();
}

int MiddleCode::get_para_size_by_name(string name)
{
	unsigned int i = 0;
	for (i = 0; i < functions.size(); i++)
	{
		if (functions[i]->get_name() == name)
			return functions[i]->get_para_size();
	}
	return -1;
}

string jump_label::get_for_label()
{
	string for_return;
	for_return.clear();
	for_return += "for_";
	for_return += std::to_string(for_count);
	for_count++;
	return for_return;
}

string jump_label::get_while_label()
{
	string for_return;
	for_return.clear();
	for_return += "while_";
	for_return += std::to_string(while_count);
	while_count++;
	return for_return;
}

string jump_label::get_if_label()
{
	string for_return;
	for_return.clear();
	for_return += "if_";
	for_return += std::to_string(if_count);
	if_count++;
	return for_return;
}

string jump_label::get_switch_label()
{
	string for_return;
	for_return.clear();
	for_return += "switch_";
	for_return += std::to_string(switch_count);
	switch_count++;
	return for_return;
}
