#include "WordInfo.h"
#include "TypeEnum.h"

void WordInfo::set_string(string in)
{
	this->input = in;
}

TypeEnum WordInfo::get_type()
{
	return this->type;
}

string WordInfo::get_string()
{
	return this->input;
}

int WordInfo::get_line()
{
	return this->line;
}

string WordInfo::type_to_string(TypeEnum t)
{
	string for_return;
	switch (t)
	{
	case TypeEnum::ASSIGN :
		for_return = "ASSIGN";
		break;
	case TypeEnum::CASETK :
		for_return = "CASETK";
		break;
	case TypeEnum::CHARCON :
		for_return = "CHARCON";
		break;
	case TypeEnum::CHARTK :
		for_return = "CHARTK";
		break;
	case TypeEnum::COLON :
		for_return = "COLON";
		break;
	case TypeEnum::COMMA :
		for_return = "COMMA";
		break;
	case TypeEnum::IDENFR :
		for_return = "IDENFR";
		break;
	case TypeEnum::INTCON :
		for_return = "INTCON";
		break;
	case TypeEnum::STRCON :
		for_return = "STRCON";
		break;
	case TypeEnum::CONSTTK :
		for_return = "CONSTTK";
		break;
	case TypeEnum::INTTK :
		for_return = "INTTK";
		break;
	case TypeEnum::VOIDTK :
		for_return = "VOIDTK";
		break;
	case TypeEnum::MAINTK :
		for_return = "MAINTK";
		break;
	case TypeEnum::IFTK :
		for_return = "IFTK";
		break;
	case TypeEnum::ELSETK :
		for_return = "ELSETK";
		break;
	case TypeEnum::SWITCHTK :
		for_return = "SWITCHTK";
		break;
	case TypeEnum::DEFAULTTK :
		for_return = "DEFAULTTK";
		break;
	case TypeEnum::WHILETK:
		for_return = "WHILETK";
		break;
	case TypeEnum::FORTK :
		for_return = "FORTK";
		break;
	case TypeEnum::SCANFTK :
		for_return = "SCANFTK";
		break;
	case TypeEnum::PRINTFTK :
		for_return = "PRINTFTK";
		break;
	case TypeEnum::RETURNTK :
		for_return = "RETURNTK";
		break;
	case TypeEnum::PLUS :
		for_return = "PLUS";
		break;
	case TypeEnum::MINU :
		for_return = "MINU";
		break;
	case TypeEnum::MULT :
		for_return = "MULT";
		break;
	case TypeEnum::DIV :
		for_return = "DIV";
		break;
	case TypeEnum::LSS :
		for_return = "LSS";
		break;
	case TypeEnum::LEQ :
		for_return = "LEQ";
		break;
	case TypeEnum::GRE :
		for_return = "GRE";
		break;
	case TypeEnum::GEQ :
		for_return = "GEQ";
		break;
	case TypeEnum::EQL :
		for_return = "EQL";
		break;
	case TypeEnum::NEQ :
		for_return = "NEQ";
		break;
	case TypeEnum::SEMICN :
		for_return = "SEMICN";
		break;
	case TypeEnum::LPARENT :
		for_return = "LPARENT";
		break;
	case TypeEnum::RPARENT :
		for_return = "RPARENT";
		break;
	case TypeEnum::LBRACK :
		for_return = "LBRACK";
		break;
	case TypeEnum::LBRACE :
		for_return = "LBRACE";
		break;
	case TypeEnum::RBRACK :
		for_return = "RBRACK";
		break;
	case TypeEnum::RBRACE :
		for_return = "RBRACE";
		break;
	default :
		for_return = "unknown type";
	}
	return for_return;
}

string WordInfo::to_string()
{
	return type_to_string(this->type) + " " + this->input;
}
