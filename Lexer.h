#ifndef _LEXER_H_
#define _LEXER_H_
#include <string>
enum class Token{
    tok_eof=-1,

    tok_def=-2,
    tok_extern=-3,

    tok_identifier=-4,
    tok_number=-5,
};

class Lexer{

public:
    int LastChar;
    Lexer();
    std::string IdentifierStr;
    double NumVal;
    // Token token;
    int  gettok();
};


#endif 