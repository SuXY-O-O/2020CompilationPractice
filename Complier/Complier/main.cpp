#include "Lexer.h"
#include <iostream>

const string in_file_name = "testfile.txt";
const string out_file_name = "output.txt";

int main()
{
	Lexer lexer;
	lexer.process_input(in_file_name);
	lexer.print_to_file(out_file_name);
}