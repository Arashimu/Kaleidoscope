#include "Parser.h"

int Parser::getNextToken(){
    return CurTok=lexer.gettok();
    // CurTok=lexer.gettok();
    // fprintf(stdout,"%d ",CurTok);
    // return CurTok;
}
int Parser::GetTokPrecedence(){
    if(!isascii(CurTok)) return -1;
    // if(BinOpPrecedence.find(CurTok)==BinOpPrecedence.end()){
    //     LogError("unexpected binary operator");
    //     return -1;
    // }
    int TokPrec=BinOpPrecedence[CurTok];
    if(TokPrec<=0) return -1;
    return TokPrec;
    
}
/*
    numberexpr : number
    */
std::unique_ptr<ExprAST> Parser::ParseNumberExpr(){
    auto Result=std::make_unique<NumberExprAST>(lexer.NumVal);
    getNextToken();
    return std::move(Result);
}
/*
    parenexpr : '(' expression ')'
*/
std::unique_ptr<ExprAST> Parser::ParseParenExpr(){
    //先解析到了'('，然后进入ParseParenExpr
    getNextToken();
    auto V=ParseExpression();
    if(!V){
        return nullptr;
    }
    if(CurTok!=')'){
        return LogError("expected )");
    }
    getNextToken();
    return V;
}
/*
identifierexpr: identifier
            | identifier '(' expression* ')'
*/
std::unique_ptr<ExprAST> Parser::ParseIdentifierExpr(){
    std::string IdName=lexer.IdentifierStr;
    getNextToken();
    if(CurTok!='(')
        return std::make_unique<VariableExprAST>(IdName);
    //call
    getNextToken();
    std::vector<std::unique_ptr<ExprAST>>Args;
    if(CurTok!=')'){
        while(1){
            if(auto Arg=ParseExpression())
                Args.push_back(std::move(Arg));
            else
                return nullptr;
            if(CurTok==')') 
                break;
            if(CurTok!=',') 
                return LogError("Expected ')' or ',' in argument list" );
            getNextToken();
        }
    }
    getNextToken();
    return std::make_unique<CallExprAST>(IdName,std::move(Args));
}
/*
primary: identifierexpr
        | numberxpr
        | parenexpr
*/
std::unique_ptr<ExprAST> Parser::ParsePrimary(){
    switch(CurTok){
        case int(Token::tok_identifier):
            return ParseIdentifierExpr();
        case int(Token::tok_number):
            return ParseNumberExpr();
        case '(':
            return ParseParenExpr();
        default:
            return LogError("unknow token when expecting an expression");
        
    }
}
std::unique_ptr<ExprAST> Parser::ParseExpression(){
    auto LHS=ParsePrimary();
    if(!LHS) return nullptr;
    return ParseBinOpRHS(0,std::move(LHS));
}

std::unique_ptr<ExprAST> Parser::ParseBinOpRHS(int ExprPrec,std::unique_ptr<ExprAST>LHS){
    while(true){
        int TokPrec=GetTokPrecedence();
        if(TokPrec<ExprPrec) 
            return LHS;

        int BinOp=CurTok;
        getNextToken();
        auto RHS=ParsePrimary();
        if(!RHS) return nullptr;
        int NextPrec=GetTokPrecedence();
        if(TokPrec<NextPrec){
            RHS=ParseBinOpRHS(TokPrec+1,std::move(RHS));
            if(!RHS) return nullptr;
        }
        LHS=std::make_unique<BinaryExprAST>(BinOp,std::move(LHS),std::move(RHS));
    }
}

std::unique_ptr<PrototypeAST> Parser::ParsePrototype(){
    if(CurTok!=int(Token::tok_identifier)) 
        return LogErrorP("Expected function name in prototype");
    
    std::string FnName=lexer.IdentifierStr;
    getNextToken();
    if(CurTok!='(')
        return LogErrorP("Expected '(' in prototype");
    
    std::vector<std::string> ArgNames;
    while(getNextToken()==int(Token::tok_identifier)){
        ArgNames.push_back(lexer.IdentifierStr);
    }
    if(CurTok!=')')
        return LogErrorP("Expected ')' in prototype");
    getNextToken();
    return std::make_unique<PrototypeAST>(FnName,std::move(ArgNames));
}

std::unique_ptr<FunctionAST> Parser::ParseDefinition(){
    getNextToken();
    auto Proto=ParsePrototype();
    if(!Proto) return nullptr;
    if(auto E=ParseExpression())
        return std::make_unique<FunctionAST>(std::move(Proto),std::move(E));
    return nullptr;
}
std::unique_ptr<PrototypeAST> Parser::ParseExtern(){
    getNextToken();
    return ParsePrototype();
}
std::unique_ptr<FunctionAST> Parser::ParseTopLevelExpr(){
    if(auto E=ParseExpression()){
        auto Proto=std::make_unique<PrototypeAST>("__anon_expr",std::vector<std::string>());
        return std::make_unique<FunctionAST>(std::move(Proto),std::move(E));
    }
    return nullptr;
}
void Parser::HandleDefinition(){
    if (auto FnAST=ParseDefinition()) {
        if(auto *FnIR=FnAST->codegen()){
            fprintf(stderr, "Read function definition:\n");
            FnIR->print(errs());
            fprintf(stderr,"\n");
        }
    } else {
        // Skip token for error recovery.
        getNextToken();
    }
}
void Parser::HandleExtern(){
    if (auto ProtoAST=ParseExtern()) {
        if(auto *FnIR=ProtoAST->codegen()){
            fprintf(stderr, "Read extern:\n");
            FnIR->print(errs());
            fprintf(stderr,"\n");
        }
    } else {
        // Skip token for error recovery.
        getNextToken();
    }   
}
void Parser::HandleTopLevelExpression(){
    if (auto FnAST=ParseTopLevelExpr()) {
        if(auto *FnIR=FnAST->codegen()){
            fprintf(stderr, "Read top-level expression:\n");
            FnIR->print(errs());
            fprintf(stderr,"\n");
            FnIR->eraseFromParent();
        }
    } else {
        // Skip token for error recovery.
        getNextToken();
    }
}
void Parser::Start(){
    InitializeModule();
    //fprintf(stdout,"ready> ");
    getNextToken();
    while(true){
        //fprintf(stdout,"ready> ");
        switch(CurTok){
            case int(Token::tok_eof): 
                return;
            case ';':
                getNextToken();
                break;
            case int(Token::tok_def):
                HandleDefinition();
                break;
            case int(Token::tok_extern):
                HandleExtern();
                break;
            default:
                HandleTopLevelExpression();
                break;
                
        }
    }
}