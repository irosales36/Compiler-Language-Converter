/* 
Ivan Rosales
CPSC 323
Program 1 lexer
Visual Studio 2015 Community
Created 9-11-17

This is a lexer header that contains a struct LexTok with lexeme and token pair
and the fuction "lexer", which returns a LexTok from the ifstream.
*/

#pragma once
#include <fstream>
#include <string>
#include <cctype>
using namespace std;

struct LexTok {
     string lexeme, token;
};
     LexTok curlex;
     string keywords[13] = { "program", "begin", "end", "function", "read", "write", "if", "elsif", "else", "while", "do", "return", "until" };
     string types[3] = { "int", "real", "string" };
     char op[11] = { '+', '-', '*', '/', '(', ')', ';', ',', '.', ':' };

LexTok lexer(ifstream& src){
     curlex = { "", "" };
     string lexstr;
     char ch;

     if (src >> ch) {
          lexstr += ch;
          //checks if lexstr is an operator
          for (int i = 0; i < 10; i++) {
               if (ch == op[i]) {
                    curlex.lexeme = lexstr;
                    ch = src.peek();
                    if (lexstr == ":" && ch == '=') {
                         ch = src.get();
                         lexstr += ch;
                         curlex.lexeme = lexstr;
                    }
                    curlex.token = "Operator";
                    return curlex;
               }
          }
          //checks if lexstr is an alpha character
          if (isalpha(ch)) {
               ch = src.peek();
               while (isalpha(ch) || isdigit(ch) || ch == '_') {
                    ch = src.get();
                    lexstr += ch;
                    ch = src.peek();
               }
               curlex.lexeme = lexstr;
               //checks if lexstr if a type
               for (int i = 0; i < 3; i++) {
                    if (lexstr.compare(types[i]) == 0) {
                         curlex.token = "Type";
                         return curlex;
                    }
               }
               //checks if lexstr is a keyword
               for (int i = 0; i < 13; i++) {
                    if (lexstr.compare(keywords[i]) == 0) {
                         curlex.token = "Keyword";
                         return curlex;
                    }
               }
               //defaults to identifier if not a type or keyword
               curlex.token = "Identifier";
               return curlex;
          }
          //checks if lexstr is a digit
          else if (isdigit(ch)) {
               ch = src.peek();
               while (isdigit(ch)) {
                    ch = src.get();
                    lexstr += ch;
                    ch = src.peek();
               }
               curlex.lexeme = lexstr;
               curlex.token = "IntConst";
               //checks if lexstr is a real constant
               if (ch == '.') {
                    ch = src.get();
                    ch = src.peek();
                    if (isdigit(ch)) {
                         lexstr += '.';
                         ch = src.get();
                         lexstr += ch;
                         ch = src.peek();
                         while (isdigit(ch)) {
                              ch = src.get();
                              lexstr += ch;
                              ch = src.peek();
                         }
                         curlex.lexeme = lexstr;
                         curlex.token = "RealConst";
                         return curlex;
                    }
                    else {
                         src.putback(ch);
                         curlex.lexeme = lexstr;
                         curlex.token = "IntConst";
                         return curlex;
                    }
               }
          }
          //checks if lexstr is a string
          else if (ch == '"') {
               ch = src.peek();
               while (ch != '"' && src.eof() != true) {
                    ch = src.get();
                    lexstr += ch;
                    ch = src.peek();
               }
               if (ch == '"') {
                    ch = src.get();
                    lexstr += ch;
                    curlex.lexeme = lexstr;
                    curlex.token = "StrConst";
                    return curlex;
               }
               else if (src.eof() == true) {
                    curlex.lexeme = lexstr;
                    curlex.token = "error";
                    return curlex;
               }
          }
          //checks if lexstr is a relational operator
          else if (ch == '=') {
               curlex.lexeme = "=";
               curlex.token = "RelOp";
               return curlex;
          }
          else if (ch == '>') {
               ch = src.peek();
               if (ch == '=') {
                    ch = src.get();
                    curlex.lexeme = ">=";
                    curlex.token = "RelOp";
                    return curlex;
               }
               else {
                    curlex.lexeme = ">";
                    curlex.token = "RelOp";
                    return curlex;
               }
          }
          else if (ch == '<') {
               ch = src.peek();
               if (ch == '=') {
                    ch = src.get();
                    curlex.lexeme = "<=";
                    curlex.token = "RelOp";
                    return curlex;
               }
               else if (ch == '>') {
                    ch = src.get();
                    curlex.lexeme = "<>";
                    curlex.token = "RelOp";
                    return curlex;
               }
               else {
                    curlex.lexeme = "<";
                    curlex.token = "RelOp";
                    return curlex;
               }
          }
          else {
               curlex.lexeme = lexstr;
               curlex.token = "error";
               return curlex;
          }
     }
     //checks if src has reached the end of file
     else if (src.eof() == true){
          curlex.lexeme = "";
          curlex.token = "eof";
          return curlex;
     }
     //outputs anything else as an error
     else {
          curlex.lexeme = "";
          curlex.token = "error";
          return curlex;
     }
     return curlex;
}