#pragma once
#include <string>
using namespace std;
class Mips
{
public:
	static string li(int reg, int value);
	static string la(int reg, string label);
	static string la_l_r(int reg, string label, int reg_offset);
	static string move(int reg_to, int reg_from);
	static string lw(int reg_to, int offset, int reg_addr);
	static string sw(int reg_from, int offset, int reg_addr);
	static string sw(int reg_from, string label);
	static string add(int reg_to, int reg_1, int reg_2);
	static string addi(int reg_to, int reg_from, int i);
	static string sub(int reg_to, int reg_1, int reg_2);
	static string subi(int reg_to, int reg_from, int i);
	static string mul(int reg_to, int reg_1, int reg_2);
	static string muli(int reg_to, int reg_from, int i);
	static string div(int reg_to, int reg_1, int reg_2);
	static string divi(int reg_to, int reg_from, int i);
	static string neg(int reg_to, int reg_from);
	static string beq(int reg1, int reg2, string label);
	static string beqi(int reg1, int i, string label);
	static string bne(int reg1, int reg2, string label);
	static string bge(int reg1, int reg2, string label);
	static string bgt(int reg1, int reg2, string label);
	static string ble(int reg1, int reg2, string label);
	static string blt(int reg1, int reg2, string label);
	static string jal(string label);
	static string jump(string label);
	static string jr();
	static string add_label(string label);
	static string syscall(int code_in_v0);
};

