/*
Ivan Rosales
CPSC 323
Program 3 Main
Visual Studio 2015 Community
Created 10-28-17
Edited 12-2-17

This is the main program for the semantics2 and optimzer header files. It takes in a file name
as a command line arguement and the calls the semantics2 header file to check the source code.
It then calls the optimizer header file to remove any unnecessary move statements. 
If the error count is 0, it loops through the master vector and prints out its contents.
*/

#include <fstream>
#include <iostream>
#include <string>
#include "semantics2.h"
#include "optimizer.h"
using namespace std;

int main(int argc, char *argv[]) {

     //checks if there is command line input
     if (argc < 2) {
          cout << "missing filename" << endl;
          return 1;
     }
     ifstream fin(argv[1]);

     //checks to see if the file can be opened
     if (!fin) {
          cout << "file not found" << endl;
          return 1;
     }
     //calls the parser to check the source code
     semantics2(fin);
     optimize(master);

     if (errorCount == 0) {
          for (int i = 0; i < master.size(); i++) {
               for (int j = 0; j < master[i].size(); j++)
                    cout << master[i][j];
               cout << endl;
          }
     }
     //closes the file
     fin.close();

     //system("pause")
     return 0;
}