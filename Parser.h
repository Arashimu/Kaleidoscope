#ifndef _PARSER_H_
#define _PARSER_H_
#include <string>
#include "Lexer.h"
#include <memory>
#include <map>
#include <vector>
#include "AST.h"

class Parser{
private:
    Lexer lexer;
    int CurTok;
    std::map<char,int>BinOpPrecedence;
public:
    Parser(Lexer lexer):lexer(lexer){
         BinOpPrecedence['<'] = 10;
         BinOpPrecedence['>'] = 10;
         BinOpPrecedence['+'] = 20;
         BinOpPrecedence['-'] = 20;
         BinOpPrecedence['*'] = 40;   
         BinOpPrecedence['/'] = 40;  
    }
    int getNextToken();
    int GetTokPrecedence();
    /*
    numberexpr : number
    */
    std::unique_ptr<ExprAST> ParseNumberExpr();
    /*
    parenexpr : '(' expression ')'
    */
    std::unique_ptr<ExprAST> ParseParenExpr();
    /*
    identifierexpr: identifier
                | identifier '(' expression* ')'
    */
    std::unique_ptr<ExprAST> ParseIdentifierExpr();
    /*
    primary: identifierexpr
            | numberxpr
            | parenexpr
    */
    std::unique_ptr<ExprAST> ParsePrimary();
    /*
    expression:primary binoprhs primary
    */
    std::unique_ptr<ExprAST> ParseExpression();
    /*
     prototype : id '(' id* ')'
    */
    std::unique_ptr<PrototypeAST> ParsePrototype();

    std::unique_ptr<ExprAST> ParseBinOpRHS(int ExprPrec,std::unique_ptr<ExprAST>LHS);
    /*
    definition : 'def' prototype expression
    */
    std::unique_ptr<FunctionAST> ParseDefinition();
    /*
    external : 'extern' prototype
    */
    std::unique_ptr<PrototypeAST> ParseExtern();
    /*
    toplevelexpr : expression
    */
    std::unique_ptr<FunctionAST> ParseTopLevelExpr();
    void HandleDefinition();
    void HandleExtern();
    void HandleTopLevelExpression();
    void Start();
};





#endif