#pragma once

#include "WordInfo.h"
#include <map>

using namespace std;

class FunctionTable
{
private:
	static map<string, int> name_to_return_type;	//0: void; 1: int; 2: char
	//TODO: save parameters of a function
public:
	static void add_func(WordInfo id, int type)
	{
		name_to_return_type.insert(pair<string, int>(id.get_string(), type));
	}
	static bool have_return(WordInfo id)
	{
		map<string, int>::iterator ite;
		ite = name_to_return_type.find(id.get_string());
		if (ite != name_to_return_type.end())
		{
			return ite->second != 0;
		}
		else
		{
			//TODO: cannot find the function
			return false;
		}
	}
};

