#pragma once

#include <string>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#include "AST.h"
#include "SymbolTable.h"

namespace vcalc {
    class LLVMIRGenerator {
    public:
        llvm::LLVMContext globalCtx;
        llvm::IRBuilder<> ir;
        llvm::Module mod;
        llvm::Function *mainFunction;
        size_t numBasicBlocks;
        size_t numVariables;
        size_t numExprAncestors;

        std::string &outputFileName;
        LLVMIRGenerator(std::string &outputFileName);
        void visit(std::shared_ptr<AST> t);
        void visitChildren(std::shared_ptr<AST> t);
        void visitBLOCK_TOKEN(std::shared_ptr<AST> t);
        void visitVAR_DECLARATION_TOKEN(std::shared_ptr<AST> t);
        void visitASSIGNMENT_TOKEN(std::shared_ptr<AST> t);
        void visitBinaryOperationToken(std::shared_ptr<AST> t);
        void visitRANGE(std::shared_ptr<AST> t);
        void visitGENERATOR_TOKEN(std::shared_ptr<AST> t);
        void visitFILTER_TOKEN(std::shared_ptr<AST> t);
        void visitID(std::shared_ptr<AST> t);
        void visitINTEGER(std::shared_ptr<AST> t);
        void visitPARENTHESIS_TOKEN(std::shared_ptr<AST> t);
        void visitPRINT(std::shared_ptr<AST> t);
        void visitEXPR_TOKEN(std::shared_ptr<AST> t);
        void visitLOOP_TOKEN(std::shared_ptr<AST> t);
        void visitCONDITIONAL_TOKEN(std::shared_ptr<AST> t);
        void visitINDEX_TOKEN(std::shared_ptr<AST> t);
    };
}