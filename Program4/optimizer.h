/*
Ivan Rosales
CPSC 323
Program 4 Optimizer
Visual Studio 2015 Community
Created 12-2-17

This functions loops through the master 2D vector and assigns the start of a bock to
the lines on which labels are located and the line after branch call statements. It 
then removes any unnecessary move statements within their respective blocks.
*/

#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "semantics2.h"
using namespace std;

vector <int> blocks;
string currentReg, checkFor;
int breakCount = 0, curLine, jumpLine;

// loops through the master 2D vector and removes unnecessary move instructions
void optimize(vector <vector <string>> &master) {
     for (int i = 0; i < master.size(); i++) {
          for (int j = 0; j < master[i].size(); j++) {
               // checks for labels and assigns the line they are on as the start of a block
               if (master[i][j] == "endIf" || master[i][j] == "else" || master[i][j] == "while" || master[i][j] == "main:"){
                    blocks.push_back(i);
               }
               // checks for branch jumps and assigns the next line as the start of a block
               if (master[i][j] == "     ble " || master[i][j] == "     bge " || master[i][j] == "     blt " ||
                    master[i][j] == "     bgt " || master[i][j] == "     bne " || master[i][j] == "     beq ") {
                    blocks.push_back(i + 1);
               }
               // removes move statements right after add and sub statements
               if ((master[i][j] == "add " || master[i][j] == "sub ") &&
                    master[i + 1][j] == "move " && master[i + 1][j + 1] != "$a0" && master[i + 1][j + 3] != "$v0") {
                    master[i][j + 1] = master[i + 1][2];
                    master.erase(master.begin() + i + 1);
               }
               // removes any other unnecessary move statements within their respective block
               else if (master[i][j] == "move " && master[i][j + 1] != "$a0" && master[i][j + 3] != "$v0") {
                    curLine = i;
                    currentReg = master[i][j + 1];
                    checkFor = master[i][j + 3];
                    jumpLine = blocks.back();
                    for (int k = jumpLine; k < curLine; k++) {
                         for (int l = 0; l < master[k].size(); l++) {
                              if (master[k][l] == checkFor)
                                   master[k][l] = currentReg;
                         }
                    }
                    master.erase(master.begin() + curLine);
               }
          }
     }
}

