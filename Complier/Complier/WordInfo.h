#pragma once

#include "TypeEnum.h"
#include<string>

using namespace std;

class WordInfo
{
private:
	TypeEnum type;
	string input;
	int line = 0;
	int count = 0;
public:
	WordInfo():type(TypeEnum::EMPTY){}
	WordInfo(TypeEnum t, int l, int c)
	{
		this->count = c;
		this->line = l;
		this->type = t;
	}
	WordInfo(TypeEnum t, string i, int l, int c)
	{
		this->count = c;
		this->line = l;
		this->type = t;
		this->input = i;
	}
	void set_string(string in);
	TypeEnum get_type();
	string get_string();
	string get_string_in_low();
	int get_line();
	static string type_to_string(TypeEnum t);
	string to_string();
};

