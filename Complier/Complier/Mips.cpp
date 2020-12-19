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

string Mips::la_l_r(int reg, string label, int reg_offset)
{
	string for_return;
	for_return.clear();
	for_return += "la $";
	for_return += std::to_string(reg);
	for_return += ", ";
	for_return += label;
	for_return += "($";
	for_return += std::to_string(reg_offset);
	for_return += ")\n";
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
	//for_return += "add $";
	for_return += "addu $";
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
	//for_return += "addi $";
	for_return += "addiu $";
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
	//for_return += "sub $";
	for_return += "subu $";
	for_return += std::to_string(reg_to);
	for_return += ", $";
	for_return += std::to_string(reg_1);
	for_return += ", $";
	for_return += std::to_string(reg_2);
	for_return += "\n";
	return for_return;
}

string Mips::subi(int reg_to, int reg_from, int i)
{
	string for_return;
	for_return.clear();
	//for_return += "subi $";
	for_return += "subiu $";
	for_return += std::to_string(reg_to);
	for_return += ", $";
	for_return += std::to_string(reg_from);
	for_return += ", ";
	for_return += std::to_string(i);
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

string Mips::muli(int reg_to, int reg_from, int i)
{
	string for_return;
	for_return.clear();
	for_return += "mul $";
	for_return += std::to_string(reg_to);
	for_return += ", $";
	for_return += std::to_string(reg_from);
	for_return += ", ";
	for_return += std::to_string(i);
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

string Mips::divi(int reg_to, int reg_from, int i)
{
	string for_return;
	for_return.clear();
	for_return += "div $";
	for_return += std::to_string(reg_to);
	for_return += ", $";
	for_return += std::to_string(reg_from);
	for_return += ", ";
	for_return += std::to_string(i);
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

string Mips::beq(int reg1, int reg2, string label)
{
	string for_return;
	for_return.clear();
	for_return += "beq $";
	for_return += std::to_string(reg1);
	for_return += ", $";
	for_return += std::to_string(reg2);
	for_return += ", ";
	for_return += label;
	for_return += "\n";
	return for_return;
}

string Mips::beqi(int reg1, int i, string label)
{
	string for_return;
	for_return.clear();
	for_return += "beq $";
	for_return += std::to_string(reg1);
	for_return += ", ";
	for_return += std::to_string(i);
	for_return += ", ";
	for_return += label;
	for_return += "\n";
	return for_return;
}

string Mips::bne(int reg1, int reg2, string label)
{
	string for_return;
	for_return.clear();
	for_return += "bne $";
	for_return += std::to_string(reg1);
	for_return += ", $";
	for_return += std::to_string(reg2);
	for_return += ", ";
	for_return += label;
	for_return += "\n";
	return for_return;
}

string Mips::bge(int reg1, int reg2, string label)
{
	string for_return;
	for_return.clear();
	for_return += "bge $";
	for_return += std::to_string(reg1);
	for_return += ", $";
	for_return += std::to_string(reg2);
	for_return += ", ";
	for_return += label;
	for_return += "\n";
	return for_return;
}

string Mips::bgt(int reg1, int reg2, string label)
{
	string for_return;
	for_return.clear();
	for_return += "bgt $";
	for_return += std::to_string(reg1);
	for_return += ", $";
	for_return += std::to_string(reg2);
	for_return += ", ";
	for_return += label;
	for_return += "\n";
	return for_return;
}

string Mips::ble(int reg1, int reg2, string label)
{
	string for_return;
	for_return.clear();
	for_return += "ble $";
	for_return += std::to_string(reg1);
	for_return += ", $";
	for_return += std::to_string(reg2);
	for_return += ", ";
	for_return += label;
	for_return += "\n";
	return for_return;
}

string Mips::blt(int reg1, int reg2, string label)
{
	string for_return;
	for_return.clear();
	for_return += "blt $";
	for_return += std::to_string(reg1);
	for_return += ", $";
	for_return += std::to_string(reg2);
	for_return += ", ";
	for_return += label;
	for_return += "\n";
	return for_return;
}

string Mips::jal(string label)
{
	return "jal " + label + "\n";
}

string Mips::jump(string label)
{
	return "j " + label + "\n";
}

string Mips::jr()
{
	return "jr $ra\n";
}

string Mips::add_label(string label)
{
	return label + ":\n";
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

string Mips::sll(int reg_to, int reg_from, int i)
{
	string for_return;
	for_return.clear();
	//for_return += "subi $";
	for_return += "sll $";
	for_return += std::to_string(reg_to);
	for_return += ", $";
	for_return += std::to_string(reg_from);
	for_return += ", ";
	for_return += std::to_string(i);
	for_return += "\n";
	return for_return;
}
