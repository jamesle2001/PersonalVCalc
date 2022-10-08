#pragma once

#include "AST.h"
#include "SymbolTable.h"

class DefRef {
private:
    std::shared_ptr<Scope> currentScope;
    std::shared_ptr<Type> resolveType(std::shared_ptr<AST> t);
    size_t numExprAncestors;
public:
    DefRef(std::shared_ptr<SymbolTable> symtab);
    void visit(std::shared_ptr<AST> t);
    void visitChildren(std::shared_ptr<AST> t);
    void visitBLOCK_TOKEN(std::shared_ptr<AST> t);
    void visitVAR_DECLARATION_TOKEN(std::shared_ptr<AST> t);
    void visitASSIGNMENT_TOKEN(std::shared_ptr<AST> t);
    void visitGENERATOR_TOKEN(std::shared_ptr<AST> t);
    void visitFILTER_TOKEN(std::shared_ptr<AST> t);
    void visitID(std::shared_ptr<AST> t);
};