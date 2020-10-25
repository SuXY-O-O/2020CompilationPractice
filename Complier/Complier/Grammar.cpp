#include "Grammar.h"
#include "Error.h"

int ZiFuChuan::read_in(Lexer& lexer)
{
	this->word_pos = lexer.get_pos();
	this->zi_fu_chuan = lexer.get_next();
	if (zi_fu_chuan.get_type() == TypeEnum::STRCON)
		return -1;
	else
	{
		lexer.set_pos(word_pos);
		return word_pos + 1;
	}
}

string ZiFuChuan::to_string()
{
	return zi_fu_chuan.to_string() + "<字符串>\n";
}

int WuFuHaoShu::read_in(Lexer& lexer)
{
	this->word_pos = lexer.get_pos();
	this->wu_fu_hao_shu = lexer.get_next();
	if (wu_fu_hao_shu.get_type() == TypeEnum::INTCON)
		return -1;
	else
	{
		lexer.set_pos(word_pos);
		return word_pos + 1;
	}
}

string WuFuHaoShu::to_string()
{
	return wu_fu_hao_shu.to_string() + "<无符号整数>\n";
}

int ZhengShu::read_in(Lexer& lexer)
{
	this->word_pos = lexer.get_pos();
	WordInfo next = lexer.peek_next();
	if (next.get_type() == TypeEnum::PLUS || next.get_type() == TypeEnum::MINU)
	{
		sign = lexer.get_next();
		have_sign = true;
	}
	else
	{
		have_sign = false;
	}
	int r;
	if ((r = number.read_in(lexer)) == -1)
		return -1;
	else
	{
		lexer.set_pos(word_pos);
		return r;
	}
}

string ZhengShu::to_string()
{
	string for_return;
	for_return.clear();
	if (have_sign)
		for_return += sign.to_string();
	return for_return + number.to_string() + "<整数>\n";
}

int ConstDingYi::read_in(Lexer& lexer)
{
	word_pos = lexer.get_pos();
	type = lexer.get_next();
	if (type.get_type() == TypeEnum::INTTK)			//define a number of int
	{
		WordInfo tmp = lexer.peek_next();
		while (tmp.get_type() == TypeEnum::IDENFR)
		{
			tmp = lexer.get_next();
			ids.push_back(tmp);						//save id

			if (!IdentifyTable::add_const(&tmp, &type))	//log id to table
			{
				// Chong Ding Yi
				ErrorTable::log_error(tmp.get_line(), "b");
			}

			tmp = lexer.get_next();					//save "="
			if (tmp.get_type() != TypeEnum::ASSIGN)
				return lexer.get_pos();			//an error accure if not a "="
			puncs.push_back(tmp);

			ZhengShu shu;							//read an int
			int r;
			if ((r = shu.read_in(lexer)) != -1)
				return r;
			ints.push_back(shu);

			tmp = lexer.peek_next();				//check if have next defination
			if (tmp.get_type() == TypeEnum::COMMA)
			{
				puncs.push_back(lexer.get_next());	//have next
				tmp = lexer.peek_next();
			}
			else
			{
				return -1;							//end of these defination
			}
		}
		return lexer.get_pos();					//an error accure when reach here
	}

	else if (type.get_type() == TypeEnum::CHARTK)	//define a number of char
	{
		WordInfo tmp = lexer.peek_next();
		while (tmp.get_type() == TypeEnum::IDENFR)
		{
			tmp = lexer.get_next();
			ids.push_back(tmp);						//save id
			IdentifyTable::add_const(&tmp, &type);	//log id to table

			tmp = lexer.get_next();					//save "="
			if (tmp.get_type() != TypeEnum::ASSIGN)
				return lexer.get_pos();			//an error accure if not a "="
			puncs.push_back(tmp);

			tmp = lexer.get_next();					//save the char
			if (tmp.get_type() != TypeEnum::CHARCON)
				return lexer.get_pos();			//an error accure if not a char
			chars.push_back(tmp);

			tmp = lexer.peek_next();				//check if have next defination
			if (tmp.get_type() == TypeEnum::COMMA)
			{
				puncs.push_back(lexer.get_next());	//have next
				tmp = lexer.peek_next();
			}
			else
			{
				return -1;							//end of this
			}
		}
		return lexer.get_pos();					//an error accure when reach here
	}
	else
	{
		return lexer.get_pos();					//an error accure when reach here
	}
}

string ConstDingYi::to_string()
{
	int i;
	WordInfo comma(TypeEnum::COMMA, ",", 0, 0);
	WordInfo assign(TypeEnum::ASSIGN, "=", 0, 0);
	string for_return;
	for_return.clear();
	for_return += type.to_string();
	if (type.get_type() == TypeEnum::INTTK)
	{
		for (i = 0; i < ids.size() - 1; i++)
		{
			for_return += ids[i].to_string();
			for_return += assign.to_string();
			for_return += ints[i].to_string();
			for_return += comma.to_string();
		}
		i = ids.size() - 1;
		for_return += ids[i].to_string();
		for_return += assign.to_string();
		for_return += ints[i].to_string();
	}
	else if (type.get_type() == TypeEnum::CHARTK)
	{
		for (i = 0; i < ids.size() - 1; i++)
		{
			for_return += ids[i].to_string();
			for_return += assign.to_string();
			for_return += chars[i].to_string();
			for_return += comma.to_string();
		}
		i = ids.size() - 1;
		for_return += ids[i].to_string();
		for_return += assign.to_string();
		for_return += chars[i].to_string();
	}
	for_return += "<常量定义>\n";
	return for_return;
}

int ConstShuoMing::read_in(Lexer& lexer)
{
	word_pos = lexer.get_pos();
	WordInfo tmp = lexer.peek_next();
	while (tmp.get_type() == TypeEnum::CONSTTK)
	{
		consts.push_back(tmp);					//save "const"
		lexer.get_next();						//pop "const"

		int r;									//read const ding yi
		ConstDingYi ding_yi_now;
		if ((r = ding_yi_now.read_in(lexer)) != -1)
		{
			lexer.set_pos(word_pos);
			return r;							//if not a ding yi, raise an error
		}
		ding_yi_s.push_back(ding_yi_now);		//save const ding yi

		//tmp = lexer.get_next();					
		if (lexer.peek_next().get_type() != TypeEnum::SEMICN)	//read ";"
		{
			// Wu Fen Hao
			ErrorTable::log_error(tmp.get_line(), "k");
		}
		else
		{
			semicns.push_back(lexer.get_next());					//save ";"
		}

		tmp = lexer.peek_next();
	}
	return -1;									//success
}

string ConstShuoMing::to_string()
{
	string for_return;
	for_return.clear();
	int i;
	for (i = 0; i < consts.size(); i++)
	{
		for_return += consts[i].to_string();
		for_return += ding_yi_s[i].to_string();
		for_return += semicns[i].to_string();
	}
	for_return += "<常量说明>\n";
	return for_return;
}

int ShengMingHead::read_in(Lexer& lexer)
{
	word_pos = lexer.get_pos();
	type = lexer.get_next();
	if (type.get_type() != TypeEnum::INTTK && type.get_type() != TypeEnum::CHARTK)
	{
		lexer.set_pos(word_pos);
		return word_pos + 1;
	}
	id = lexer.get_next();
	if (id.get_type() != TypeEnum::IDENFR)
	{
		lexer.set_pos(word_pos);
		return word_pos + 2;
	}
	return -1;
}

string ShengMingHead::to_string()
{
	return type.to_string() + id.to_string() + "<声明头部>\n";
}

int ChangLiang::read_in(Lexer& lexer)
{
	word_pos = lexer.get_pos();
	WordInfo tmp = lexer.peek_next();
	int r = word_pos + 1;
	if (tmp.get_type() == TypeEnum::CHARCON)
	{
		type = 2;
		zi_fu = tmp;
		lexer.get_next();
		return -1;
	}
	else if ((r = zheng_shu.read_in(lexer)) == -1)
	{
		type = 1;
		return -1;
	}
	else
	{
		return r;
	}
}

string ChangLiang::to_string()
{
	if (type == 1)
	{
		return zheng_shu.to_string() + "<常量>\n";
	}
	else if (type == 2)
	{
		return zi_fu.to_string() + "<常量>\n";
	}
	else
	{
		return "";
	}
}

void VariableDingYi::add_to_table()
{
	unsigned int i = 1;
	while (i < define.size() && define[i].get_type() == TypeEnum::IDENFR)
	{
		WordInfo* now_id = &(define[i]);
		i++;
		unsigned int dim = 0;
		while (i < define.size()
			&& define[i].get_type() != TypeEnum::COMMA
			&& define[i].get_type() != TypeEnum::ASSIGN)
		{
			if (define[i].get_type() == TypeEnum::LBRACK)
				dim++;
			i++;
		}
		if (!IdentifyTable::add_var(now_id, &(define[0]), dim))
		{
			//Chong Ding Yi
			ErrorTable::log_error(now_id->get_line(), "b");
		}
		i++;
	}
}

bool VariableDingYi::check_initial()
{
	if (type == 1)
		return true;
	IdentifyType this_type;
	if (define[0].get_type() == TypeEnum::INTTK)
		this_type = IdentifyType::INT;
	else
		this_type = IdentifyType::CHAR;
	if (dimension == 0)
	{
		if (initial[0].get_type() != TypeEnum::EMPTY)
		{
			//Chu Shi Hua Ge Shu
			ErrorTable::log_error(define[0].get_line(), "n");
			return false;
		}
		else if (this_type != chang_liang_s[0].get_type())
		{
			//Chang Liang Lei Xing
			ErrorTable::log_error(define[0].get_line(), "o");
			return false;
		}
		return true;
	}
	else if (dimension == 1)
	{
		int target = zheng_shu_s[0].get_number();
		if (target != chang_liang_s.size())
		{
			//Chu Shi Hua Ge Shu
			ErrorTable::log_error(define[0].get_line(), "n");
			return false;
		}
		else
		{
			unsigned int i;
			for (i = 0; i < chang_liang_s.size(); i++)
			{
				if (chang_liang_s[i].get_type() != this_type)
				{
					//Chang Liang Lei Xing
					ErrorTable::log_error(define[0].get_line(), "o");
					return false;
				}
			}
		}
		return true;
	}
	else // dimension = 2
	{
		int target1 = zheng_shu_s[0].get_number();
		int target2 = zheng_shu_s[1].get_number();
		int count1 = 0;
		int count2 = 0;
		int braces = 0;
		unsigned int i;
		for (i = 0; i < initial.size(); i++)
		{
			if (initial[i].get_type() == TypeEnum::LBRACE)
			{
				braces++;
			}
			if (initial[i].get_type() == TypeEnum::EMPTY)
			{
				count2++;
			}
			if (initial[i].get_type() == TypeEnum::RBRACE)
			{
				braces--;
				count1++;
				if (count2 != target2 && braces == 1)
				{
					//Chu Shi Hua Ge Shu
					ErrorTable::log_error(initial[i].get_line(), "n");
					return false;
				}
				count2 = 0;
			}
		}
		count1 -= 1;
		if (count1 != target1)
		{
			//Chu Shi Hua Ge Shu
			ErrorTable::log_error(initial[0].get_line(), "n");
			return false;
		}
		for (i = 0; i < chang_liang_s.size(); i++)
		{
			if (chang_liang_s[i].get_type() != this_type)
			{
				//Chang Liang Lei Xing
				ErrorTable::log_error(define[0].get_line(), "o");
				return false;
			}
		}
		return true;
	}
}

int VariableDingYi::read_in(Lexer& lexer)
{
	/*
	* FSM: state
	* 0: begin					->1
	* 1: int/char read			->2
	* 2: id read				->3/6/7/8
	* 3: "[" read (dimension++)	->4
	* 4: unsigned int read		->5
	* 5: "]" read				->3/6/7/8
	* 6: "," read (type = 1)	->2
	* 7: next is ";" (end)		->out
	* 8: "=" read (type = 2)	->9
	* 9: read ChangLiang		->7
	*/
	int state = 0;
	type = 0;
	WordInfo empty(TypeEnum::EMPTY, 0, 0);
	word_pos = lexer.get_pos();
	WordInfo tmp = lexer.peek_next();
	while (state != 8 && tmp.get_type() != TypeEnum::SEMICN)
	{
		tmp = lexer.get_next();
		switch (tmp.get_type())
		{
		case TypeEnum::INTTK:
		case TypeEnum::CHARTK:
			if (state == 0)
			{
				state = 1;
				define.push_back(tmp);
				break;
			}
			else
			{
				return lexer.get_pos();
			}
		case TypeEnum::IDENFR:
			if (state == 1 || state == 6)
			{
				state = 2;
				define.push_back(tmp);
				if (lexer.peek_next().get_type() == TypeEnum::LPARENT)
					return lexer.get_pos();				//not a VariableDingYi
				if (lexer.peek_next().get_type() != TypeEnum::LBRACK
					&& lexer.peek_next().get_type() != TypeEnum::ASSIGN
					&& lexer.peek_next().get_type() != TypeEnum::COMMA)
				{
					type = 1;
					check_initial();
					add_to_table();
					return -1;
				}
				break;
			}
			else
			{
				return lexer.get_pos();
			}
		case TypeEnum::LBRACK:
			if (state == 2)
			{
				state = 3;
				define.push_back(tmp);
				dimension = 1;
				break;
			}
			else if (state == 5)
			{
				state = 3;
				define.push_back(tmp);
				dimension = 2;
				break;
			}
			else
			{
				return lexer.get_pos();
			}
		case TypeEnum::INTCON:
			if (state == 3)
			{
				state = 4;
				define.push_back(empty);
				WuFuHaoShu shu(tmp);
				zheng_shu_s.push_back(shu);
				WordInfo wi = lexer.peek_next();
				if (wi.get_type() != TypeEnum::RBRACK)
				{
					// Wu Zhong Kuo Hao
					ErrorTable::log_error(tmp.get_line(), "m");
					state = 5;
					if (wi.get_type() != TypeEnum::ASSIGN
						&& wi.get_type() != TypeEnum::COMMA
						&& wi.get_type() != TypeEnum::LBRACK)
					{
						type = 1;
						check_initial();
						add_to_table();
						return -1;
					}
				}
				break;
			}
			else
			{
				return lexer.get_pos();
			}
		case TypeEnum::RBRACK:
			if (state == 4)
			{
				state = 5;
				define.push_back(tmp);
				if (lexer.peek_next().get_type() != TypeEnum::ASSIGN
					&& lexer.peek_next().get_type() != TypeEnum::COMMA
					&& lexer.peek_next().get_type() != TypeEnum::LBRACK)
				{
					type = 1;
					check_initial();
					add_to_table();
					return -1;
				}
				break;
			}
			else 
			{
				return lexer.get_pos();
			}
		case TypeEnum::COMMA:
			if (state == 2 || state == 5)
			{
				state = 6;
				type = 1;
				define.push_back(tmp);
				break;
			}
			else
			{
				return lexer.get_pos();
			}
		case TypeEnum::ASSIGN:
			if (type == 1)	//already have "," ; cannot initial
			{
				return lexer.get_pos();
			}
			if (state == 2 || state == 5)
			{
				state = 8;
				type = 2;
				define.push_back(tmp);
				break;
			}
			else
			{
				return lexer.get_pos();
			}
		default:
			return lexer.get_pos();
		}//end switch
		tmp = lexer.peek_next();
	}//end while
	if (state != 8)	//without initialize
	{
		type = 1;
		add_to_table();
		return -1;
	}
	type = 2;		//read in initialize
	tmp = lexer.peek_next();
	int braces = 0;
	while (1)
	{
		switch (tmp.get_type())
		{
			case TypeEnum::LBRACE:
				braces++;			
				tmp = lexer.get_next();
				initial.push_back(tmp);
				break;
			case TypeEnum::RBRACE:
				braces--;
				tmp = lexer.get_next();
				initial.push_back(tmp);
				break;
			case TypeEnum::COMMA:
				tmp = lexer.get_next();	
				initial.push_back(tmp);
				break;
			default:				
				initial.push_back(empty);//set the position for ChangLiang
				ChangLiang chang;
				int r;
				if ((r = chang.read_in(lexer)) != -1)
					return r;
				chang_liang_s.push_back(chang);
				break;
		}//end switch
		if (braces == 0)
			break;
		tmp = lexer.peek_next();
	}//end while
	check_initial();
	add_to_table();
	return -1;
}

string VariableDingYi::to_string()
{
	string for_return;
	for_return.clear();
	int i;
	int j = 0;
	for (i = 0; i < define.size(); i++)
	{
		if (define[i].get_type() == TypeEnum::EMPTY)
		{
			for_return += zheng_shu_s[j].to_string();
			j++;
		}
		else
		{
			for_return += define[i].to_string();
		}
	}
	if (type == 1)
	{
		for_return += "<变量定义无初始化>\n<变量定义>\n";
		return for_return;
	}
	j = 0;
	for (i = 0; i < initial.size(); i++)
	{
		if (initial[i].get_type() == TypeEnum::EMPTY)
		{
			for_return += chang_liang_s[j].to_string();
			j++;
		}
		else
		{
			for_return += initial[i].to_string();
		}
	}
	for_return += "<变量定义及初始化>\n<变量定义>\n";
	return for_return;
}

int VariableShuoMing::read_in(Lexer& lexer)
{
	word_pos = lexer.get_pos();
	int r = -1;
	int count = 0;
	int tmp_pos = 0;
	while (1)
	{
		VariableDingYi tmp;
		tmp_pos = lexer.get_pos();
		int line_count = lexer.peek_next().get_line();
		WordInfo pre_read = lexer.peek_next();
		if (pre_read.get_type() != TypeEnum::CHARTK
			&& pre_read.get_type() != TypeEnum::INTTK)		//cannot be a variable ding yi
			break;
		if ((r = tmp.read_in(lexer) != -1))
			break;
		ding_yi_s.push_back(tmp);
		WordInfo wi = lexer.peek_next();
		if (wi.get_type() == TypeEnum::SEMICN)
			semicns.push_back(lexer.get_next()); 
		else
		{
			// Wu Fen Hao
			ErrorTable::log_error(line_count, "k");
		}
		count++;
	}
	if (count == 0)		//read in none
	{
		lexer.set_pos(word_pos);
		return r;
	}
	else				//read in success
	{
		lexer.set_pos(tmp_pos);
		return -1;
	}
}

string VariableShuoMing::to_string()
{
	string for_return;
	for_return.clear();
	int i;
	for (i = 0; i < ding_yi_s.size(); i++)
	{
		for_return += ding_yi_s[i].to_string();
		for_return += semicns[i].to_string();
	}
	for_return += "<变量说明>\n";
	return for_return;
}

int SentenceReturn::read_in(Lexer& lexer)
{
	word_pos = lexer.get_pos();
	WordInfo tmp = lexer.get_next();
	if (tmp.get_type() != TypeEnum::RETURNTK)
	{
		return lexer.get_pos();				//an error
	}
	items.push_back(tmp);
	tmp = lexer.peek_next();
	
	if (tmp.get_type() == TypeEnum::LPARENT)
	{
		type = 1;
		tmp = lexer.get_next();
		items.push_back(tmp);
		if (lexer.peek_next().get_type() == TypeEnum::RPARENT)
		{
			type = 0;
			items.push_back(lexer.get_next());
			return -1;							//got a wrong have return
		}
		int r;
		if ((r = expression.read_in(lexer)) != -1)
			return r;						//read expression fail
		if (lexer.peek_next().get_type() != TypeEnum::RPARENT)
		{
			// Wu Xiao Kuo Hao
			ErrorTable::log_error(tmp.get_line(), "l");
		}
		else
		{
			items.push_back(lexer.get_next());
		}
		return -1;							//got a have return
	}
	else
	{
		type = 2;
		return -1;							//got a none return
	}
}

vector<SentenceReturn*> SentenceReturn::get_all_return()
{
	vector<SentenceReturn*> for_return;
	for_return.clear();
	for_return.push_back(this);
	return for_return;
}

string SentenceReturn::to_string()
{
	string for_return;
	for_return.clear();
	for_return += items[0].to_string();
	if (type == 2)
	{
		for_return += "<返回语句>\n";
		return for_return;
	}
	for_return += items[1].to_string();
	for_return += expression.to_string();
	for_return += items[2].to_string();
	for_return += "<返回语句>\n";
	return for_return;
}

int SentencePrint::read_in(Lexer& lexer)
{
	word_pos = lexer.get_pos();
	WordInfo tmp = lexer.get_next();
	if (tmp.get_type() != TypeEnum::PRINTFTK)		//"printf"
	{
		return lexer.get_pos();						//an error
	}
	items.push_back(tmp);
	tmp = lexer.get_next();
	if (tmp.get_type() != TypeEnum::LPARENT)		//"("
	{
		return lexer.get_pos();
	}
	items.push_back(tmp);
	tmp = lexer.peek_next();
	int r = -1;
	if (tmp.get_type() == TypeEnum::STRCON)
	{
		r = zi_fu.read_in(lexer);					//string; checked
		tmp = lexer.peek_next();
		if (tmp.get_type() == TypeEnum::COMMA)		//have expression
		{
			items.push_back(lexer.get_next());
			r = expression.read_in(lexer);
			if (r != -1)
				return r;
			type = 1;
		}
		else
			type = 2;								//only zi fu
	}
	else
	{
		r = expression.read_in(lexer);				//only expression
		if (r != -1)
			return r;
		type = 3;
	}
	if (lexer.peek_next().get_type() != TypeEnum::RPARENT)
	{
		// Wu Xiao Kuo Hao
		ErrorTable::log_error(tmp.get_line(), "l");
	}
	else
	{
		items.push_back(lexer.get_next());
	}
	return -1;
}

vector<SentenceReturn*> SentencePrint::get_all_return()
{
	vector<SentenceReturn*> for_return;
	for_return.clear();
	return for_return;
}

string SentencePrint::to_string()
{
	string for_return;
	for_return.clear();
	int i = 0;
	for_return += items[i].to_string();
	i++;
	for_return += items[i].to_string();
	i++;
	if (type == 1)
	{
		for_return += zi_fu.to_string();
		for_return += items[i].to_string();
		i++;
		for_return += expression.to_string();
	}
	else if (type == 2)
		for_return += zi_fu.to_string();
	else if (type == 3)
		for_return += expression.to_string();
	for_return += items[i].to_string();
	i++;
	for_return += "<写语句>\n";
	return for_return;
}

int SentenceRead::read_in(Lexer& lexer)
{
	word_pos = lexer.get_pos();
	WordInfo tmp = lexer.get_next();			//"scanf"
	if (tmp.get_type() != TypeEnum::SCANFTK)
		return lexer.get_pos();		//error
	items.push_back(tmp);
	tmp = lexer.get_next();						//"("
	if (tmp.get_type() != TypeEnum::LPARENT)
		return lexer.get_pos();		//error
	items.push_back(tmp);
	tmp = lexer.get_next();						//identifier
	if (tmp.get_type() != TypeEnum::IDENFR)
		return lexer.get_pos();		//error
	if (!IdentifyTable::have_var_const(&tmp))
	{
		// Wei Ding Yi
		ErrorTable::log_error(tmp.get_line(), "c");
	}
	else if (IdentifyTable::get_property_by_name(&tmp) == IdentifyProperty::CONST)
	{
		// Chang Liang Fu Zhi
		ErrorTable::log_error(tmp.get_line(), "j");
	}
	items.push_back(tmp);
	if (lexer.peek_next().get_type() != TypeEnum::RPARENT)
	{
		// Wu Xiao Kuo Hao
		ErrorTable::log_error(tmp.get_line(), "l");
	}
	else
	{
		items.push_back(lexer.get_next());
	}
	return -1;
}

vector<SentenceReturn*> SentenceRead::get_all_return()
{
	vector<SentenceReturn*> for_return;
	for_return.clear();
	return for_return;
}

string SentenceRead::to_string()
{
	int i;
	string for_return;
	for_return.clear();
	for (i = 0; i < items.size(); i++)
	{
		for_return += items[i].to_string();
	}
	for_return += "<读语句>\n";
	return for_return;
}

int SentenceLie::read_in(Lexer& lexer)
{
	word_pos = lexer.get_pos();
	int tmp_pos = word_pos;
	Sentence* tmp = new Sentence;
	int r = tmp->read_in(lexer);
	while (r == -1)		//TODO: check number of sentence
	{
		tmp_pos = lexer.get_pos();
		sentences.push_back(*tmp);
		tmp = new Sentence;
		r = tmp->read_in(lexer);
	}
	lexer.set_pos(tmp_pos);
	return -1;
}

vector<SentenceReturn*> SentenceLie::get_all_return()
{
	vector<SentenceReturn*> for_return;
	for_return.clear();
	unsigned int i;
	for (i = 0; i < sentences.size(); i++)
	{
		vector<SentenceReturn*> this_st = sentences[i].get_all_return();
		for_return.insert(for_return.end(), this_st.begin(), this_st.end());
	}
	return for_return;
}

string SentenceLie::to_string()
{
	string for_return;
	for_return.clear();
	for (int i = 0; i < sentences.size(); i++)
		for_return += sentences[i].to_string();
	for_return += "<语句列>\n";
	return for_return;
}

int ParameterValue::read_in(Lexer& lexer)
{
	int r;
	word_pos = lexer.get_pos();
	WordInfo tmp = lexer.peek_next();
	if (tmp.get_type() == TypeEnum::RPARENT)		//is empty
	{
		expressions.clear();
		commas.clear();
		return -1;
	}
	else if (tmp.get_type() == TypeEnum::SEMICN
		|| tmp.get_type() == TypeEnum::MULT
		|| tmp.get_type() == TypeEnum::DIV)			// no RPARENT error
	{
		expressions.clear();
		commas.clear();
		return -1;
	}
	while (1)
	{
		Expression* e = new Expression;
		r = e->read_in(lexer);
		if (r != -1)
			return r;
		expressions.push_back(*e);
		tmp = lexer.peek_next();
		if (tmp.get_type() != TypeEnum::COMMA)
			break;
		tmp = lexer.get_next();
		commas.push_back(tmp);
	}
	return -1;
}

string ParameterValue::to_string()
{
	string for_return;
	for_return.clear();
	for (int i = 0; i < expressions.size(); i++)
	{
		for_return += expressions[i].to_string();
		if (i < commas.size())
			for_return += commas[i].to_string();
	}
	for_return += "<值参数表>\n";
	return for_return;
}

int SentenceDiaoYong::read_in(Lexer& lexer)
{
	word_pos = lexer.get_pos();
	WordInfo tmp = lexer.get_next();		//read id
	if (tmp.get_type() != TypeEnum::IDENFR)
		return lexer.get_pos();
	items.push_back(tmp);
	if (!IdentifyTable::have_func(&tmp))
	{
		// Wei Ding Yi
		ErrorTable::log_error(tmp.get_line(), "c");
		type = 0;
	}
	else
	{
		if (IdentifyTable::have_return(&tmp))
			type = 1;
		else
			type = 2;
	}
	tmp = lexer.get_next();				//read (
	if (tmp.get_type() != TypeEnum::LPARENT)
		return lexer.get_pos();
	items.push_back(tmp);
	int r = paras.read_in(lexer);		//read values
	if (r != -1)
		return r;

	if (!IdentifyTable::check_func_para_num(&(items[0]), &paras))
	{
		// Can Shu Ge Shu
		ErrorTable::log_error(items[0].get_line(), "d");
	}
	else if (!IdentifyTable::check_func_para_type(&(items[0]), &paras))
	{
		// Can Shu Lei Xing
		ErrorTable::log_error(items[0].get_line(), "e");
	}

	if (lexer.peek_next().get_type() != TypeEnum::RPARENT)
	{
		// Wu Xiao Kuo Hao
		ErrorTable::log_error(tmp.get_line(), "l");
	}
	else
	{
		items.push_back(lexer.get_next());
	}
	return -1;
}

string SentenceDiaoYong::to_string()
{
	string for_return;
	for_return.clear();
	for_return += items[0].to_string();
	for_return += items[1].to_string();
	for_return += paras.to_string();
	for_return += items[2].to_string();
	if (type == 1)
		for_return += "<有返回值函数调用语句>\n";
	else if (type == 2)
		for_return += "<无返回值函数调用语句>\n";
	return for_return;
}

int ConditionDefault::read_in(Lexer& lexer)
{
	sentence = new Sentence;
	word_pos = lexer.get_pos();
	default_tk = lexer.peek_next();		//read default
	if (default_tk.get_type() != TypeEnum::DEFAULTTK)
	{
		// Wu Que Sheng
		ErrorTable::log_error(default_tk.get_line(), "p");
		return -1;
	}
	else
	{
		lexer.get_next();
	}
	colon = lexer.get_next();			//read :
	if (colon.get_type() != TypeEnum::COLON)
		return lexer.get_pos();
	int r = sentence->read_in(lexer);	//read sentence
	return r;
}

vector<SentenceReturn*> ConditionDefault::get_all_return()
{
	return sentence->get_all_return();
}

string ConditionDefault::to_string()
{
	string for_return;
	for_return.clear();
	for_return += default_tk.to_string();
	for_return += colon.to_string();
	for_return += sentence->to_string();
	for_return += "<缺省>\n";
	return for_return;
}

int ConditionCase::read_in(Lexer& lexer, IdentifyType target)
{
	sentence = new Sentence;
	word_pos = lexer.get_pos();
	case_tk = lexer.get_next();							//read case
	if (case_tk.get_type() != TypeEnum::CASETK)
		return lexer.get_pos();
	int r = chang_liang.read_in(lexer);					//read chang liang
	if (r != -1)
		return -1;
	if (chang_liang.get_type() != target)
	{
		// Chang Liang Lei Xing
		ErrorTable::log_error(case_tk.get_line(), "o");
	}
	colon = lexer.get_next();							//read :
	if (colon.get_type() != TypeEnum::COLON)
		return lexer.get_pos();
	r = sentence->read_in(lexer);						//read sentence
	return r;
}

vector<SentenceReturn*> ConditionCase::get_all_return()
{
	return sentence->get_all_return();
}

string ConditionCase::to_string()
{
	string for_return;
	for_return.clear();
	for_return += case_tk.to_string();
	for_return += chang_liang.to_string();
	for_return += colon.to_string();
	for_return += sentence->to_string();
	for_return += "<情况子语句>\n";
	return for_return;
}

int ConditionTable::read_in(Lexer& lexer, IdentifyType target)
{
	word_pos = lexer.get_pos();
	WordInfo tmp = lexer.peek_next();
	while (tmp.get_type() == TypeEnum::CASETK)
	{
		ConditionCase now;
		int r = now.read_in(lexer, target);
		if (r != -1)
			return r;
		cases.push_back(now);
		tmp = lexer.peek_next();
	}
	if (cases.size() == 0)
		return lexer.get_pos();
	return -1;
}

vector<SentenceReturn*> ConditionTable::get_all_return()
{
	vector<SentenceReturn*> for_return;
	for_return.clear();
	unsigned int i;
	for (i = 0; i < cases.size(); i++)
	{
		vector<SentenceReturn*> this_case = cases[i].get_all_return();
		for_return.insert(for_return.end(), this_case.begin(), this_case.end());
	}
	return for_return;
}

string ConditionTable::to_string()
{
	string for_return;
	for_return.clear();
	for (int i = 0; i < cases.size(); i++)
	{
		for_return += cases[i].to_string();
	}
	for_return += "<情况表>\n";
	return for_return;
}

int SentenceQingKuang::read_in(Lexer& lexer)
{
	word_pos = lexer.get_pos();
	WordInfo tmp = lexer.get_next();				//read switch
	if (tmp.get_type() != TypeEnum::SWITCHTK)
		return lexer.get_pos();
	items.push_back(tmp);
	tmp = lexer.get_next();							//read (
	if (tmp.get_type() != TypeEnum::LPARENT)
		return lexer.get_pos();
	items.push_back(tmp);
	int r = expression.read_in(lexer);				//read expression
	if (r != -1)
		return r;
	IdentifyType target_type = expression.check_type();
	if (lexer.peek_next().get_type() != TypeEnum::RPARENT)
	{												// read ')'
		// Wu Xiao Kuo Hao
		ErrorTable::log_error(tmp.get_line(), "l");
	}
	else
	{
		items.push_back(lexer.get_next());
	}
	tmp = lexer.get_next();							//read {
	if (tmp.get_type() != TypeEnum::LBRACE)
		return lexer.get_pos();
	items.push_back(tmp);
	r = table.read_in(lexer, target_type);			//read table
	if (r != -1)
		return r;
	int line_count = lexer.peek_next().get_line();
	r = def.read_in(lexer);							//read default
	if (r != -1)
		return r;
	tmp = lexer.get_next();							//read }
	if (tmp.get_type() != TypeEnum::RBRACE)
		return lexer.get_pos();
	items.push_back(tmp);
	return -1;
}

vector<SentenceReturn*> SentenceQingKuang::get_all_return()
{
	vector<SentenceReturn*> for_return;
	for_return.clear();
	vector<SentenceReturn*> table_return = table.get_all_return();
	vector<SentenceReturn*> def_return = def.get_all_return();
	for_return.insert(for_return.end(), table_return.begin(), table_return.end());
	for_return.insert(for_return.end(), def_return.begin(), def_return.end());
	return for_return;
}

string SentenceQingKuang::to_string()
{
	string for_return;
	for_return.clear();
	for_return += items[0].to_string();
	for_return += items[1].to_string();
	for_return += expression.to_string();
	for_return += items[2].to_string();
	for_return += items[3].to_string();
	for_return += table.to_string();
	for_return += def.to_string();
	for_return += items[4].to_string();
	for_return += "<情况语句>\n";
	return for_return;
}

int BuChang::read_in(Lexer& lexer)
{
	word_pos = lexer.get_pos();
	return zheng_shu.read_in(lexer);
}

string BuChang::to_string()
{
	return zheng_shu.to_string() + "<步长>\n";
}

int SentenceXunHuan::read_in(Lexer& lexer)
{
	word_pos = lexer.get_pos();
	WordInfo tmp = lexer.get_next();
	sentence = new Sentence;
	if (tmp.get_type() == TypeEnum::WHILETK)			//is while
	{
		items.push_back(tmp);
		type = 1;
		tmp = lexer.get_next();							//read (
		if (tmp.get_type() != TypeEnum::LPARENT)
			return lexer.get_pos();
		items.push_back(tmp);
		int r = tiao_jian.read_in(lexer);				//read tiao jian
		if (r != -1)
			return r;
		if (lexer.peek_next().get_type() != TypeEnum::RPARENT)
		{												// read ')'
			// Wu Xiao Kuo Hao
			ErrorTable::log_error(tmp.get_line(), "l");
		}
		else
		{
			items.push_back(lexer.get_next());
		}
		return sentence->read_in(lexer);				//read sentence
	}
	else if (tmp.get_type() == TypeEnum::FORTK)			//is for
	{
		items.push_back(tmp);
		type = 2;
		tmp = lexer.get_next();							//read (
		if (tmp.get_type() != TypeEnum::LPARENT)
			return lexer.get_pos();
		items.push_back(tmp);
		tmp = lexer.get_next();							//read id
		if (tmp.get_type() != TypeEnum::IDENFR)
			return lexer.get_pos();
		if (!IdentifyTable::have_var_const(&tmp))
		{
			// Wei Ding Yi
			ErrorTable::log_error(tmp.get_line(), "c");
		}
		else if (IdentifyTable::get_property_by_name(&tmp) == IdentifyProperty::CONST)
		{
			// Chang Liang Fu Zhi
			ErrorTable::log_error(tmp.get_line(), "j");
		}
		items.push_back(tmp);
		tmp = lexer.get_next();							//read =
		if (tmp.get_type() != TypeEnum::ASSIGN)
			return lexer.get_pos();
		items.push_back(tmp);
		int r = expression.read_in(lexer);				//read expression
		if (r != -1)
			return -1;
		tmp = lexer.peek_next();						//read ;
		if (tmp.get_type() != TypeEnum::SEMICN)
		{
			// Wu Fen Hao
			ErrorTable::log_error(tmp.get_line(), "k");
		}
		else
		{
			items.push_back(lexer.get_next());
		}
		r = tiao_jian.read_in(lexer);					//read tiao jian
		if (r != -1)
			return r;
		tmp = lexer.peek_next();						//read ;
		if (tmp.get_type() != TypeEnum::SEMICN)
		{
			// Wu Fen Hao
			ErrorTable::log_error(tmp.get_line(), "k");
		}
		else
		{
			items.push_back(lexer.get_next());
		}
		tmp = lexer.get_next();							//read id
		if (tmp.get_type() != TypeEnum::IDENFR)
			return lexer.get_pos();
		if (!IdentifyTable::have_var_const(&tmp))
		{
			// Wei Ding Yi
			ErrorTable::log_error(tmp.get_line(), "c");
		}
		items.push_back(tmp);
		tmp = lexer.get_next();							//read =
		if (tmp.get_type() != TypeEnum::ASSIGN)
			return lexer.get_pos();
		items.push_back(tmp);
		tmp = lexer.get_next();							//read id
		if (tmp.get_type() != TypeEnum::IDENFR)
			return lexer.get_pos();
		if (!IdentifyTable::have_var_const(&tmp))
		{
			// Wei Ding Yi
			ErrorTable::log_error(tmp.get_line(), "c");
		}
		items.push_back(tmp);
		tmp = lexer.get_next();							//read +|-
		if (tmp.get_type() != TypeEnum::PLUS && tmp.get_type() != TypeEnum::MINU)
			return lexer.get_pos();
		items.push_back(tmp);
		r = bu_chang.read_in(lexer);					//read bu chang
		if (r != -1)
			return r;
		if (lexer.peek_next().get_type() != TypeEnum::RPARENT)
		{												// read ')'
			// Wu Xiao Kuo Hao
			ErrorTable::log_error(tmp.get_line(), "l");
		}
		else
		{
			items.push_back(lexer.get_next());
		}
		return sentence->read_in(lexer);				//read sentence
	}
	else
	{
		return lexer.get_pos();
	}
}

vector<SentenceReturn*> SentenceXunHuan::get_all_return()
{
	return sentence->get_all_return();
}

string SentenceXunHuan::to_string()
{
	string for_return;
	for_return.clear();
	if (type == 1)
	{
		for_return += items[0].to_string();
		for_return += items[1].to_string();
		for_return += tiao_jian.to_string();
		for_return += items[2].to_string();
		for_return += sentence->to_string();
	}
	else if (type == 2)
	{
		for_return += items[0].to_string();
		for_return += items[1].to_string();
		for_return += items[2].to_string();
		for_return += items[3].to_string();
		for_return += expression.to_string();
		for_return += items[4].to_string();
		for_return += tiao_jian.to_string();
		for_return += items[5].to_string();
		for_return += items[6].to_string();
		for_return += items[7].to_string();
		for_return += items[8].to_string();
		for_return += items[9].to_string();
		for_return += bu_chang.to_string();
		for_return += items[10].to_string();
		for_return += sentence->to_string();
	}
	for_return += "<循环语句>\n";
	return for_return;
}

int TiaoJian::read_in(Lexer& lexer)
{
	word_pos = lexer.get_pos();
	int r = e_left.read_in(lexer);
	if (r != -1)
		return r;
	relation = lexer.get_next();
	TypeEnum type = relation.get_type();
	if (type != TypeEnum::EQL && type != TypeEnum::LEQ && type != TypeEnum::LSS
		&& type != TypeEnum::GEQ && type != TypeEnum::GRE && type != TypeEnum::NEQ)
	{
		return lexer.get_pos();
	}
	r =  e_right.read_in(lexer);
	if (r != -1)
		return r;
	if (e_left.check_type() != IdentifyType::INT
		|| e_right.check_type() != IdentifyType::INT)
	{
		// Tiao Jian 
		ErrorTable::log_error(relation.get_line(), "f");
	}
	return -1;
}

string TiaoJian::to_string()
{
	string for_return;
	for_return.clear();
	for_return += e_left.to_string();
	for_return += relation.to_string();
	for_return += e_right.to_string();
	for_return += "<条件>\n";
	return for_return;
}

int SentenceTiaoJian::read_in(Lexer& lexer)
{
	s_if = new Sentence;
	s_else = new Sentence;
	word_pos = lexer.get_pos();
	WordInfo tmp = lexer.get_next();			//read if
	if (tmp.get_type() != TypeEnum::IFTK)
		return lexer.get_pos();
	items.push_back(tmp);
	tmp = lexer.get_next();						//read (
	if (tmp.get_type() != TypeEnum::LPARENT)
		return lexer.get_pos();
	items.push_back(tmp);
	int r = tiao_jian.read_in(lexer);			//read tiao jian
	if (r != -1)
		return r;
	if (lexer.peek_next().get_type() != TypeEnum::RPARENT)
	{												// read ')'
		// Wu Xiao Kuo Hao
		ErrorTable::log_error(tmp.get_line(), "l");
	}
	else
	{
		items.push_back(lexer.get_next());
	}
	r = s_if->read_in(lexer);					//read if sentence
	if (r != -1)
		return -1;
	tmp = lexer.peek_next();					//check else
	if (tmp.get_type() != TypeEnum::ELSETK)
	{
		have_else = false;
		return -1;
	}
	have_else = true;
	tmp = lexer.get_next();						//read else
	items.push_back(tmp);
	return s_else->read_in(lexer);
}

vector<SentenceReturn*> SentenceTiaoJian::get_all_return()
{
	vector<SentenceReturn*> for_return;
	for_return.clear();
	vector<SentenceReturn*> if_return = s_if->get_all_return();
	for_return.insert(for_return.end(), if_return.begin(), if_return.end());
	if (have_else)
	{
		vector<SentenceReturn*> else_return = s_else->get_all_return();
		for_return.insert(for_return.end(), else_return.begin(), else_return.end());
	}
	return for_return;
}

string SentenceTiaoJian::to_string()
{
	string for_return;
	for_return.clear();
	for_return += items[0].to_string();
	for_return += items[1].to_string();
	for_return += tiao_jian.to_string();
	for_return += items[2].to_string();
	for_return += s_if->to_string();
	if (have_else)
	{
		for_return += items[3].to_string();
		for_return += s_else->to_string();
	}
	for_return += "<条件语句>\n";
	return for_return;
}

int SentenceFuZhi::read_in(Lexer& lexer)
{
	word_pos = lexer.get_pos();
	WordInfo tmp = lexer.peek_next();
	WordInfo empty(TypeEnum::EMPTY, 0, 0);
	while (tmp.get_type() != TypeEnum::ASSIGN)
	{
		switch (tmp.get_type())						
		{
			case TypeEnum::IDENFR:
				tmp = lexer.get_next();
				items.push_back(tmp);
				if (!IdentifyTable::have_var_const(&tmp))
				{
					// Wei Ding Yi
					ErrorTable::log_error(tmp.get_line(), "c");
				}
				else if (IdentifyTable::get_property_by_name(&tmp) == IdentifyProperty::CONST)
				{
					// Chang Liang Fu Zhi
					ErrorTable::log_error(tmp.get_line(), "j");
				}
				break;
			case TypeEnum::LBRACK:
			{
				tmp = lexer.get_next();
				items.push_back(tmp);
				items.push_back(empty);
				Expression e;
				int r = e.read_in(lexer);
				if (r != -1)
					return r;
				if (e.check_type() != IdentifyType::INT)
				{
					// Shu Zu Xia Biao
					ErrorTable::log_error(tmp.get_line(), "i");
				}
				exps.push_back(e);
				if (lexer.peek_next().get_type() != TypeEnum::RBRACK)
				{
					// Wu Zhong Kuo Hao
					ErrorTable::log_error(tmp.get_line(), "m");
				}
				else
				{
					items.push_back(lexer.get_next());
				}
				break;
			}
		}
		tmp = lexer.peek_next();
	}
	tmp = lexer.get_next();
	items.push_back(tmp);
	items.push_back(empty);
	Expression right;
	int r = right.read_in(lexer);
	if (r != -1)
		return r;
	exps.push_back(right);
	return -1;
}

vector<SentenceReturn*> SentenceFuZhi::get_all_return()
{
	vector<SentenceReturn*> for_return;
	for_return.clear();
	return for_return;
}

string SentenceFuZhi::to_string()
{
	string for_return;
	for_return.clear();
	int j = 0;
	for (int i = 0; i < items.size(); i++)
	{
		if (items[i].get_type() != TypeEnum::EMPTY)
			for_return += items[i].to_string();
		else
		{
			for_return += exps[j].to_string();
			j++;
		}
	}
	for_return += "<赋值语句>\n";
	return for_return;
}

int Sentence::read_in(Lexer& lexer)
{
	word_pos = lexer.get_pos();
	WordInfo pre_read = lexer.peek_next();
	int r = -1;
	switch (pre_read.get_type())
	{
		case TypeEnum::SEMICN:
			type = 0;							//empty
			semicn = lexer.get_next();
			break;
		case TypeEnum::WHILETK:
		case TypeEnum::FORTK:
			type = 1;							//xun huan
			r = xun_huan.read_in(lexer);
			if (r != -1)
				return r;
			break;
		case TypeEnum::IFTK:
			type = 2;							//tiao jian
			r = tiao_jian.read_in(lexer);
			if (r != -1)
				return r;
			break;
		case TypeEnum::IDENFR:
			lexer.get_next();
			pre_read = lexer.peek_next();
			if (pre_read.get_type() == TypeEnum::LPARENT)
			{
				type = 3;						//han shu diao yong
				lexer.set_pos(word_pos);
				r = diao_yong.read_in(lexer);
				if (r != -1)
					return r;
			}
			else
			{
				type = 4;						//fu zhi
				lexer.set_pos(word_pos);
				r = fu_zhi.read_in(lexer);
				if (r != -1)
					return r;
			}
			semicn = lexer.peek_next();
			if (semicn.get_type() != TypeEnum::SEMICN)
			{
				// Wu Fen Hao
				ErrorTable::log_error(pre_read.get_line(), "k");
			}
			else
			{
				lexer.get_next();
			}
			break;
		case TypeEnum::SCANFTK:
			type = 5;							//scanf
			r = read.read_in(lexer);
			if (r != -1)
				return r;
			semicn = lexer.peek_next();
			if (semicn.get_type() != TypeEnum::SEMICN)
			{
				// Wu Fen Hao
				ErrorTable::log_error(pre_read.get_line(), "k");
			}
			else
			{
				lexer.get_next();
			}
			break;
		case TypeEnum::PRINTFTK:
			type = 6;							//printf
			r = print.read_in(lexer);
			if (r != -1)
				return r;
			semicn = lexer.peek_next();
			if (semicn.get_type() != TypeEnum::SEMICN)
			{
				// Wu Fen Hao
				ErrorTable::log_error(pre_read.get_line(), "k");
			}
			else
			{
				lexer.get_next();
			}
			break;
		case TypeEnum::RETURNTK:
			type = 7;							//return
			r = fan_hui.read_in(lexer);
			if (r != -1)
				return r;
			semicn = lexer.peek_next();
			if (semicn.get_type() != TypeEnum::SEMICN)
			{
				// Wu Fen Hao
				ErrorTable::log_error(pre_read.get_line(), "k");
			}
			else
			{
				lexer.get_next();
			}
			break;
		case TypeEnum::SWITCHTK:
			type = 8;							//qing kuang
			r = qing_kuang.read_in(lexer);
			if (r != -1)
				return r;
			break;
		case TypeEnum::LBRACE:
		{
			type = 9;							//yu ju lie
			lbrace = lexer.get_next();
			yu_ju_lie = new SentenceLie;
			r = yu_ju_lie->read_in(lexer);
			if (r != -1)
				return r;
			rbrace = lexer.get_next();
			if (rbrace.get_type() != TypeEnum::RBRACE)
				return lexer.get_pos();
			break;
		}
		default:
			return lexer.get_pos();
	}
	return -1;
}

vector<SentenceReturn*> Sentence::get_all_return()
{
	switch (type)
	{
		case 1:
			return xun_huan.get_all_return();
			break;
		case 2:
			return tiao_jian.get_all_return();
			break;
		case 7:
			return fan_hui.get_all_return();
			break;
		case 8:
			return qing_kuang.get_all_return();
			break;
		case 9:
			return yu_ju_lie->get_all_return();
			break;
		default:
			vector<SentenceReturn*> for_return;
			for_return.clear();
			return for_return;
	}
}

string Sentence::to_string()
{
	string for_return;
	for_return.clear();
	switch (type)
	{
		case 0:
			for_return += semicn.to_string();
			break;
		case 1:
			for_return += xun_huan.to_string();
			break;
		case 2:
			for_return += tiao_jian.to_string();
			break;
		case 3:
			for_return += diao_yong.to_string();
			for_return += semicn.to_string();
			break;
		case 4:
			for_return += fu_zhi.to_string();
			for_return += semicn.to_string();
			break;
		case 5:
			for_return += read.to_string();
			for_return += semicn.to_string();
			break;
		case 6:
			for_return += print.to_string();
			for_return += semicn.to_string();
			break;
		case 7:
			for_return += fan_hui.to_string();
			for_return += semicn.to_string();
			break;
		case 8:
			for_return += qing_kuang.to_string();
			break;
		case 9:
			for_return += lbrace.to_string();
			for_return += yu_ju_lie->to_string();
			for_return += rbrace.to_string();
			break;
	}
	for_return += "<语句>\n";
	return for_return;
}

int Factor::read_in(Lexer& lexer)
{
	word_pos = lexer.get_pos();
	WordInfo pre_read = lexer.peek_next();
	int r;
	WordInfo empty(TypeEnum::EMPTY, 0, 0);
	switch (pre_read.get_type())
	{
		case TypeEnum::PLUS:
		case TypeEnum::MINU:
		case TypeEnum::INTCON:
			type = 2;
			r = zheng_shu.read_in(lexer);
			if (r != -1)
				return r;
			break;
		case TypeEnum::CHARCON:
			type = 3;
			zi_fu = lexer.get_next();
			break;
		case TypeEnum::LPARENT:
		{
			type = 1;
			pre_read = lexer.get_next();
			ids.push_back(pre_read);
			Expression e;
			r = e.read_in(lexer);
			if (r != -1)
				return r;
			exps.push_back(e);
			ids.push_back(empty);
			if (lexer.peek_next().get_type() != TypeEnum::RPARENT)
			{
				// WU Xiao Kuo Hao
				ErrorTable::log_error(pre_read.get_line(), "l");
			}
			else
			{
				ids.push_back(lexer.get_next());
			}
			break; 
		}
		case TypeEnum::IDENFR:
			lexer.get_next();
			pre_read = lexer.peek_next();
			if (pre_read.get_type() == TypeEnum::LPARENT)
			{
				type = 4;
				lexer.set_pos(word_pos);
				diao_yong = new SentenceDiaoYong;
				r = diao_yong->read_in(lexer);
				if (r != -1)
					return r;
				break;
			}
			else
			{
				type = 1;
				lexer.set_pos(word_pos);
				pre_read = lexer.get_next();
				ids.push_back(pre_read);
				if (!IdentifyTable::have_var_const(&pre_read))
				{
					// Wei Ding Yi
					ErrorTable::log_error(pre_read.get_line(), "c");
				}
				pre_read = lexer.peek_next();
				while (pre_read.get_type() == TypeEnum::LBRACK)
				{
					ids.push_back(lexer.get_next());
					Expression e;
					r = e.read_in(lexer);
					if (r != -1)
						return r;
					exps.push_back(e);
					if (e.check_type() != IdentifyType::INT)
					{
						// Shu Zu XIa Biao
						ErrorTable::log_error(pre_read.get_line(), "i");
					}
					ids.push_back(empty);
					if (lexer.peek_next().get_type() != TypeEnum::RBRACK)
					{
						// Wu Zhong Kuo Hao
						ErrorTable::log_error(pre_read.get_line(), "m");
					}
					else
					{
						ids.push_back(lexer.get_next());
					}
					pre_read = lexer.peek_next();
				}
			}
			break;
		default:
			return lexer.get_pos();
	}
	return -1;
}

IdentifyType Factor::check_type()
{
	if (type == 2)
		return IdentifyType::INT;
	if (type == 3)
		return IdentifyType::CHAR;
	if (type == 4)
		return IdentifyTable::get_return_type(diao_yong->get_func_id());
	if (type == 1)
	{
		if (ids[0].get_type() == TypeEnum::LPARENT)
		{
			return exps[0].check_type();
		}
		else
		{
			return IdentifyTable::get_type_by_name(&(ids[0]));
		}
	}
	return IdentifyType::NONE;
}

string Factor::to_string()
{
	string for_return;
	for_return.clear();
	switch (type)
	{
		case 2:
			for_return += zheng_shu.to_string();
			break;
		case 3:
			for_return += zi_fu.to_string();
			break;
		case 4:
			for_return += diao_yong->to_string();
			break;
		case 1:
			int j = 0;
			for (int i = 0; i < ids.size(); i++)
			{
				if (ids[i].get_type() == TypeEnum::EMPTY)
				{
					for_return += exps[j].to_string();
					j++;
				}
				else
				{
					for_return += ids[i].to_string();
				}
			}
			break;
	}
	for_return += "<因子>\n";
	return for_return;
}

int Item::read_in(Lexer& lexer)
{
	word_pos = lexer.get_pos();
	Factor begin;
	int r = begin.read_in(lexer);
	if (r != -1)
		return r;
	factors.push_back(begin);
	WordInfo tmp = lexer.peek_next();
	while (tmp.get_type() == TypeEnum::MULT || tmp.get_type() == TypeEnum::DIV)
	{
		signs.push_back(lexer.get_next());
		Factor f;
		r = f.read_in(lexer);
		if (r != -1)
			return r;
		factors.push_back(f);
		tmp = lexer.peek_next();
	}
	return -1;
}

IdentifyType Item::check_type()
{
	if (signs.size() != 0)
		return IdentifyType::INT;
	else
		return factors[0].check_type();
}

string Item::to_string()
{
	string for_return;
	for_return.clear();
	for (int i = 0; i < factors.size(); i++)
	{
		for_return += factors[i].to_string();
		if (i < signs.size())
			for_return += signs[i].to_string();
	}
	for_return += "<项>\n";
	return for_return;
}

int Expression::read_in(Lexer& lexer)
{
	word_pos = lexer.get_pos();
	WordInfo tmp = lexer.peek_next();
	if (tmp.get_type() == TypeEnum::PLUS || tmp.get_type() == TypeEnum::MINU)
	{
		signs.push_back(lexer.get_next());
		have_pre_sign = true;
	}
	else
		have_pre_sign = false;
	Item begin;
	int r = begin.read_in(lexer);
	if (r != -1)
		return r;
	items.push_back(begin);
	tmp = lexer.peek_next();
	while (tmp.get_type() == TypeEnum::PLUS || tmp.get_type() == TypeEnum::MINU)
	{
		signs.push_back(lexer.get_next());
		Item i;
		int r = i.read_in(lexer);
		if (r != -1)
			return r;
		items.push_back(i);
		tmp = lexer.peek_next();
	}
	return -1;
}

IdentifyType Expression::check_type()
{
	if (signs.size() != 0)
		return IdentifyType::INT;
	else
		return items[0].check_type();
}

string Expression::to_string()
{
	string for_return;
	for_return.clear();
	int i_sign = 0;
	int i_item = 0;
	if (have_pre_sign)
	{
		for_return += signs[0].to_string();
		i_sign++;
	}
	for (i_item = 0; i_item < items.size(); i_item++)
	{
		for_return += items[i_item].to_string();
		if (i_sign < signs.size())
		{
			for_return += signs[i_sign].to_string();
			i_sign++;
		}
	}
	for_return += "<表达式>\n";
	return for_return;
}

int FunctionMain::read_in(Lexer& lexer)
{
	word_pos = lexer.get_pos();
	void_tk = lexer.get_next();
	if (void_tk.get_type() != TypeEnum::VOIDTK)
		return lexer.get_pos();
	main_tk = lexer.get_next();
	if (main_tk.get_type() != TypeEnum::MAINTK)
		return lexer.get_pos();
	IdentifyTable::add_func(&main_tk, IdentifyType::VOID, NULL);
	lparent = lexer.get_next();
	if (lparent.get_type() != TypeEnum::LPARENT)
		return lexer.get_pos();
	rparent = lexer.peek_next();
	if (rparent.get_type() != TypeEnum::RPARENT)
	{
		// Wu Xiao Kuo Hao
		ErrorTable::log_error(lparent.get_line(), "l");
	}
	else
	{
		lexer.get_next();
	}
	lbrace = lexer.get_next();
	if (lbrace.get_type() != TypeEnum::LBRACE)
		return lexer.get_pos();
	int r = fu_he.read_in(lexer);
	if (r != -1)
		return r;
	rbrace = lexer.get_next();
	if (rbrace.get_type() != TypeEnum::RBRACE)
		return lexer.get_pos();
	vector<SentenceReturn*> returns = fu_he.get_all_return();
	unsigned int i;
	for (i = 0; i < returns.size(); i++)
	{
		if (returns[i]->get_return_type() != IdentifyType::VOID)
		{
			// Wu Fan Hui Zhi Han Shu Bu Pi Pei
			ErrorTable::log_error(returns[i]->get_word_line(), "g");
		}
	}
	return -1;
}

string FunctionMain::to_string()
{
	string for_return;
	for_return.clear();
	for_return += void_tk.to_string();
	for_return += main_tk.to_string();
	for_return += lparent.to_string();
	for_return += rparent.to_string();
	for_return += lbrace.to_string();
	for_return += fu_he.to_string();
	for_return += rbrace.to_string();
	for_return += "<主函数>\n";
	return for_return;
}

int ParameterTable::read_in(Lexer& lexer)
{
	word_pos = lexer.get_pos();
	WordInfo tmp = lexer.peek_next();
	count = 0;
	items.clear();
	types.clear();
	ids.clear();
	while (tmp.get_type() == TypeEnum::INTTK || tmp.get_type() == TypeEnum::CHARTK)
	{
		types.push_back(lexer.get_next());
		ids.push_back(lexer.get_next());
		count++;
		tmp = lexer.peek_next();
		if (tmp.get_type() == TypeEnum::COMMA)
		{
			items.push_back(lexer.get_next());
			tmp = lexer.peek_next();
		}
		else
		{
			break;
		}
	}
	if (count == 0)
		is_empty = true;
	else
		is_empty = false;
	//printf("ids:%d, types:%d\n", ids.size(), types.size());
	return -1;
	//TODO : check wrong
}

string ParameterTable::to_string()
{
	string for_return;
	for_return.clear();
	unsigned int i;
	for (i = 0; i < count - 1; i++)
	{
		for_return += types[i].to_string();
		for_return += ids[i].to_string();
		for_return += items[0].to_string();
	}
	for_return += types[count - 1].to_string();
	for_return += ids[count - 1].to_string();
	for_return += "<参数表>\n";
	return for_return;
}

int SentenceFuHe::read_in(Lexer& lexer)
{
	word_pos = lexer.get_pos();
	WordInfo tmp = lexer.peek_next();
	int r;
	if (tmp.get_type() == TypeEnum::CONSTTK)
	{
		type |= 1;
		r = consts.read_in(lexer);
		if (r != -1)
			return r;
		tmp = lexer.peek_next();
	}
	if (tmp.get_type() == TypeEnum::INTTK || tmp.get_type() == TypeEnum::CHARTK)
	{
		type |= 2;
		r = vars.read_in(lexer);
		if (r != -1)
			return r;
		tmp = lexer.peek_next();
	}
	yu_ju_lie = new SentenceLie;
	r = yu_ju_lie->read_in(lexer);
	if (r != -1)
		return r;
	return -1;
}

vector<SentenceReturn*> SentenceFuHe::get_all_return()
{
	return yu_ju_lie->get_all_return();
}

string SentenceFuHe::to_string()
{
	string for_return;
	for_return.clear();
	if (type & 1)
		for_return += consts.to_string();
	if (type & 2)
		for_return += vars.to_string();
	for_return += yu_ju_lie->to_string();
	for_return += "<复合语句>\n";
	return for_return;
}

int FunctionDingYi::read_in(Lexer& lexer)
{
	word_pos = lexer.get_pos();
	WordInfo tmp = lexer.peek_next();
	IdentifyType return_type;
	if (tmp.get_type() == TypeEnum::VOIDTK)
	{
		have_return = false;
		void_tk = lexer.get_next();
		id = lexer.get_next();
		if (id.get_type() != TypeEnum::IDENFR)
			return lexer.get_pos();
		return_type = IdentifyType::VOID;
	}
	else if (tmp.get_type() == TypeEnum::INTTK || tmp.get_type() == TypeEnum::CHARTK)
	{
		have_return = true;
		if (tmp.get_type() == TypeEnum::INTTK)
			return_type = IdentifyType::INT;
		else
			return_type = IdentifyType::CHAR;
		int r = head.read_in(lexer);
		if (r != -1)
			return r;
		id = head.get_id();
	}
	else
		return lexer.get_pos();

	lparent = lexer.get_next();
	if (lparent.get_type() != TypeEnum::LPARENT)
		return lexer.get_pos();

	int r = table.read_in(lexer);
	if (r != -1)
		return r;

	if (!IdentifyTable::add_func(&id, return_type, &table))
	{
		// Chong Ding Yi
		ErrorTable::log_error(id.get_line(), "b");
	}

	rparent = lexer.peek_next();
	if (rparent.get_type() != TypeEnum::RPARENT)
	{
		// Wu Xiao Kuo Hao
		ErrorTable::log_error(lparent.get_line(), "l");
	}
	else
	{
		lexer.get_next();
	}
	lbrace = lexer.get_next();
	if (lbrace.get_type() != TypeEnum::LBRACE)
		return lexer.get_pos();
	r = fu_he.read_in(lexer);
	if (r != -1)
		return r;
	rbrace = lexer.get_next();
	if (rbrace.get_type() != TypeEnum::RBRACE)
		return lexer.get_pos();

	vector<SentenceReturn* > returns = fu_he.get_all_return();
	if (return_type == IdentifyType::VOID)
	{
		unsigned int i;
		for (i = 0; i < returns.size(); i++)
		{
			if (returns[i]->get_return_type() != return_type)
			{
				// Wu Fan Hui Zhi Han Shu Bu Pi Pei
				ErrorTable::log_error(returns[i]->get_word_line(), "g");
			}
		}
	}
	else
	{
		if (returns.size() == 0)
		{
			// You Fan Hui Zhi Han Shu Bu Pi Pei
			ErrorTable::log_error(rbrace.get_line(), "h");
		}
		else
		{
			unsigned int i;
			for (i = 0; i < returns.size(); i++)
			{
				if (returns[i]->get_return_type() != return_type)
				{
					// You Fan Hui Zhi Han Shu Bu Pi Pei
					ErrorTable::log_error(returns[i]->get_word_line(), "h");
				}
			}
		}
	}

	return -1;
}

string FunctionDingYi::to_string()
{
	string for_return;
	for_return.clear();
	if (have_return)
	{
		for_return += head.to_string();
	}
	else
	{
		for_return += void_tk.to_string();
		for_return += id.to_string();
	}
	for_return += lparent.to_string();
	for_return += table.to_string();
	for_return += rparent.to_string();
	for_return += lbrace.to_string();
	for_return += fu_he.to_string();
	for_return += rbrace.to_string();
	if (have_return)
		for_return += "<有返回值函数定义>\n";
	else
		for_return += "<无返回值函数定义>\n";
	return for_return;
}

int Grammar::read_in(Lexer& lexer)
{
	lexer.set_pos(0);
	WordInfo pre_read = lexer.peek_next();
	int r;
	if (pre_read.get_type() == TypeEnum::CONSTTK)
	{
		type |= 1;
		r = consts.read_in(lexer);
		if (r != -1)
			return r;
		pre_read = lexer.peek_next();
	}
	if (pre_read.get_type() == TypeEnum::INTTK || pre_read.get_type() == TypeEnum::CHARTK)
	{
		WordInfo check_brace = lexer.get_by_pos(lexer.get_pos() + 2);
		if (check_brace.get_type() != TypeEnum::LPARENT)
		{
			type |= 2;
			r = vars.read_in(lexer);
			if (r != -1)
				return r;
			pre_read = lexer.peek_next();
		}
	}
	while (pre_read.get_type() == TypeEnum::INTTK ||
		pre_read.get_type() == TypeEnum::CHARTK || pre_read.get_type() == TypeEnum::VOIDTK)
	{
		WordInfo check_id = lexer.get_by_pos(lexer.get_pos() + 1);
		if (check_id.get_type() == TypeEnum::MAINTK)
			break;
		type |= 4;
		FunctionDingYi f;
		r = f.read_in(lexer);
		if (r != -1)
			return r;
		funcs.push_back(f);
		pre_read = lexer.peek_next();
	}
	r = main_func.read_in(lexer);
	if (r != -1)
		return r;
	return -1;
}

void Grammar::print_to_file(string out_file_name)
{
	ofstream out;
	out.open(out_file_name);
	if (type & 1)
	{
		out << consts.to_string();
	}
	if (type & 2)
	{
		out << vars.to_string();
	}
	for (int i = 0; i < funcs.size(); i++)
	{
		out << funcs[i].to_string();
	}
	out << main_func.to_string();
	out << "<程序>";
	out.close();
}


