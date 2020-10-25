#pragma once
#include "WordInfo.h"
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;

class Error
{
private:
	int line;
	string error_type;
public:
	Error(int i, string e) : line(i), error_type(e) {}
	bool operator < (const Error& e1)
	{
		return this->line < e1.line;
	}
	bool operator > (const Error& e1)
	{
		return this->line > e1.line;
	}
	bool operator == (const Error& e1)
	{
		return this->line == e1.line;
	}
	bool operator != (const Error& e1)
	{
		return this->line != e1.line;
	}
	int get_line()
	{
		return line;
	}
	string to_string()
	{
		string for_return;
		for_return.clear();
		for_return = std::to_string(line) + " " + error_type + "\n";
		return for_return;
	}
};

class ErrorTable
{
private:
	static vector<Error* > errors;
public:
	static void log_error(int line, string type)
	{
		Error* new_one = new Error(line, type);
		errors.push_back(new_one);
	}
	static void print_to_file(string file_name)
	{
		//sort(errors.begin(), errors.end());
		unsigned int i, j;
		for (i = 0; i < errors.size(); i++)
		{
			for (j = i; j < errors.size(); j++)
			{
				if (errors[i]->get_line() > errors[j]->get_line())
				{
					Error* tmp = errors[i];
					errors[i] = errors[j];
					errors[j] = tmp;
				}
			}
		}
		ofstream out;
		out.open(file_name);
		j = -1;
		for (i = 0; i < errors.size(); i++)
		{
			if (j != errors[i]->get_line())
			{
				out << errors[i]->to_string();
				j = errors[i]->get_line();
			}
			printf("%s", errors[i]->to_string().c_str());
		}
		out.close();
	}
	static bool have_error()
	{
		return errors.size() != 0;
	}
};