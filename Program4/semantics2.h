/*
Ivan Rosales
CPSC 323
Program 4 Semantics2
Visual Studio 2015 Community
Created 9-29-17 for Program 2
Edited 10-28-17 for Program 3
Edited 12-2-17 for Program 4

This used to be the parser header file that goes through the source code looking for syntax errors.
It still calls the lexer to get a lex/tok pair and then recursivly calls other fucntions
depending on the lex/tok being read. The semantics program starts out by calling the "program()"
function and then recursivly calls any sequential functions. It appends each rule to the master vector
and then testSemantics.cpp loops through the vector and prints out its contents.
*/

#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "lexer.h"
using namespace std;

//function prototypes
void Program(ifstream& src);
void DecList(ifstream& src);
void Decl(ifstream& src);
void StmtList(ifstream& src);
void Stmt(ifstream& src);
void Type(ifstream& src);
void VarList(ifstream& src);
void Assign(ifstream& src);
void Read(ifstream& src);
void Write(ifstream& src);
void If(ifstream& src);
void While(ifstream& src);
void Cond(ifstream& src);
int Expr(ifstream& src);
int Term(ifstream& src);
int Factor(ifstream& src);

//declarations
LexTok curLexTok = { "","" };
string curTok, curLex, tempLex, tempStr, temp = "endIf";
vector <string> varStr, decStr, row;
vector <int> assigned;
vector <vector<string>> master;
vector <int> patchLoc, curIf;
int counter = 0, curReg = 0, globalReg1 = 0, globalReg2 = 0, errorCount = 0, decSize;
int whileNum = 1, ifNum = 1, elseNum = 1, elseifNum = 1;

//writes out the error message
void writeError(string s) {
     cout << "error: expected " << s << " but read " << curLex << " " << curTok << endl;
}

// Gets the next lex/tok pair from the lexer and assigns it to curLexTok
// as well as assigning its lex to curLex and its tok to curTok.
void getNextLexTok(ifstream& src) {
     curLexTok = lexer(src);
     curLex = curLexTok.lexeme;
     curTok = curLexTok.token;
}

// Gets the first lex/tok pair and initiates the parser by calling the Program function.
void semantics2(ifstream& src) {
     getNextLexTok(src);
     Program(src);
}

// Checks if the current lex or tok is accepted, and if it is, returns true.
// Also gets the next lex/tok pair if the current lex or tok is accepted.
// If it doesn't accept, returns false
bool accept(string s, ifstream& src) {
     if (s == curLex || s == curTok) {
          getNextLexTok(src);
          return true;
     }
     return false;
}

// Calls the accept function to check if the current lex or tok is the one that
// is expected (accept). If it isn't expected, writes out the error message.
void expect(string s, ifstream& src) {
     if (!accept(s, src))
          writeError(s);
}


// Checks for duplicate declarations and writes out an error message
void checkDup() {
     // adds all variables from varStr to decStr
     for (int i = 0; i < varStr.size(); i++) {
          decStr.push_back(varStr[i]);
          assigned.push_back(NULL);
     }
     // checks for duplicates in decStr
     for (int i = 0; i < decStr.size()-1; i++) {
          for (int j = i + 1; j < decStr.size(); j++) {
               if (decStr[i].compare(decStr[j]) == 0) {
                    cout << "Error: " << decStr[j] << " is delcared more than once" << endl;
                    errorCount++;
               }
          }
     }
     decSize = decStr.size();
}

// Checks if a variable has been assigned a value or not and writes out an error message
void checkAssigned(string s) {
     int position;
     for (int i = 0; i < decStr.size(); i++) {
          if (s == decStr[i])
               position = i;
     }
     for (int i = 0; i < assigned.size(); i++) {
          if (assigned[position] == NULL) {
               errorCount++;
               if (errorCount == 1)
                    cout << "Error: using variable '" << decStr[position] << "' before initializing" << endl;
               
          }
     }
}

// Checks to see if a variable was declared and writes out an error message
void checkDec(string s) {
     bool declared = false;
     for (int i = 0; i < decStr.size(); i++) {
          if (s.compare(decStr[i]) == 0)
               declared = true;
     }
     if (!declared) {
          cout << "Error: Using " << s << " without declaring first" << endl;
          errorCount++;
     }
}

// calls the Program fucntion, checks if the lexems match and if it sees a
// "Type" token so to call Declist or a "function" lexeme so to call FuncList
// or other lex/tok to call StmtList or not.
void Program(ifstream& src) {
     expect("program", src);
     if (curTok == "Type") {
          row.clear();
          row.push_back("     .data");
          master.push_back(row);
          DecList(src);
     }
     if (curLex == "begin") {
          row.clear();
          row.push_back("     .text");
          master.push_back(row);
          row.clear();
          row.push_back("main:");
          master.push_back(row);
     }
     expect("begin", src);
     if (curTok == "Identifier" || curLex == "read" || curLex == "write" || curLex == "if" || curLex == "while")
          StmtList(src);
     expect("end", src);
     if (curLex == ".") {
          row.clear();
          row.push_back("     ");
          row.push_back("li ");
          row.push_back("$v0");
          row.push_back(", ");
          row.push_back("10");
          master.push_back(row);
          row.clear();
          row.push_back("     ");
          row.push_back("syscall");
          master.push_back(row);
     }
     expect(".", src);
     //master.push_back("Progream => program [Declist] begin {StmtList} end." << endl;
}

// Calls the Decl function and then keeps calling Decl if
// the next token is "Type".
void DecList(ifstream& src) {
     Decl(src);
     while (curTok == "Type")
          Decl(src);
     //master.push_back("DeclList => Decl{Decl}" << endl;
}

// calls the Type functin and then checks if the next 
// token is "Identifier" so to call the VarList function.
void Decl(ifstream& src) {
     Type(src);
     if (curTok == "Identifier")
          VarList(src);
     else
          writeError("Identifier");
     expect(";", src);
     checkDup();
     for (int i = 0; i < varStr.size(); i++) {
          row.clear();
          row.push_back(varStr[i] + ":     .word  0");
          master.push_back(row);
     }
     varStr.clear();
     //master.push_back("Decl => Type VarList" << endl;
}

// checks that curLex is "int" and then comsumes it
void Type(ifstream& src) {
     if (curLex == "int") {
          //master.push_back("Type => int" << endl;
          getNextLexTok(src);
     }
     else
          writeError("int");
}

// Checks if current token is "Identifier" and then stores the curLex 
// in the varStr vector while it keeps seeing a ","
void VarList(ifstream& src) {
     varStr.push_back(curLex);
     expect("Identifier", src);
     while (accept(",", src)) {
          varStr.push_back(curLex);
          expect("Identifier", src);
     }
     //master.push_back("VarList => Ident (, Ident)" << endl;
}

// Calls Stmt function and contiues to call Stmt while the condition is met
void StmtList(ifstream& src) {
     Stmt(src);
     while (curTok == "Identifier" || curLex == "read" || curLex == "write" || curLex == "if" || curLex == "while") {
          Stmt(src);
     }
     //master.push_back("StmtList => Stmt {Stmt}" << endl;
}

// Checks for the appropriate statment
void Stmt(ifstream& src) {
     if (curTok == "Identifier") 
          Assign(src);
     else if (curLex == "read")
          Read(src);
     else if (curLex == "write")
          Write(src);
     else if (curLex == "if") 
          If(src);
     else if (curLex == "while")
          While(src);
     else
          writeError("Identifier, read, write, if, or while");
}

//checks for an identifier and then calls Expr
void Assign(ifstream& src) {
     int ident;
     checkDec(curLex);
     for (int i = 0; i < decStr.size(); i++) {
          if (curLex == decStr[i])
               ident = i;
     }
     expect("Identifier", src);
     expect(":=", src);
     curReg = Expr(src);
     expect(";", src);
     for (int i = 0; i < assigned.size(); i++) {
          if (ident == i)
               assigned[i] = 1;
     }

     row.clear();
     row.push_back("     ");
     row.push_back("move ");
     row.push_back("$t" + to_string(ident));
     row.push_back(", ");
     row.push_back("$t" + to_string(curReg));
     master.push_back(row);

     counter = 0;
     //master.push_back("Assign => Ident := Expr ;" << endl;
}

// checks for "read" then calls VarList if an identifier is read
void Read(ifstream& src) {
     expect("read", src);
     expect("(", src);
     if (curTok == "Identifier")
          VarList(src);
     expect(")", src);
     expect(";", src);
     int tempReg;
     for (int i = 0; i < varStr.size(); i++) {
          row.clear();
          row.push_back("     ");
          row.push_back("li ");
          row.push_back("$v0");
          row.push_back(", ");
          row.push_back("5");
          master.push_back(row);
          row.clear();
          row.push_back("     ");
          row.push_back("syscall");
          master.push_back(row);
          row.clear();
          row.push_back("     ");
          row.push_back("move ");
          for (int j = 0; j < decStr.size(); j++) {
               if (varStr[i] == decStr[j])
                    tempReg = j;
          }
          row.push_back("$t" + to_string(tempReg));
          row.push_back(", ");
          row.push_back("$v0");
          master.push_back(row);
          //master.push_back("     sw $v0, " + varStr[i]);
     }
     varStr.clear();
     //master.push_back("Read => read ( VarList ) ;" << endl;
}

// calls Expr if the condition is met and continues to call Expr while
// it sees that the next lexeme is ",".
void Write(ifstream& src) {
     expect("write", src);
     expect("(", src);
     if (curTok == "Identifier" || curTok == "IntConst" || curLex == "(") {
          if (curTok == "Identifier") {
               checkDec(curLex);
               checkAssigned(curLex);
          }
          curReg = Expr(src);

          row.clear();
          row.push_back("     ");
          row.push_back("li ");
          row.push_back("$v0");
          row.push_back(", ");
          row.push_back("1");
          master.push_back(row);

          row.clear();
          row.push_back("     ");
          row.push_back("move ");
          row.push_back("$a0");
          row.push_back(", ");
          row.push_back("$t" + to_string(curReg));
          master.push_back(row);

          row.clear();
          row.push_back("     ");
          row.push_back("syscall");
          master.push_back(row);

          counter = 0;
     }
     else
          writeError("Identifier, IntConst, or (");
     while (curLex == ",") {
          getNextLexTok(src);
          while (curTok == "Identifier" || curTok == "IntConst" || curLex == "(") {
               if (curTok == "Identifier") {
                    checkDec(curLex);
                    checkAssigned(curLex);
               }
               curReg = Expr(src);

               row.clear();
               row.push_back("     ");
               row.push_back("li ");
               row.push_back("$v0");
               row.push_back(", ");
               row.push_back("1");
               master.push_back(row);

               row.clear();
               row.push_back("     ");
               row.push_back("move ");
               row.push_back("$a0");
               row.push_back(", ");
               row.push_back("$t" + to_string(curReg));
               master.push_back(row);
               
               row.clear();
               row.push_back("     ");
               row.push_back("syscall");
               master.push_back(row);

               counter = 0;
          }
     }
     expect(")", src);
     expect(";", src);
     //master.push_back("Write => write ( Expr {, Expr} ) ;" << endl;
}

// checks for if, elsif, and else statements and keeps track of how many if, elsif, and else
// statements have been called. Also keeps track of "patchloc" and patches that location depending
// on whether an elsif, else, or none follow the if statement.
// This fucntion is a huge mess, but it works
void If(ifstream& src) {
     expect("if", src);
     expect("(", src);
     if (curTok == "Identifier" || curTok == "IntConst" || curLex == "(")
          Cond(src);

     row.clear();
     row.push_back(tempStr);
     row.push_back("$t" + to_string(globalReg1));
     row.push_back(", ");
     row.push_back("$t" + to_string(globalReg2));
     row.push_back(", ");
     master.push_back(row);

     //master.push_back(tempStr + "$t" + to_string(globalReg1) + ", $t" + to_string(globalReg2) + ", ");

     patchLoc.push_back(master.size()-1);

     expect(")", src);
     expect("begin", src);
     curIf.push_back(ifNum);
     ifNum++;
     if (curTok == "Identifier" || curLex == "read" || curLex == "write" || curLex == "if" || curLex == "while")
          StmtList(src);
     expect("end", src);
     
     while (curLex == "elsif") {

          row.clear();
          row.push_back("     b endIf");
          row.push_back(to_string(curIf.back()));
          master.push_back(row);

          //master.push_back("     b endIf" + to_string(curIf.back()));

          master[patchLoc.back()].push_back("elsif" + to_string(elseifNum));

          //master[patchLoc.back()] += ("elsif" + to_string(elseifNum));

          patchLoc.pop_back();

          row.clear();
          row.push_back("elsif");
          row.push_back(to_string(elseifNum) + ":");
          master.push_back(row);

          //master.push_back("elsif" + to_string(elseifNum) + ":");

          expect("elsif", src);
          expect("(", src);
          if (curTok == "Identifier" || curTok == "IntConst" || curLex == "(")
               Cond(src);

          row.clear();
          row.push_back(tempStr);
          row.push_back("$t" + to_string(globalReg1));
          row.push_back(", ");
          row.push_back("$t" + to_string(globalReg2));
          row.push_back(", ");
          master.push_back(row);

          //master.push_back(tempStr + "$t" + to_string(globalReg1) + ", $t" + to_string(globalReg2) + ", ");

          patchLoc.push_back(master.size()-1);

          elseifNum++;
          expect(")", src);
          expect("begin", src);
          if (curTok == "Identifier" || curLex == "read" || curLex == "write" || curLex == "if" || curLex == "while")
               StmtList(src);
          expect("end", src);
     }
     if (curLex == "else") {

          row.clear();
          row.push_back("     b endIf");
          row.push_back(to_string(curIf.back()));
          master.push_back(row);

          //master.push_back("     b endIf" + to_string(curIf.back()));

          master[patchLoc.back()].push_back("else" + to_string(elseNum));

          //master[patchLoc.back()] += ("else" + to_string(elseNum));

          patchLoc.pop_back();

          row.clear();
          row.push_back("else");
          row.push_back(to_string(elseNum) + ":");
          master.push_back(row);

          //master.push_back("else" + to_string(elseNum) + ":");

          elseNum++;
          expect("else", src);
          expect("begin", src);
          if (curTok == "Identifier" || curLex == "read" || curLex == "write" || curLex == "if" || curLex == "while")
               StmtList(src);
          expect("end", src);

          row.clear();
          row.push_back("endIf");
          row.push_back(to_string(curIf.back()) + ":");
          master.push_back(row);

          //master.push_back("endIf" + to_string(curIf.back()) + ":");

     }
     else if (elseifNum == 1) {

          master[patchLoc.back()].push_back("endIf" + to_string(ifNum - 1));

          //master[patchLoc.back()] += ("endIf" + to_string(ifNum-1));

          patchLoc.pop_back();

          row.clear();
          row.push_back("endIf");
          row.push_back(to_string(curIf.back()) + ":");
          master.push_back(row);

          //master.push_back("endIf" + to_string(curIf.back()) + ":");
     }
     else {

          master[patchLoc.back()].push_back("endIf" + to_string(elseifNum - 1));

          //master[patchLoc.back()] += ("endIf" + to_string(elseifNum-1));

          patchLoc.pop_back();

          row.clear();
          row.push_back("endIf");
          row.push_back(to_string(curIf.back()) + ":");
          master.push_back(row);

          //master.push_back("endIf" + to_string(curIf.back()) + ":");
     }
     curIf.pop_back();
     //master.push_back("If => if ( Cond ) begin StmtList end {elsif ( Cond ) begin StmtList end} [else begin StmtList end]" << endl;
}

// calls cond and then checks for StmtList and keeps track of how many "whiles" have been called
void While(ifstream& src) {
     row.clear();
     row.push_back("while");
     row.push_back(to_string(whileNum) + ":");
     master.push_back(row);

     expect("while", src);
     expect("(", src);
     Cond(src);

     row.clear();
     row.push_back(tempStr);
     row.push_back("$t" + to_string(globalReg1));
     row.push_back(", ");
     row.push_back("$t" + to_string(globalReg2));
     row.push_back(", endWhile" + to_string(whileNum));
     master.push_back(row);

     whileNum++;
     expect(")", src);
     expect("begin", src);
     if (curTok == "Identifier" || curLex == "read" || curLex == "write" || curLex == "if" || curLex == "while")
          StmtList(src);
     expect("end", src);
     whileNum--;

     row.clear();
     row.push_back("     ");
     row.push_back("b while");
     row.push_back(to_string(whileNum));
     master.push_back(row);

     row.clear();
     row.push_back("endWhile");
     row.push_back(to_string(whileNum) + ":");
     master.push_back(row);

     //master.push_back("While => while ( Cond ) begin [StmtList] end" << endl;
}

// checks for RelOp
void Cond(ifstream& src) {
     globalReg1 = Expr(src);
     if (curLex == ">" || curLex == "<" || curLex == ">=" || curLex == "<=" || curLex == "=" || curLex == "<>") {
          tempLex = curLex;
          getNextLexTok(src);
     }
     else
          writeError("RelOp");
     globalReg2 = Expr(src);
     if (tempLex == ">")
          tempStr = "     ble ";
     else if (tempLex == "<")
          tempStr = "     bge ";
     else if (tempLex == ">=")
          tempStr = "     blt ";
     else if (tempLex == "<=")
          tempStr = "     bgt ";
     else if (tempLex == "=")
          tempStr = "     bne ";
     else if (tempLex == "<>")
          tempStr = "     beq ";
     else
          writeError("RelOp");

     counter = 0;
     //master.push_back("Cond => Expr RelOp Expr" << endl;
}

// calls Term and then checks for + or - and then calls Term again
int Expr(ifstream& src) {
     int reg1 = Term(src);
     while (curLex == "+") {
          getNextLexTok(src);
          int reg2 = Term(src);

          if (counter > 1)
               counter = 0;

          row.clear();
          row.push_back("     ");
          row.push_back("add ");
          row.push_back("$t" + to_string(decSize + counter));
          row.push_back(", ");
          row.push_back("$t" + to_string(reg1));
          row.push_back(", ");
          row.push_back("$t" + to_string(reg2));
          master.push_back(row);

          counter++;
          return decSize + counter - 1;
     }
     while (curLex == "-") {
          getNextLexTok(src);
          int reg2 = Term(src);

          if (counter > 1)
               counter = 0;

          row.clear();
          row.push_back("     ");
          row.push_back("sub ");
          row.push_back("$t" + to_string(decSize + counter));
          row.push_back(", ");
          row.push_back("$t" + to_string(reg1));
          row.push_back(", ");
          row.push_back("$t" + to_string(reg2));
          master.push_back(row);

          counter++;
          return decSize + counter - 1;
     }
     //master.push_back("Expr => Term {(+|-) Term}" << endl;
     return reg1;
}

// calls Factor and then checks for * or / and then calls Factor again
int Term(ifstream& src) {
     int reg1 = Factor(src);
     while (curLex == "*") {
          getNextLexTok(src);
          int reg2 = Factor(src);

          if (counter > 1)
               counter = 0;

          row.clear();
          row.push_back("     ");
          row.push_back("mult ");
          row.push_back("$t" + to_string(reg1));
          row.push_back(", ");
          row.push_back("$t" + to_string(reg2));
          master.push_back(row);

          row.clear();
          row.push_back("     ");
          row.push_back("mflo ");
          row.push_back("$t" + to_string(decSize + counter));
          master.push_back(row);

          counter++;
          return decSize + counter - 1;
     }
     while (curLex == "/") {
          getNextLexTok(src);
          int reg2 = Factor(src);

          if (counter > 1)
               counter = 0;

          row.clear();
          row.push_back("     ");
          row.push_back("div ");
          row.push_back("$t" + to_string(reg1));
          row.push_back(", ");
          row.push_back("$t" + to_string(reg2));
          master.push_back(row);

          row.clear();
          row.push_back("     ");
          row.push_back("mflo ");
          row.push_back("$t" + to_string(decSize + counter));
          master.push_back(row);

          counter++;
          return decSize + counter - 1;
     }
     return reg1;
     //master.push_back("Term => Factor {(*|/) Factor}" << endl;
}

// checks for the appropriate Factor rule ( ident, intConst, or "(" )
int Factor(ifstream& src) {
     int tempReg;
     if (curTok == "Identifier") {
          checkAssigned(curLex);
          for (int i = 0; i < decStr.size(); i++) {
               if (curLex == decStr[i])
                    tempReg = i;
          }
          getNextLexTok(src);
          return tempReg;
          //master.push_back("Factor => Ident" << endl;
     }
     else if (curTok == "IntConst") {
          tempReg = decSize + counter;
          counter++;

          row.clear();
          row.push_back("     ");
          row.push_back("li ");
          row.push_back("$t" + to_string(tempReg));
          row.push_back(", ");
          row.push_back(curLex);
          master.push_back(row);

          getNextLexTok(src);
          return tempReg;
          //master.push_back("Factor => IntConst" << endl;
     }
     else if (curLex == "(") {
          getNextLexTok(src);
          if (curTok == "Identifier" || curTok == "IntConst" || curLex == "(")
               tempReg = Expr(src);
          expect(")", src);
          return tempReg;
          //master.push_back("Factor => ( Expr )" << endl;
     }
     else {
          writeError("identifier or intConst or '('");
          return 1;
     }
}
