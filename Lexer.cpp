#include "Lexer.h"
#include <cstring>
#include <string>
#include <iostream>

Lexer::Lexer(){
    LastChar=' ';
}
int Lexer::gettok(){
    while(isspace(LastChar)){
        LastChar=getchar();
    }
    if(isalpha(LastChar)){
        IdentifierStr=LastChar;
        while(isalnum((LastChar=getchar())))
            IdentifierStr+=LastChar;
        if(IdentifierStr=="def")
            return int(Token::tok_def);
        if(IdentifierStr=="extern")
            return int(Token::tok_extern);
        return int(Token::tok_identifier);
    }
    if(isdigit(LastChar)||LastChar=='.'){
        std::string NumStr;
        do{
            NumStr+=LastChar;
            LastChar=getchar();
        }while(isdigit(LastChar)||LastChar=='.');
        NumVal=strtod(NumStr.c_str(),0);
        return int(Token::tok_number);
    }
    if(LastChar=='#'){
        do{
            LastChar=getchar();
        }while(LastChar!=EOF && LastChar!='\n' && LastChar!='\r');
        if(LastChar!=EOF) return gettok();
    }
    if(LastChar==EOF || LastChar=='\n') return int(Token::tok_eof);
    int ThisChar=LastChar;
    LastChar=getchar();  //每次读完一个token之后需要再往下读一个字符
    return ThisChar;
}

