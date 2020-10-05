#pragma once
#include<vector>
#include<fstream>
#include"WordInfo.h"
using namespace std;
class Lexer
{
private :
	vector<WordInfo> word_list;
	int word_pos = 0;
public :
	void process_input(string in_file_name);
	WordInfo get_next();
	WordInfo peek_next();
	WordInfo get_by_pos(int index);
	int get_pos();
	void set_pos(int pos);
	void print_to_file(string out_file_name);
};