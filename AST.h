#ifndef _AST_H_
#define _AST_H_

#include <string>
#include <memory>
#include <map>
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
using namespace llvm;
class ExprAST{
public:
    virtual ~ExprAST()=default;
    virtual Value *codegen()=0;    
};

class NumberExprAST:public ExprAST{
private:
    double Val;
public:
    NumberExprAST(double Val):Val(Val){}
    Value *codegen() override;
};

class VariableExprAST:public ExprAST{
private:
    std::string Name;
public:
    VariableExprAST(const std::string &Name):Name(Name){}
    Value *codegen() override;
};

class BinaryExprAST:public ExprAST{
private:
    char Op;
    std::unique_ptr<ExprAST>LHS,RHS;
public:
    BinaryExprAST(char Op,std::unique_ptr<ExprAST>LHS,std::unique_ptr<ExprAST>RHS)
    :Op(Op),LHS(std::move(LHS)),RHS(std::move(RHS)){}
    Value *codegen() override;
};

class CallExprAST:public ExprAST{
private:
    std::string Callee;
    std::vector<std::unique_ptr<ExprAST>>Args;
public:
    CallExprAST(const std::string &Callee,std::vector<std::unique_ptr<ExprAST>>Args)
    :Callee(Callee),Args(std::move(Args)){}
    Value *codegen() override;

};

class PrototypeAST{
private:
    std::string Name;
    std::vector<std::string> Args;
public:
    PrototypeAST(const std::string &Name, std::vector<std::string> Args)
    : Name(Name), Args(std::move(Args)) {}
    Function *codegen() ;
    const std::string &getName() const{
        return Name;
    }
};

class FunctionAST{
private:
    std::unique_ptr<PrototypeAST>Proto;
    std::unique_ptr<ExprAST>Body;
public:
    FunctionAST(std::unique_ptr<PrototypeAST>Proto,std::unique_ptr<ExprAST>Body)
    :Proto(std::move(Proto)),Body(std::move(Body)){}
    Function *codegen() ;
};

static std::unique_ptr<LLVMContext> TheContext;
static std::unique_ptr<IRBuilder<>> Builder;
static std::unique_ptr<Module> TheModule;
static std::map<std::string,Value*>NamedValues;
void InitializeModule();


std::unique_ptr<ExprAST> LogError(const char *Str);
std::unique_ptr<PrototypeAST> LogErrorP(const char *Str);
Value *LogErrorV(const char *Str);

#endif