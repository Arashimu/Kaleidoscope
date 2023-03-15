#include "AST.h"

std::unique_ptr<ExprAST> LogError(const char *Str){
    fprintf(stderr,"Error: %s\n",Str);
    return nullptr;
}
std::unique_ptr<PrototypeAST> LogErrorP(const char *Str) {
  LogError(Str);
  return nullptr;
}

Value *LogErrorV(const char *Str) {
  LogError(Str);
  return nullptr;
}

Value *NumberExprAST::codegen(){
    return ConstantFP::get(*TheContext,APFloat(Val));
}
Value *VariableExprAST::codegen(){
    Value *V=NamedValues[Name];
    if(!V){
        LogErrorV("Unknown variable name");
    }
    return V;
}
Value *BinaryExprAST::codegen(){
    Value *L=LHS->codegen();
    Value *R=RHS->codegen();
    if(!L||!R) return nullptr;
    switch(Op){
        case '+':
            return Builder->CreateFAdd(L,R,"add");
        case '-':
            return Builder->CreateFSub(L,R,"sub");
        case '*':
            return Builder->CreateFMul(L,R,"mul");
        case '/':
            return Builder->CreateFDiv(L,R,"div");
        case '<':
            L=Builder->CreateFCmpULT(L,R,"cmplt");
            return Builder->CreateUIToFP(L,Type::getDoubleTy(*TheContext),"bool");
        default:
            return LogErrorV("invalid binary operator");
    }   
}
Value *CallExprAST::codegen(){
    Function *CalleeF=TheModule->getFunction(Callee);
    if(!CalleeF)
        return LogErrorV("Unknow function referenced");
    if(CalleeF->arg_size()!=Args.size())
        return LogErrorV("Incorrect arguments passed");
    
    std::vector<Value*>ArgsV;
    for(unsigned i=0,e=Args.size();i!=e;i++){
        ArgsV.push_back(Args[i]->codegen());
        if(!ArgsV.back())
            return nullptr;
    }
    return Builder->CreateCall(CalleeF,ArgsV,"call");
}
Function *PrototypeAST::codegen(){
    std::vector<Type*>Doubles(Args.size(),Type::getDoubleTy(*TheContext));
    FunctionType *FT=FunctionType::get(Type::getDoubleTy(*TheContext),Doubles,false);
    Function *F=Function::Create(FT,Function::ExternalLinkage,Name,TheModule.get());
    unsigned Idx=0;
    for(auto &Arg:F->args())
        Arg.setName(Args[Idx++]);
    return F;
}
Function *FunctionAST::codegen(){
    Function *TheFunction=TheModule->getFunction(Proto->getName());
    if(!TheFunction) TheFunction=Proto->codegen();
    if(!TheFunction) return nullptr;
    if(!TheFunction->empty()) return (Function*)LogErrorV("Function cannot be redefined");
    BasicBlock *BB=BasicBlock::Create(*TheContext,"entry",TheFunction);
    Builder->SetInsertPoint(BB);
    NamedValues.clear();
    for(auto &Arg:TheFunction->args())
        NamedValues[std::string(Arg.getName())]=&Arg;
    if(Value *RetVal=Body->codegen()){
         Builder->CreateRet(RetVal);
         verifyFunction(*TheFunction);
         return TheFunction;
    }
    TheFunction->eraseFromParent();
    return nullptr;
}

void InitializeModule(){
    TheContext=std::make_unique<LLVMContext>();
    TheModule=std::make_unique<Module>("my cool jit",*TheContext);

    Builder=std::make_unique<IRBuilder<>>(*TheContext);
}