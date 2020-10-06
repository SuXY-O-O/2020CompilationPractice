//#include "Lexer.h"
#include "Grammar.h"
//#include <iostream>

const string in_file_name = "testfile.txt";
const string out_lexer = "output_lexer.txt";
const string out_file_name = "output.txt";

int main()
{
	Lexer lexer;
	lexer.process_input(in_file_name);
	lexer.print_to_file(out_lexer);
	Grammar grammar;
	grammar.read_in(lexer);
	grammar.print_to_file(out_file_name);
}