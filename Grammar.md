# 词法分析阶段设计文档
## 输入输出要求摘录

| 单词名称 | 类别码 | 单词名称 | 类别码 | 单词名称 | 类别码 | 单词名称 | 类别码 |
| ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
| 标识符 | IDENFR | else | ELSETK | - | MINU | = | ASSIGN |
| 整形常量 | INTCON | switch | SWITCHTK | * | MULT | ; | SEMICN |
| 字符常量 | CHARCON | case | CASETK | / | DIV | , | COMMA |
| 字符串 | STRCON | default | DEFAULTTK | < | LSS | ( | LPARENT | 
| const | CONSTTK | while | WHILETK | <= | LEQ | ) | RPARENT |
| int | INTTK | for | FORTK | > | GRE | [ | LBRACK |
| char | CHARTK | scanf | SCANFTK | >= | GEQ | ] | RBRACK |
| void | VOIDTK | printf | PRINTFTK | == | EQL | { | LBRACE |
| main | MAINTK | return | RETURNTK | != | NEQ | } | RBRACE |
| if | IFTK | + | PLUS | ： | COLON | 


- 输入：`testfile.txt`
- 输出：`output.txt`

## 程序环境
- 语言：C++
- IDE：VS2019
- 测试环境：VS自带编译器、C++ 14
- 目标环境：Clang 8.0.1、C++ 11

## 程序设计
### 编码前设计
设计单一工具类，由主函数读取文件后，传字符串给工具类并进行分析，分析得到单词及其类别码后直接输出到文件。
### 编码后设计
- `TypeEnum.h`
    枚举类，用于记录所有的单纯类别码；
- `WordInfo.h`; `WordInfo.cpp`
    单词信息封装类；内含：
    + 单词类别码
    + 单词对应的字符串
    + 单词所在行数
    + 单词位于所在行的位置
- `Lexer.h`; `Lexer.cpp`
    对词法分析的工具类，对读到的单词用`WordInfo`封装并存储在`vector`中；
    传入输入文件路径开始分析并存储，传入输出文件路径对读取到的单词信息进行输出。
- `main.cpp`
    主函数，传输输入、输出文件路径给工具类。