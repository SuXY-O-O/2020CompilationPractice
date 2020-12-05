#pragma once
#include <string>
using namespace std;
class Mips
{
public:
	static string li(int reg, int value);
	static string la(int reg, string label);
	static string move(int reg_to, int reg_from);
	static string lw(int reg_to, int offset, int reg_addr);
	static string sw(int reg_from, int offset, int reg_addr);
	static string sw(int reg_from, string label);
	static string add(int reg_to, int reg_1, int reg_2);
	static string addi(int reg_to, int reg_from, int i);
	static string sub(int reg_to, int reg_1, int reg_2);
	static string mul(int reg_to, int reg_1, int reg_2);
	static string div(int reg_to, int reg_1, int reg_2);
	static string neg(int reg_to, int reg_from);
	static string jal(string label);
	static string jr();
	static string syscall(int code_in_v0);
};

