#pragma once
#include<vector>
#include<fstream>
#include"WordInfo.h"
using namespace std;
class Lexer
{
private :
	vector<WordInfo> word_list;
	int word_pos;
public :
	void process_input(string in_file_name);
	WordInfo get_next();
	WordInfo get_index(int index);
	void set_pos(int pos);
	void print_to_file(string out_file_name);
};

