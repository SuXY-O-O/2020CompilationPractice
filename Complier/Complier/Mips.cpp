#include "Mips.h"

Mips mips;

string Mips::li(int reg, int value)
{
	string for_return;
	for_return.clear();
	for_return += "li $";
	for_return += std::to_string(reg);
	for_return += ", ";
	for_return += std::to_string(value);
	for_return += "\n";
	return for_return;
}

string Mips::la(int reg, string label)
{
	string for_return;
	for_return.clear();
	for_return += "la $";
	for_return += std::to_string(reg);
	for_return += ", ";
	for_return += label;
	for_return += "\n";
	return for_return;
}

string Mips::move(int reg_to, int reg_from)
{
	string for_return;
	for_return.clear();
	for_return += "move $";
	for_return += std::to_string(reg_to);
	for_return += ", $";
	for_return += std::to_string(reg_from);
	for_return += "\n";
	return for_return;
}

string Mips::lw(int reg_to, int offset, int reg_addr)
{
	string for_return;
	for_return.clear();
	for_return += "lw $";
	for_return += std::to_string(reg_to);
	for_return += ", ";
	for_return += std::to_string(offset);
	for_return += "($";
	for_return += std::to_string(reg_addr);
	for_return += ")\n";
	return for_return;
}

string Mips::sw(int reg_from, int offset, int reg_addr)
{
	string for_return;
	for_return.clear();
	for_return += "sw $";
	for_return += std::to_string(reg_from);
	for_return += ", ";
	for_return += std::to_string(offset);
	for_return += "($";
	for_return += std::to_string(reg_addr);
	for_return += ")\n";
	return for_return;
}

string Mips::sw(int reg_from, string label)
{
	string for_return;
	for_return.clear();
	for_return += "sw $";
	for_return += std::to_string(reg_from);
	for_return += ", ";
	for_return += label;
	for_return += "\n";
	return for_return;
}

string Mips::add(int reg_to, int reg_1, int reg_2)
{
	string for_return;
	for_return.clear();
	for_return += "add $";
	for_return += std::to_string(reg_to);
	for_return += ", $";
	for_return += std::to_string(reg_1);
	for_return += ", $";
	for_return += std::to_string(reg_2);
	for_return += "\n";
	return for_return;
}

string Mips::addi(int reg_to, int reg_from, int i)
{
	string for_return;
	for_return.clear();
	for_return += "addi $";
	for_return += std::to_string(reg_to);
	for_return += ", $";
	for_return += std::to_string(reg_from);
	for_return += ", ";
	for_return += std::to_string(i);
	for_return += "\n";
	return for_return;
}

string Mips::sub(int reg_to, int reg_1, int reg_2)
{
	string for_return;
	for_return.clear();
	for_return += "sub $";
	for_return += std::to_string(reg_to);
	for_return += ", $";
	for_return += std::to_string(reg_1);
	for_return += ", $";
	for_return += std::to_string(reg_2);
	for_return += "\n";
	return for_return;
}

string Mips::mul(int reg_to, int reg_1, int reg_2)
{
	string for_return;
	for_return.clear();
	for_return += "mul $";
	for_return += std::to_string(reg_to);
	for_return += ", $";
	for_return += std::to_string(reg_1);
	for_return += ", $";
	for_return += std::to_string(reg_2);
	for_return += "\n";
	return for_return;
}

string Mips::div(int reg_to, int reg_1, int reg_2)
{
	string for_return;
	for_return.clear();
	for_return += "div $";
	for_return += std::to_string(reg_to);
	for_return += ", $";
	for_return += std::to_string(reg_1);
	for_return += ", $";
	for_return += std::to_string(reg_2);
	for_return += "\n";
	return for_return;
}

string Mips::neg(int reg_to, int reg_from)
{
	string for_return;
	for_return.clear();
	for_return += "neg $";
	for_return += std::to_string(reg_to);
	for_return += ", $";
	for_return += std::to_string(reg_from);
	for_return += "\n";
	return for_return;
}

string Mips::jal(string label)
{
	return "jal " + label + "\n";
}

string Mips::jr()
{
	return "jr $ra\n";
}

string Mips::syscall(int code_in_v0)
{
	string for_return;
	for_return.clear();
	for_return += "li $v0, ";
	for_return += std::to_string(code_in_v0);
	for_return += "\n";
	for_return += "syscall\n";
	return for_return;
}
