#pragma once

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
        std::shared_ptr<SymbolTable> symtab;
        size_t numExprAncestors;
        LLVMIRGenerator(std::shared_ptr<SymbolTable> symtab);
        void visit(std::shared_ptr<AST> t);
        void visitChildren(std::shared_ptr<AST> t);
        void visitBLOCK_TOKEN(std::shared_ptr<AST> t);
        void visitVAR_DECLARATION_TOKEN(std::shared_ptr<AST> t);
        void visitASSIGNMENT_TOKEN(std::shared_ptr<AST> t);
        void visitGENERATOR_TOKEN(std::shared_ptr<AST> t);
        void visitFILTER_TOKEN(std::shared_ptr<AST> t);
        void visitID(std::shared_ptr<AST> t);
    };
}