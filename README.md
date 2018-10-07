# Compiler-Language-Converter
This program parses through a custom, simplified high-level language and then converts it to SPIM code. 
It also goes through the SPIM code and optimizes it by removing any unnecessary move statements.

The programs takes in a file name as a command line arguement and then it parses through the file.
It checks for any syntax errors and if none are found, it converts the custom language into SPIM code.
It then parses through the SPIM code aswell and removes any uncesessary move statements before 
returning the newly optimized SPIM code.
