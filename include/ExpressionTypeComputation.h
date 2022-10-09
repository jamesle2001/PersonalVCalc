#pragma once

#include "AST.h"
#include "SymbolTable.h"

namespace vcalc {
    class ExpressionTypeComputation {
    private:
        std::shared_ptr<SymbolTable> symtab;
        size_t numExprAncestors;
    public:
        ExpressionTypeComputation(std::shared_ptr<SymbolTable> symtab);
        void visit(std::shared_ptr<AST> t);
        void visitChildren(std::shared_ptr<AST> t);
        void visitEXPR_TOKEN(std::shared_ptr<AST> t);
        void visitINTEGER(std::shared_ptr<AST> t);
        void visitRANGE(std::shared_ptr<AST> t);
        void visitINDEX_TOKEN(std::shared_ptr<AST> t);
        void visitBinaryOperationToken(std::shared_ptr<AST> t);
        void visitPARENTHESIS_TOKEN(std::shared_ptr<AST> t);
        void visitID(std::shared_ptr<AST> t);
    };
}
