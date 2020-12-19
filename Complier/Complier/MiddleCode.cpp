#include "MiddleCode.h"
#include "Mips.h"

#include <map>

string global_mf_name = "__global";

void MiddleFunction::read_yu_ju(vector<Sentence> sent, StringTable& strings)
{
	if (this->name_in_low != global_mf_name)
		IdentifyTable::set_active_block_by_name(this->name_in_low);
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
					vector<MiddleSentence> tmp = x->get_tiao_jian().add_to_middle(
						*local_var, *global_var, local_const, global_const, while_label + "_end");
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
					vector<MiddleSentence> tmp = x->get_for_init(*local_var, *global_var, local_const, global_const);
					m_sentences.insert(m_sentences.end(), tmp.begin(), tmp.end());
					//
					MiddleSentence for_begin(Operation::LABEL, new Arg(ArgType::IDENTIFY, for_label + "_begin"), NULL, NULL);
					m_sentences.push_back(for_begin);
					//
					tmp = x->get_tiao_jian().add_to_middle(
						*local_var, *global_var, local_const, global_const, for_label + "_end");
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
					MiddleSentence jump_back(Operation::JUMP, new Arg(ArgType::IDENTIFY, for_label + "_begin"), NULL, NULL);
					m_sentences.push_back(jump_back);
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
					vector<MiddleSentence> tmp = t->get_tiao_jian().add_to_middle(
						*local_var, *global_var, local_const, global_const, if_label + "_else");
					m_sentences.insert(m_sentences.end(), tmp.begin(), tmp.end());
				}
				else
				{
					vector<MiddleSentence> tmp = t->get_tiao_jian().add_to_middle(
						*local_var, *global_var, local_const, global_const, if_label + "_end");
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
				d->add_to_middle(m_sentences, local_const, global_const, *local_var, *global_var);
				break;
			}
			case 4:		//fu zhi
			{
				SentenceFuZhi* f = dynamic_cast<SentenceFuZhi*>(sent[i].get_sentence());
				vector<MiddleSentence> m = f->add_to_middle(*local_var, *global_var, local_const, global_const);
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
				vector<MiddleSentence> m = p->add_to_middle(strings, *local_var, *global_var, local_const, global_const);
				m_sentences.insert(m_sentences.end(), m.begin(), m.end());
				break;
			}
			case 7:		//fan hui
			{
				SentenceReturn* r = dynamic_cast<SentenceReturn*>(sent[i].get_sentence());
				vector<MiddleSentence> m = r->add_to_middle(
					*local_var, *global_var, local_const, global_const, this->get_name());
				m_sentences.insert(m_sentences.end(), m.begin(), m.end());
				break;
			}
			case 8:		//switch
			{
				SentenceQingKuang* q = dynamic_cast<SentenceQingKuang*>(sent[i].get_sentence());
				string switch_label = jump_label::get_switch_label();
				//
				Arg* compare = q->get_expression().add_to_middle(m_sentences, *local_var, *global_var, local_const, global_const);
				vector<ConditionCase> cases = q->get_all_cases();
				unsigned int i;
				for (i = 0; i < cases.size(); i++)
				{
					Arg* chang_shu = new Arg(ArgType::INT, cases[i].get_case_num());
					Arg* label = new Arg(ArgType::IDENTIFY, switch_label + "_" + std::to_string(i));
					MiddleSentence beq(Operation::BEQI, new Arg(compare), chang_shu, label);
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
	if (this->name_in_low == "main")
	{
		for_return += Mips::addi(29, 29, -4);	//$sp = $sp - 4
		for_return += Mips::sw(31, 0, 29);		// store $ra only
	}
	else
	{
		unsigned int i;
		int reg_count = 0;
		for (i = 0; i < 32; i++)
		{
			if (reg_need_save[i])
			{
				reg_count += 4;
				//for_return += Mips::addi(29, 29, -4);//$sp = $sp - 4
				for_return += Mips::sw(i, -reg_count, 29);
			}
		}
		for_return += Mips::addi(29, 29, -reg_count);
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
	if (this->name_in_low == "main")
	{
		for_return += Mips::lw(31, 0, 29);		//turn back $ra only
	}
	else
	{
		int i;
		int reg_count = 0;
		for (i = 31; i >= 0; i--)
		{
			if (reg_need_save[i])
			{
				for_return += Mips::lw(i, reg_count, 29);
				reg_count += 4;
				//for_return += Mips::addi(29, 29, 4);
			}
		}
		for_return += Mips::addi(29, 29, reg_count);
	}
	for_return += Mips::jr();
	for_return += "\n";
	return for_return;
}

int MiddleFunction::check_exit_arg(Arg* a, bool add_opt)
{
	unsigned int i;
	if (!add_opt)
	{
		for (i = 5; i <= 25; i++)	//a1-a3, t0-t9, s0-s7
		{
			if (now_arg[i] == a)
				return i;
		}
		return 0;
	}
	else
	{
		for (i = 5; i <= 25; i++)	//a1-a3, t0-t9, s0-s7
		{
			if (now_arg[i] != NULL && *(now_arg[i]) == *a)
				return i;
		}
		return 0;
	}
}

void MiddleFunction::clear_now_arg()
{
	int i;
	for (i = 0; i < 31; i++)
	{
		now_arg[i] = NULL;
	}
}

int MiddleFunction::get_empty_reg()
{
	unsigned int fisrt_empty = 0;
	unsigned int i;
	for (i = 5; i <= 25; i++)
	{
		if (now_arg[i] == NULL || now_arg[i]->get_type() == ArgType::NONE)
			return i;
	}
	return 0;
}

int MiddleFunction::get_chang_reg()
{
	int for_return = 5 + reg_count;	//5 ~ 25
	reg_count = (reg_count + 1) % 21;
	return for_return;
}

void MiddleFunction::orignaize_one_reg(Arg* a, bool is_save, bool add_opt)
{
	if (a->get_type() == ArgType::ARRAY)
		orignaize_one_reg(a->get_offset(), false, add_opt);
	if (a->is_static() && a->get_value() == 0)
	{
		a->set_target_reg(0, false);
		return;
	}
	int target_reg = check_exit_arg(a, add_opt);
	if (target_reg != 0)
	{
		if (is_save)
			a->set_target_reg(target_reg, true);
		else if (!add_opt)
			a->set_target_reg(target_reg, true);
		else
			a->set_target_reg(target_reg, false);
		reg_need_save[target_reg] = true;
		return;
	}
	target_reg = get_empty_reg();
	if (target_reg == 0)
	{
		target_reg = get_chang_reg();
		now_arg[target_reg]->set_need_stack(true);
	}
	a->set_target_reg(target_reg, true);
	now_arg[target_reg] = a;
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
	for (i = 0; i < 32; i++)
	{
		reg_need_save[i] = false;
		now_arg[i] = NULL;
	}
}

string MiddleFunction::get_name()
{
	return name_in_low;
}

void MiddleFunction::pre_orgnaize_reg(vector<MiddleSentence> m_sent, bool add_opt)
{
	local_var->count_arg_stack();
	clear_now_arg();
	unsigned int i;
	for (i = 0; i < m_sent.size(); i++)
	{
		switch (m_sent[i].get_operation())
		{
			case Operation::ADD:
			case Operation::SUB:
			case Operation::MULT:
			case Operation::DIV:
			{
				orignaize_one_reg(m_sent[i].get_arg1(), false, add_opt);
				orignaize_one_reg(m_sent[i].get_arg2(), false, add_opt);
				orignaize_one_reg(m_sent[i].get_arg_out(), true, add_opt);
				break;
			}
			case Operation::ADDI:
			case Operation::SUBI:
			case Operation::MULI:
			case Operation::DIVI:
			case Operation::SLL:
			{
				orignaize_one_reg(m_sent[i].get_arg1(), false, add_opt);
				orignaize_one_reg(m_sent[i].get_arg_out(), true, add_opt);
				break;
			}
			case Operation::NEG:
			case Operation::ASSIGN:
			{
				orignaize_one_reg(m_sent[i].get_arg1(), false, add_opt);
				orignaize_one_reg(m_sent[i].get_arg_out(), true, add_opt);
				break;
			}
			case Operation::INIT:
			{
				orignaize_one_reg(m_sent[i].get_arg_out(), true, add_opt);
				break;
			}
			case Operation::P_STR:
			case Operation::P_INT:
			case Operation::P_CHAR:
			{
				if (m_sent[i].get_arg1()->get_type() == ArgType::ARRAY)
					orignaize_one_reg(m_sent[i].get_arg1()->get_offset(), false, add_opt);
				m_sent[i].get_arg1()->set_target_reg(4, true);
				now_arg[4] = m_sent[i].get_arg1();
				reg_need_save[4] = true;
				break;
			}
			case Operation::S_CHAR:
			case Operation::S_INT:
			{
				orignaize_one_reg(m_sent[i].get_arg_out(), true, add_opt);
				break;
			}
			case Operation::SAVE_PARA:
			{
				orignaize_one_reg(m_sent[i].get_arg1(), false, add_opt);
				break;
			}
			case Operation::SAVE_RET:
			case Operation::LOAD_RET:
			{
				orignaize_one_reg(m_sent[i].get_arg1(), false, add_opt);
				break;
			}
			case Operation::BEQI:
			{
				orignaize_one_reg(m_sent[i].get_arg1(), false, add_opt);
				break;
			}
			case Operation::BEQ:
			case Operation::BNE:
			case Operation::BGE:
			case Operation::BGT:
			case Operation::BLE:
			case Operation::BLT:
			{
				orignaize_one_reg(m_sent[i].get_arg1(), false, add_opt);
				orignaize_one_reg(m_sent[i].get_arg2(), false, add_opt);
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
	//OUTPUT
	/*
	printf("\nFunc_%s:\n", this->name_in_low.c_str());
	unsigned int i;
	for (i = 0; i < m_sentences.size(); i++)
	{
		printf("%s", m_sentences[i].to_string().c_str());
	}
	*/
}

string MiddleFunction::to_mips(StringTable* strings, MiddleCode* all_code, bool add_opt)
{
	if (!add_opt)
		pre_orgnaize_reg(this->m_sentences, false);
	else
		optimize_immediate_all();
	printf("\nFunc_%s:\n", this->name_in_low.c_str());
	string for_return;
	for_return.clear();
	for_return += mips_begin_func();
	for_return += "\n";
	unsigned int i;
	int func_para_size = 0;
	for (i = 0; i < m_sentences.size(); i++)
	{
		printf("%s", m_sentences[i].to_string().c_str());
		for_return += m_sentences[i].to_mips(
			local_const, global_const, local_var, global_var, 
			strings, all_code, func_para_size);
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

vector<MiddleSentence> MiddleFunction::optimize_immediate_block(vector<MiddleSentence> old)
{
	vector<MiddleSentence> fig_ass;
	fig_ass.clear();
	unsigned int i;
	for (i = 0; i < old.size(); i++)
	{
		switch (old[i].get_operation())
		{
			case Operation::ADD:
			case Operation::SUB:
			case Operation::MULT:
			case Operation::DIV:
			case Operation::ADDI:
			case Operation::SUBI:
			case Operation::MULI:
			case Operation::DIVI:
			case Operation::NEG:
			{
				if (i + 1 < old.size() && old[i + 1].get_operation() == Operation::ASSIGN)
				{
					if (*(old[i].get_arg_out()) == *(old[i + 1].get_arg1()))
					{
						//printf("inside\n");
						MiddleSentence changed = old[i];
						changed.set_arg_out(old[i + 1].get_arg_out());
						fig_ass.push_back(changed);
						i++;
					}
					else
					{
						fig_ass.push_back(old[i]);
					}
				}
				else
					fig_ass.push_back(old[i]);
				break;
			}
			default:
			{
				fig_ass.push_back(old[i]);
				break;
			}
		}
	}
	vector<MiddleSentence> opted;
	opted.clear();
	map<Arg, int> arg_to_val;
	arg_to_val.clear();
	for (i = 0; i < fig_ass.size(); i++)
	{
		MiddleSentence this_sent = fig_ass[i];
		Arg* ar1 = this_sent.get_arg1();
		if (ar1 != NULL && ar1->get_type() == ArgType::ARRAY)
		{
			if (arg_to_val.count( *(ar1->get_offset() )) > 0)
			{
				Arg* o = new Arg(ArgType::INT, arg_to_val[*(ar1->get_offset())]);
				ar1->set_offset(o);
				this_sent.get_arg1()->set_offset(o);
			}
		}
		Arg* ar2 = this_sent.get_arg2();
		if (ar2 != NULL && ar2->get_type() == ArgType::ARRAY)
		{
			if (arg_to_val.count(*(ar2->get_offset())) > 0)
			{
				Arg* o = new Arg(ArgType::INT, arg_to_val[*(ar2->get_offset())]);
				ar2->set_offset(o);
				this_sent.get_arg2()->set_offset(o);
			}
		}
		Arg* out = this_sent.get_arg_out();
		if (out != NULL && out->get_type() == ArgType::ARRAY)
		{
			if (arg_to_val.count(*(out->get_offset())) > 0)
			{
				Arg* o = new Arg(ArgType::INT, arg_to_val[*(out->get_offset())]);
				out->set_offset(o);
				this_sent.get_arg_out()->set_offset(o);
			}
		}
		switch (fig_ass[i].get_operation())
		{
			case Operation::ADD:
			{
				if (ar1 == NULL || ar2 == NULL || out == NULL)
				{
					opted.push_back(this_sent);
					break;
				}	
				if (ar1->is_static() && ar2->is_static())
				{
					int value = ar1->get_value() + ar2->get_value();
					arg_to_val[*out] = value;
					if (!out->is_tmp)
					{
						MiddleSentence changed(Operation::INIT, new Arg(ArgType::INT, value), NULL, out);
						opted.push_back(changed);
					}
				}
				else if (ar1->is_static() && arg_to_val.count(*ar2) > 0)
				{
					int value = ar1->get_value() + arg_to_val[*ar2];
					arg_to_val[*out] = value;
					if (!out->is_tmp)
					{
						MiddleSentence changed(Operation::INIT, new Arg(ArgType::INT, value), NULL, out);
						opted.push_back(changed);
					}
				}
				else if (arg_to_val.count(*ar1) > 0 && ar2->is_static())
				{
					int value = arg_to_val[*ar1] + ar2->get_value();
					arg_to_val[*out] = value;
					if (!out->is_tmp)
					{
						MiddleSentence changed(Operation::INIT, new Arg(ArgType::INT, value), NULL, out);
						opted.push_back(changed);
					}
				}
				else if (arg_to_val.count(*ar1) > 0 && arg_to_val.count(*ar2) > 0)
				{
					int value = arg_to_val[*ar1] + arg_to_val[*ar2];
					arg_to_val[*out] = value;
					if (!out->is_tmp)
					{
						MiddleSentence changed(Operation::INIT, new Arg(ArgType::INT, value), NULL, out);
						opted.push_back(changed);
					}
				}
				else if (ar2->is_static() || arg_to_val.count(*ar2) > 0)
				{
					int t;
					if (ar2->is_static())
						t = ar2->get_value();
					else
						t = arg_to_val[*ar2];
					if (t == 0)
					{
						MiddleSentence change(Operation::ASSIGN, ar1, NULL, out);
						opted.push_back(change);
					}
					else
					{
						MiddleSentence change(Operation::ADDI, ar1, new Arg(ArgType::INT, t), out);
						opted.push_back(change);
					}
					arg_to_val.erase(*out);
				}
				else if (ar1->is_static() || arg_to_val.count(*ar1) > 0)
				{
					int t;
					if (ar1->is_static())
						t = ar1->get_value();
					else
						t = arg_to_val[*ar1];
					if (t == 0)
					{
						MiddleSentence change(Operation::ASSIGN, ar2, NULL, out);
						opted.push_back(change);
					}
					else
					{
						MiddleSentence change(Operation::ADDI, ar2, new Arg(ArgType::INT, t), out);
						opted.push_back(change);
					}
					arg_to_val.erase(*out);
				}
				else
				{
					arg_to_val.erase(*out);
					opted.push_back(this_sent);
				}
				break;
			}
			case Operation::ADDI:
			{
				if (ar1 == NULL || ar2 == NULL || out == NULL)
				{
					opted.push_back(this_sent);
					break;
				}
				if (ar1->is_static())
				{
					int value = ar1->get_value() + ar2->get_value();
					arg_to_val[*out] = value;
					if (!out->is_tmp)
					{
						MiddleSentence changed(Operation::INIT, new Arg(ArgType::INT, value), NULL, out);
						opted.push_back(changed);
					}
				}
				else if (arg_to_val.count(*ar1) > 0)
				{
					int value = arg_to_val[*ar1] + ar2->get_value();
					arg_to_val[*out] = value;
					if (!out->is_tmp)
					{
						MiddleSentence changed(Operation::INIT, new Arg(ArgType::INT, value), NULL, out);
						opted.push_back(changed);
					}
				}
				else if (ar2->get_value() == 0)
				{
					arg_to_val.erase(*out);
					MiddleSentence change(Operation::ASSIGN, ar1, NULL, out);
					opted.push_back(change);
				}
				else
				{
					arg_to_val.erase(*out);
					opted.push_back(this_sent);
				}
				break;
			}
			case Operation::SUB:
			{
				if (ar1 == NULL || ar2 == NULL || out == NULL)
				{
					opted.push_back(this_sent);
					break;
				}
				if (ar1->is_static() && ar2->is_static())
				{
					int value = ar1->get_value() - ar2->get_value();
					arg_to_val[*out] = value;
					if (!out->is_tmp)
					{
						MiddleSentence changed(Operation::INIT, new Arg(ArgType::INT, value), NULL, out);
						opted.push_back(changed);
					}
				}
				else if (ar1->is_static() && arg_to_val.count(*ar2) > 0)
				{
					int value = ar1->get_value() - arg_to_val[*ar2];
					arg_to_val[*out] = value;
					if (!out->is_tmp)
					{
						MiddleSentence changed(Operation::INIT, new Arg(ArgType::INT, value), NULL, out);
						opted.push_back(changed);
					}
				}
				else if (arg_to_val.count(*ar1) > 0 && ar2->is_static())
				{
					int value = arg_to_val[*ar1] - ar2->get_value();
					arg_to_val[*out] = value;
					if (!out->is_tmp)
					{
						MiddleSentence changed(Operation::INIT, new Arg(ArgType::INT, value), NULL, out);
						opted.push_back(changed);
					}
				}
				else if (arg_to_val.count(*ar1) > 0 && arg_to_val.count(*ar2) > 0)
				{
					int value = arg_to_val[*ar1] - arg_to_val[*ar2];
					arg_to_val[*out] = value;
					if (!out->is_tmp)
					{
						MiddleSentence changed(Operation::INIT, new Arg(ArgType::INT, value), NULL, out);
						opted.push_back(changed);
					}
				}
				else if (ar2->is_static() || arg_to_val.count(*ar2) > 0)
				{
					int t;
					if (ar2->is_static())
						t = ar2->get_value();
					else
						t = arg_to_val[*ar2];
					if (t == 0)
					{
						MiddleSentence change(Operation::ASSIGN, ar1, NULL, out);
						opted.push_back(change);
					}
					else
					{
						MiddleSentence change(Operation::SUBI, ar1, new Arg(ArgType::INT, t), out);
						opted.push_back(change);
					}
					arg_to_val.erase(*out);
				}
				else if (ar1->is_static() || arg_to_val.count(*ar1) > 0)
				{
					int t;
					if (ar1->is_static())
						t = ar1->get_value();
					else
						t = arg_to_val[*ar1];
					if (t == 0)
					{
						MiddleSentence change(Operation::NEG, ar2, NULL, out);
						opted.push_back(change);
					}
					else
					{
						MiddleSentence change(Operation::SUB, new Arg(ArgType::INT, t), ar2, out);
						opted.push_back(change);
					}
					arg_to_val.erase(*out);
				}
				else
				{
					arg_to_val.erase(*out);
					opted.push_back(this_sent);
				}
				break;
			}
			case Operation::SUBI:
			{
				if (ar1 == NULL || ar2 == NULL || out == NULL)
				{
					opted.push_back(this_sent);
					break;
				}
				if (ar1->is_static())
				{
					int value = ar1->get_value() - ar2->get_value();
					arg_to_val[*out] = value;
					if (!out->is_tmp)
					{
						MiddleSentence changed(Operation::INIT, new Arg(ArgType::INT, value), NULL, out);
						opted.push_back(changed);
					}
				}
				else if (arg_to_val.count(*ar1) > 0)
				{
					int value = arg_to_val[*ar1] - ar2->get_value();
					arg_to_val[*out] = value;
					if (!out->is_tmp)
					{
						MiddleSentence changed(Operation::INIT, new Arg(ArgType::INT, value), NULL, out);
						opted.push_back(changed);
					}
				}
				else if (ar2->get_value() == 0)
				{
					arg_to_val.erase(*out);
					MiddleSentence change(Operation::ASSIGN, ar1, NULL, out);
					opted.push_back(change);
				}
				else
				{
					arg_to_val.erase(*out);
					opted.push_back(this_sent);
				}
				break;
			}
			case Operation::MULT:
			{
				if (ar1 == NULL || ar2 == NULL || out == NULL)
				{
					opted.push_back(this_sent);
					break;
				}
				if (ar1->is_static() && ar2->is_static())
				{
					int value = ar1->get_value() * ar2->get_value();
					arg_to_val[*out] = value;
					if (!out->is_tmp)
					{
						MiddleSentence changed(Operation::INIT, new Arg(ArgType::INT, value), NULL, out);
						opted.push_back(changed);
					}
				}
				else if (ar1->is_static() && arg_to_val.count(*ar2) > 0)
				{
					int value = ar1->get_value() * arg_to_val[*ar2];
					arg_to_val[*out] = value;
					if (!out->is_tmp)
					{
						MiddleSentence changed(Operation::INIT, new Arg(ArgType::INT, value), NULL, out);
						opted.push_back(changed);
					}
				}
				else if (arg_to_val.count(*ar1) > 0 && ar2->is_static())
				{
					int value = arg_to_val[*ar1] * ar2->get_value();
					arg_to_val[*out] = value;
					if (!out->is_tmp)
					{
						MiddleSentence changed(Operation::INIT, new Arg(ArgType::INT, value), NULL, out);
						opted.push_back(changed);
					}
				}
				else if (arg_to_val.count(*ar1) > 0 && arg_to_val.count(*ar2) > 0)
				{
					int value = arg_to_val[*ar1] * arg_to_val[*ar2];
					arg_to_val[*out] = value;
					if (!out->is_tmp)
					{
						MiddleSentence changed(Operation::INIT, new Arg(ArgType::INT, value), NULL, out);
						opted.push_back(changed);
					}
				}
				else if (ar2->is_static() || arg_to_val.count(*ar2) > 0)
				{
					int t;
					if (ar2->is_static())
						t = ar2->get_value();
					else
						t = arg_to_val[*ar2];
					int value = t;
					if (value == 0)
					{
						MiddleSentence change(Operation::INIT, new Arg(ArgType::INT, 0), NULL, out);
						opted.push_back(change);
					}
					else if (value > 0)
					{
						t = jump_label::log2(t);
						if (t == -1)
						{
							MiddleSentence change(Operation::MULI, ar1, new Arg(ArgType::INT, value), out);
							opted.push_back(change);
						}
						else if (t == 0)
						{
							MiddleSentence change(Operation::ASSIGN, ar1, NULL, out);
							opted.push_back(change);
						}
						else
						{
							MiddleSentence change(Operation::SLL, ar1, new Arg(ArgType::INT, t), out);
							opted.push_back(change);
						}
					}
					else	//value < 0
					{
						t = jump_label::log2(-t);
						if (t == -1)
						{
							MiddleSentence change(Operation::MULI, ar1, new Arg(ArgType::INT, value), out);
							opted.push_back(change);
						}
						else if (t == 0)
						{
							MiddleSentence change(Operation::NEG, ar1, NULL, out);
							opted.push_back(change);
						}
						else
						{
							MiddleSentence change(Operation::SLL, ar1, new Arg(ArgType::INT, t), out);
							opted.push_back(change);
							MiddleSentence neg(Operation::NEG, new Arg(out), NULL, new Arg(out));
							opted.push_back(neg);
						}
					}
					arg_to_val.erase(*out);
				}
				else if (ar1->is_static() || arg_to_val.count(*ar1) > 0)
				{
					int t;
					if (ar1->is_static())
						t = ar1->get_value();
					else
						t = arg_to_val[*ar1];
					int value = t;
					if (value == 0)
					{
						MiddleSentence change(Operation::INIT, new Arg(ArgType::INT, 0), NULL, out);
						opted.push_back(change);
					}
					else if (value > 0)
					{
						//TODO: mult under_zero or zero while cause error
						t = jump_label::log2(t);
						if (t == -1)
						{
							MiddleSentence change(Operation::MULI, ar2, new Arg(ArgType::INT, value), out);
							opted.push_back(change);
						}
						else if (t == 0)
						{
							MiddleSentence change(Operation::ASSIGN, ar2, NULL, out);
							opted.push_back(change);
						}
						else
						{
							MiddleSentence change(Operation::SLL, ar2, new Arg(ArgType::INT, t), out);
							opted.push_back(change);
						}
					}
					else	//value < 0
					{
						t = jump_label::log2(-t);
						if (t == -1)
						{
							MiddleSentence change(Operation::MULI, ar2, new Arg(ArgType::INT, value), out);
							opted.push_back(change);
						}
						else if (t == 0)
						{
							MiddleSentence change(Operation::NEG, ar2, NULL, out);
							opted.push_back(change);
						}
						else
						{
							MiddleSentence change(Operation::SLL, ar2, new Arg(ArgType::INT, t), out);
							opted.push_back(change);
							MiddleSentence neg(Operation::NEG, new Arg(out), NULL, new Arg(out));
							opted.push_back(neg);
						}
					}
					arg_to_val.erase(*out);
				}
				else
				{
					arg_to_val.erase(*out);
					opted.push_back(this_sent);
				}
				break;
			}
			case Operation::MULI:
			{
				if (ar1 == NULL || ar2 == NULL || out == NULL)
				{
					opted.push_back(this_sent);
					break;
				}
				if (ar1->is_static())
				{
					int value = ar1->get_value() * ar2->get_value();
					arg_to_val[*out] = value;
					if (!out->is_tmp)
					{
						MiddleSentence changed(Operation::INIT, new Arg(ArgType::INT, value), NULL, out);
						opted.push_back(changed);
					}
				}
				else if (arg_to_val.count(*ar1) > 0)
				{
					int value = arg_to_val[*ar1] * ar2->get_value();
					arg_to_val[*out] = value;
					if (!out->is_tmp)
					{
						MiddleSentence changed(Operation::INIT, new Arg(ArgType::INT, value), NULL, out);
						opted.push_back(changed);
					}
				}
				else if (ar2->get_value() == 0)
				{
					MiddleSentence change(Operation::INIT, new Arg(ArgType::INT, 0), NULL, out);
					opted.push_back(change);
				}
				else if (ar2->get_value() > 0 && jump_label::log2(ar2->get_value()) != -1)
				{
					int t = jump_label::log2(ar2->get_value());
					if (t == 0)
					{
						MiddleSentence changed(Operation::ASSIGN, ar1, NULL, out);
						opted.push_back(changed);
					}
					else
					{
						MiddleSentence changed(Operation::SLL, ar1, new Arg(ArgType::INT, t), out);
						opted.push_back(changed);
					}
					arg_to_val.erase(*out);
				}
				else if (ar2->get_value() < 0 && jump_label::log2(-ar2->get_value()) != -1)
				{	
					int t = jump_label::log2(-ar2->get_value());
					if (t == 0)
					{
						MiddleSentence changed(Operation::NEG, ar1, NULL, out);
						opted.push_back(changed);
					}
					else
					{
						MiddleSentence changed(Operation::SLL, ar1, new Arg(ArgType::INT, t), out);
						opted.push_back(changed);
						MiddleSentence neg(Operation::NEG, new Arg(out), NULL, new Arg(out));
						opted.push_back(neg);
					}
					arg_to_val.erase(*out);
				}
				else
				{
					arg_to_val.erase(*out);
					opted.push_back(this_sent);
				}
				break;
			}
			case Operation::DIV:
			{
				if (ar1 == NULL || ar2 == NULL || out == NULL)
				{
					opted.push_back(this_sent);
					break;
				}
				if (ar1->is_static() && ar2->is_static())
				{
					int value = ar1->get_value() / ar2->get_value();
					arg_to_val[*out] = value;
					if (!out->is_tmp)
					{
						MiddleSentence changed(Operation::INIT, new Arg(ArgType::INT, value), NULL, out);
						opted.push_back(changed);
					}
				}
				else if (ar1->is_static() && arg_to_val.count(*ar2) > 0)
				{
					int value = ar1->get_value() / arg_to_val[*ar2];
					arg_to_val[*out] = value;
					if (!out->is_tmp)
					{
						MiddleSentence changed(Operation::INIT, new Arg(ArgType::INT, value), NULL, out);
						opted.push_back(changed);
					}
				}
				else if (arg_to_val.count(*ar1) > 0 && ar2->is_static())
				{
					int value = arg_to_val[*ar1] / ar2->get_value();
					arg_to_val[*out] = value;
					if (!out->is_tmp)
					{
						MiddleSentence changed(Operation::INIT, new Arg(ArgType::INT, value), NULL, out);
						opted.push_back(changed);
					}
				}
				else if (arg_to_val.count(*ar1) > 0 && arg_to_val.count(*ar2) > 0)
				{
					int value = arg_to_val[*ar1] / arg_to_val[*ar2];
					arg_to_val[*out] = value;
					if (!out->is_tmp)
					{
						MiddleSentence changed(Operation::INIT, new Arg(ArgType::INT, value), NULL, out);
						opted.push_back(changed);
					}
				}
				else if (ar2->is_static() || arg_to_val.count(*ar2) > 0)
				{
					int t;
					if (ar2->is_static())
						t = ar2->get_value();
					else
						t = arg_to_val[*ar2];
					if (t == 1)
					{
						MiddleSentence change(Operation::ASSIGN, ar1, NULL, out);
						opted.push_back(change);
					}
					else
					{
						MiddleSentence change(Operation::DIVI, ar1, new Arg(ArgType::INT, t), out);
						opted.push_back(change);
					}
					arg_to_val.erase(*out);
				}
				else if (ar1->is_static() || arg_to_val.count(*ar1) > 0)
				{
					int t;
					if (ar1->is_static())
						t = ar1->get_value();
					else
						t = arg_to_val[*ar1];
					if (t == 0)
					{
						MiddleSentence change(Operation::INIT, new Arg(ArgType::INT, 0), NULL, out);
						opted.push_back(change);
						arg_to_val[*out] = 0;
					}
					else
					{
						MiddleSentence change(Operation::DIVI, new Arg(ArgType::INT, t), ar2, out);
						opted.push_back(change);
						arg_to_val.erase(*out);
					}
				}
				else
				{
					arg_to_val.erase(*out);
					opted.push_back(this_sent);
				}
				break;
			}
			case Operation::DIVI:
			{
				if (ar1 == NULL || ar2 == NULL || out == NULL)
				{
					opted.push_back(this_sent);
					break;
				}
				if (ar1->is_static())
				{
					int value = ar1->get_value() / ar2->get_value();
					arg_to_val[*out] = value;
					if (!out->is_tmp)
					{
						MiddleSentence changed(Operation::INIT, new Arg(ArgType::INT, value), NULL, out);
						opted.push_back(changed);
					}
				}
				else if (arg_to_val.count(*ar1) > 0)
				{
					int value = arg_to_val[*ar1] / ar2->get_value();
					arg_to_val[*out] = value;
					if (!out->is_tmp)
					{
						MiddleSentence changed(Operation::INIT, new Arg(ArgType::INT, value), NULL, out);
						opted.push_back(changed);
					}
				}
				else if (ar2->get_value() == 1)
				{
					arg_to_val.erase(*out);
					MiddleSentence change(Operation::ASSIGN, ar1, NULL, out);
					opted.push_back(change);
				}
				else
				{
					arg_to_val.erase(*out);
					opted.push_back(this_sent);
				}
				break;
			}
			case Operation::INIT:
			{
				if (ar1 == NULL || out == NULL)
				{
					opted.push_back(this_sent);
					break;
				}
				opted.push_back(this_sent);
				arg_to_val[*out] = ar1->get_value();
				break;
			}
			case Operation::ASSIGN:
			{
				if (ar1 == NULL || out == NULL)
				{
					opted.push_back(this_sent);
					break;
				}
				if (ar1->is_static() || arg_to_val.count(*ar1) > 0)
				{
					int t;
					if (ar1->is_static())
						t = ar1->get_value();
					else
						t = arg_to_val[*ar1];
					arg_to_val.erase(*out);
					arg_to_val[*out] = t;
					if (!out->is_tmp)
					{
						MiddleSentence change(Operation::INIT, new Arg(ArgType::INT, t), NULL, out);
						opted.push_back(change);
					}
				}
				else
				{
					arg_to_val.erase(*out);
					opted.push_back(this_sent);
				}
				break;
			}
			case Operation::NEG:
			{
				if (ar1->is_static())
				{
					int value = -ar1->get_value();
					if (out != NULL && !out->is_tmp)
					{
						MiddleSentence change(Operation::INIT, new Arg(ArgType::INT, value), NULL, out);
						opted.push_back(change);
					}
					if (out != NULL)
						arg_to_val[*out] = value;
				}
				else if (ar1 != NULL && arg_to_val.count(*ar1) > 0)
				{
					int value = -arg_to_val[*ar1];
					if (out != NULL && !out->is_tmp)
					{
						MiddleSentence change(Operation::INIT, new Arg(ArgType::INT, value), NULL, out);
						opted.push_back(change);
					}
					if (out != NULL)
						arg_to_val[*out] = value;
				}
				else
				{
					if (out != NULL)
						arg_to_val.erase(*out);
					opted.push_back(this_sent);
				}
				break;
			}
			default:
			{
				if (ar1 != NULL && arg_to_val.count(*ar1) > 0)
					this_sent.set_arg1(new Arg(ArgType::INT, arg_to_val[*ar1]));
				if (ar2 != NULL && arg_to_val.count(*ar2) > 0)
					this_sent.set_arg2(new Arg(ArgType::INT, arg_to_val[*ar2]));
				if (out != NULL)
					arg_to_val.erase(*out);
				opted.push_back(this_sent);
				break;
			}
		}
	}
	this->pre_orgnaize_reg(opted, true);
	return opted;
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
	/*
	unsigned int i;
	std::printf("\nInit global:\n");
	for (i = 0; i < m_sentences.size(); i++)
	{
		printf("%s", m_sentences[i].to_string().c_str());
	}
	*/
}

void MiddleCode::print_mips_to_file(string file_name, bool add_opt)
{
	//TODO
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
	MiddleFunction tmp_global(global_mf_name, global_const, global_var);
	tmp_global.pre_orgnaize_reg(m_sentences, false);
	unsigned int i;
	int func_para_size = 0;
	std::printf("\nInit global:\n");
	for (i = 0; i < m_sentences.size(); i++)
	{
		printf("%s", m_sentences[i].to_string().c_str());
		out << m_sentences[i].to_mips(
			new ConstTable(), global_const, new VarTable(), global_var, 
			all_string, this, func_para_size);
	}
	out << Mips::jal("func_main_begin");
	//out << mips_goto_func("main");
	//out << mips_exit_func();
	out << Mips::syscall(10);

	// functions
	for (i = 0; i < functions.size(); i++)
	{
		out << functions[i]->to_mips(all_string, this, add_opt);
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

void MiddleFunction::optimize_immediate_all()
{
	unsigned int i;
	vector<MiddleSentence> output;
	output.clear();
	vector<MiddleSentence> this_block;
	this_block.clear();
	for (i = 0; i < m_sentences.size(); i++)
	{
		if (m_sentences[i].get_operation() != Operation::LABEL)
		{
			this_block.push_back(m_sentences[i]);
		}
		else
		{
			vector<MiddleSentence> opted_block = optimize_immediate_block(this_block);
			output.insert(output.end(), opted_block.begin(), opted_block.end());
			output.push_back(m_sentences[i]);
			this_block.clear();
		}
	}
	vector<MiddleSentence> opted_block = optimize_immediate_block(this_block);
	output.insert(output.end(), opted_block.begin(), opted_block.end());
	this->m_sentences = output;
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

bool jump_label::starts_With(const string& src, const string& target)
{
	if (src.find(target) == 0) 
	{
		return true;
	}
	else 
	{
		return false;
	}
}

int jump_label::log2(int value)
{
	if ((value & (value - 1)) != 0)
		return -1;
	if (value == 1)
		return 0;
	else
		return 1 + log2(value >> 1);
}
