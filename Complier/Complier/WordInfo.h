#pragma once
#include<string>
#include "TypeEnum.h"
using namespace std;
class WordInfo
{
private:
	TypeEnum type;
	string input;
	int line;
	int count;
public:
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
	int get_line();
	static string type_to_string(TypeEnum t);
	string to_string();
};

