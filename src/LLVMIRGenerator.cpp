#include "LLVMIRGenerator.h"
#include "VCalcParser.h"
#include "LocalScope.h"
#include "Symbol.h"
#include "VariableSymbol.h"

namespace vcalc {
    LLVMIRGenerator::LLVMIRGenerator(std::shared_ptr<SymbolTable> symtab) : symtab(symtab), globalCtx(), ir(globalCtx), mod("vcalc", globalCtx) { };

    void LLVMIRGenerator::visit(std::shared_ptr<AST> t) {
        if ( t->isNil() ) {
            visitChildren(t);
        } else {
            switch ( t->getNodeType() ) {
                case VCalcParser::BLOCK_TOKEN:
                    visitBLOCK_TOKEN(t);
                    break;
                case VCalcParser::VAR_DECLARATION_TOKEN:
                    visitVAR_DECLARATION_TOKEN(t);
                    break;
                case VCalcParser::ASSIGNMENT_TOKEN:
                    visitASSIGNMENT_TOKEN(t);
                    break;
                case VCalcParser::ID:
                    visitID(t);
                    break;
                case VCalcParser::EXPR_TOKEN:
                    // Keep track of number of expression ancestors for id references
                    numExprAncestors++;
                    visitChildren(t);
                    numExprAncestors--;
                    break;
                default: // The other nodes we don't care about just have their children visited
                    visitChildren(t);
            }
        }
    }

    void LLVMIRGenerator::visitChildren(std::shared_ptr<AST> t) {
        for ( auto child : t->children ) visit(child);
    }

    void visitVAR_DECLARATION_TOKEN(std::shared_ptr<AST> t) {
        
    }
}