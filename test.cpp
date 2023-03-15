#include "Parser.h"
#include <iostream>
int main(){
    Lexer lexer;
    Parser parser(lexer);
    parser.Start();
}