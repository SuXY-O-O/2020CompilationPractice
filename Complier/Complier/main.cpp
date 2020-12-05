//#include "Lexer.h"
#include "Grammar.h"
#include "Error.h"
#include "MiddleCode.h"

//#include <iostream>

const string in_file_name = "testfile.txt";
const string out_lexer = "output_lexer.txt";
const string out_file_name = "output.txt";
const string error_file = "error.txt";

int main()
{
	Lexer lexer;
	IdentifyTable::init();
	lexer.process_input(in_file_name);
	//lexer.print_to_file(out_lexer);
	//ErrorTable::print_to_file(error_file);
	Grammar grammar;
	MiddleCode middle;
	grammar.read_in(lexer);
	if (ErrorTable::have_error())
	{
		ErrorTable::print_to_file(error_file);
	}
	else
	{
		lexer.print_to_file(out_lexer);
		grammar.print_to_file(out_file_name);
		middle.read_in(grammar);
		middle.print_mips_to_file("mips.txt");
	}
}