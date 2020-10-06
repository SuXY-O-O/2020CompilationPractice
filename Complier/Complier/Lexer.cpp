//#include <ctype.h>
#include "Lexer.h"
//#include "WordInfo.h"
//#include "TypeEnum.h"
#include <iostream>

void Lexer::process_input(string in_file_name)
{
	ifstream infile(in_file_name);
	if (!infile.is_open())
	{
		printf("unopen\n");
		//throw new exception("Input file fail to open\n");
	}
	char now;
	int line_count = 0;
	int pos_count = 0;
	while (infile.peek() != EOF)
	{
		now = infile.get();
		// when we get a space
		if (isspace(now))
		{
			// when we get a new line
			if (now == '\n')
			{
				line_count++;
				pos_count = 0;
			}
			continue;
		}
		// when we get a number
		else if (isdigit(now))
		{
			string num;
			num.clear();
			num += now;
			while (isdigit(infile.peek()))
			{
				num += infile.get();
			}
			WordInfo now_word(TypeEnum::INTCON, num, line_count, pos_count);
			word_list.push_back(now_word);
			pos_count++;
			continue;
		}
		// when we get a beginner of word
		else if (isalpha(now) || now == '_')
		{
			string word, to_low;
			word.clear();
			to_low.clear();
			word += now;
			bool flag;	// set if the word can be a reserved word
			if (now == '_')
				flag = false;
			else
			{
				flag = true;
				to_low += tolower(now);
			}
			while (isalnum(infile.peek()) || infile.peek() == '_')
			{
				now = infile.get();
				word += now;
				if (isdigit(now) || now == '_')
					flag = false;
				else
					to_low += tolower(now);
			}
			if (flag)	// can be a reserved word
			{
				if (to_low.compare("const") == 0)
				{
					WordInfo now_word(TypeEnum::CONSTTK, word, line_count, pos_count);
					word_list.push_back(now_word);
				}
				else if (to_low.compare("int") == 0)
				{
					WordInfo now_word(TypeEnum::INTTK, word, line_count, pos_count);
					word_list.push_back(now_word);
				}
				else if (to_low.compare("char") == 0)
				{
					WordInfo now_word(TypeEnum::CHARTK, word, line_count, pos_count);
					word_list.push_back(now_word);
				}
				else if (to_low.compare("void") == 0)
				{
					WordInfo now_word(TypeEnum::VOIDTK, word, line_count, pos_count);
					word_list.push_back(now_word);
				}
				else if (to_low.compare("main") == 0)
				{
					WordInfo now_word(TypeEnum::MAINTK, word, line_count, pos_count);
					word_list.push_back(now_word);
				}
				else if (to_low.compare("if") == 0)
				{
					WordInfo now_word(TypeEnum::IFTK, word, line_count, pos_count);
					word_list.push_back(now_word);
				}
				else if (to_low.compare("else") == 0)
				{
					WordInfo now_word(TypeEnum::ELSETK, word, line_count, pos_count);
					word_list.push_back(now_word);
				}
				else if (to_low.compare("switch") == 0)
				{
					WordInfo now_word(TypeEnum::SWITCHTK, word, line_count, pos_count);
					word_list.push_back(now_word);
				}
				else if (to_low.compare("case") == 0)
				{
					WordInfo now_word(TypeEnum::CASETK, word, line_count, pos_count);
					word_list.push_back(now_word);
				}
				else if (to_low.compare("default") == 0)
				{
					WordInfo now_word(TypeEnum::DEFAULTTK, word, line_count, pos_count);
					word_list.push_back(now_word);
				}
				else if (to_low.compare("while") == 0)
				{
					WordInfo now_word(TypeEnum::WHILETK, word, line_count, pos_count);
					word_list.push_back(now_word);
				}
				else if (to_low.compare("for") == 0)
				{
					WordInfo now_word(TypeEnum::FORTK, word, line_count, pos_count);
					word_list.push_back(now_word);
				}
				else if (to_low.compare("scanf") == 0)
				{
					WordInfo now_word(TypeEnum::SCANFTK, word, line_count, pos_count);
					word_list.push_back(now_word);
				}
				else if (to_low.compare("printf") == 0)
				{
					WordInfo now_word(TypeEnum::PRINTFTK, word, line_count, pos_count);
					word_list.push_back(now_word);
				}
				else if (to_low.compare("return") == 0)
				{
					WordInfo now_word(TypeEnum::RETURNTK, word, line_count, pos_count);
					word_list.push_back(now_word);
				}
				else
				{
					WordInfo now_word(TypeEnum::IDENFR, word, line_count, pos_count);
					word_list.push_back(now_word);
				}
			}
			else
			{
				WordInfo now_word(TypeEnum::IDENFR, word, line_count, pos_count);
				word_list.push_back(now_word);
			}
			pos_count++;
			continue;
		}
		// when we are going to read a char
		else if (now == '\'')
		{
			now = infile.get();
			infile.ignore(1);
			string s;
			s.clear();
			s += now;
			WordInfo now_word(TypeEnum::CHARCON, s, line_count, pos_count);
			word_list.push_back(now_word);
			pos_count++;
			continue;
		}
		// when we are going to read a string
		else if (now == '\"')
		{
			string s;
			s.clear();
			while (infile.peek() != '\"')
			{
				s += infile.get();
			}
			infile.ignore(1);
			WordInfo now_word(TypeEnum::STRCON, s, line_count, pos_count);
			word_list.push_back(now_word);
			pos_count++;
			continue;
		}
		// when it seems we get an operator
		else
		{
			switch (now)
			{
				// only one char
				case '+': 
				{
					WordInfo now_word(TypeEnum::PLUS, "+", line_count, pos_count);
					word_list.push_back(now_word);
					pos_count++;
					break; 
				}
				case '-': 
				{
					WordInfo now_word(TypeEnum::MINU, "-", line_count, pos_count);
					word_list.push_back(now_word);
					pos_count++;
					break; 
				}
				case '*':
				{
					WordInfo now_word(TypeEnum::MULT, "*", line_count, pos_count);
					word_list.push_back(now_word);
					pos_count++;
					break;
				}
				case '/':
				{
					WordInfo now_word(TypeEnum::DIV, "/", line_count, pos_count);
					word_list.push_back(now_word);
					pos_count++;
					break;
				}
				case ':':
				{
					WordInfo now_word(TypeEnum::COLON, ":", line_count, pos_count);
					word_list.push_back(now_word);
					pos_count++;
					break;
				}
				case ';':
				{
					WordInfo now_word(TypeEnum::SEMICN, ";", line_count, pos_count);
					word_list.push_back(now_word);
					pos_count++;
					break;
				}
				case ',':
				{
					WordInfo now_word(TypeEnum::COMMA, ",", line_count, pos_count);
					word_list.push_back(now_word);
					pos_count++;
					break;
				}
				case '(':
				{
					WordInfo now_word(TypeEnum::LPARENT, "(", line_count, pos_count);
					word_list.push_back(now_word);
					pos_count++;
					break;
				}
				case ')':
				{
					WordInfo now_word(TypeEnum::RPARENT, ")", line_count, pos_count);
					word_list.push_back(now_word);
					pos_count++;
					break;
				}
				case '[':
				{
					WordInfo now_word(TypeEnum::LBRACK, "[", line_count, pos_count);
					word_list.push_back(now_word);
					pos_count++;
					break;
				}
				case ']':
				{
					WordInfo now_word(TypeEnum::RBRACK, "]", line_count, pos_count);
					word_list.push_back(now_word);
					pos_count++;
					break;
				}
				case '{':
				{
					WordInfo now_word(TypeEnum::LBRACE, "{", line_count, pos_count);
					word_list.push_back(now_word);
					pos_count++;
					break;
				}
				case '}':
				{
					WordInfo now_word(TypeEnum::RBRACE, "}", line_count, pos_count);
					word_list.push_back(now_word);
					pos_count++;
					break;
				}
				// only two chars
				case '!':
				{
					if (infile.peek() != '=')
					{
						printf("Unspecified char %c\n", now);
					}
					else
					{
						infile.ignore(1);
						WordInfo now_word(TypeEnum::NEQ, "!=", line_count, pos_count);
						word_list.push_back(now_word);
						pos_count++;
					}
					break;
				}
				// one or two chars
				case '<':
				{
					if (infile.peek() == '=')
					{
						infile.ignore(1);
						WordInfo now_word(TypeEnum::LEQ, "<=", line_count, pos_count);
						word_list.push_back(now_word);
						pos_count++;
					}
					else
					{
						WordInfo now_word(TypeEnum::LSS, "<", line_count, pos_count);
						word_list.push_back(now_word);
						pos_count++;
					}
					break;
				}
				case '>':
				{
					if (infile.peek() == '=')
					{
						infile.ignore(1);
						WordInfo now_word(TypeEnum::GEQ, ">=", line_count, pos_count);
						word_list.push_back(now_word);
						pos_count++;
					}
					else
					{
						WordInfo now_word(TypeEnum::GRE, ">", line_count, pos_count);
						word_list.push_back(now_word);
						pos_count++;
					}
					break;
				}
				case '=':
				{
					if (infile.peek() == '=')
					{
						infile.ignore(1);
						WordInfo now_word(TypeEnum::EQL, "==", line_count, pos_count);
						word_list.push_back(now_word);
						pos_count++;
					}
					else
					{
						WordInfo now_word(TypeEnum::ASSIGN, "=", line_count, pos_count);
						word_list.push_back(now_word);
						pos_count++;
					}
					break;
				}
				// unspecified char
				default :
				{
					// wrong handler
					printf("Unspecified char %c\n", now);
				}
			}
		}
	}
	infile.close();
}

WordInfo Lexer::get_next()
{
	int for_return = word_pos;
	word_pos++;
	return word_list[for_return];
}

WordInfo Lexer::peek_next()
{
	return word_list[word_pos];
}

WordInfo Lexer::get_by_pos(int index)
{
	return word_list[index];
}

int Lexer::get_pos()
{
	return word_pos;
}

void Lexer::set_pos(int pos)
{
	word_pos = pos;
}

void Lexer::print_to_file(string out_file_name)
{
	ofstream out;
	out.open(out_file_name);
	int i;
	//out << "begin" << word_list.size() << "\n";
	for (i = 0; i < word_list.size(); i++)
	{
		out << word_list[i].to_string();
	}
	out.close();
}