#include "DefRef.h"
#include "VCalcParser.h"
#include "ExpressionTypeComputation.h"

#include <iostream>

namespace vcalc {
    ExpressionTypeComputation::ExpressionTypeComputation(std::shared_ptr<SymbolTable> symtab) : symtab(symtab), numExprAncestors(0) { }

    void ExpressionTypeComputation::visit(std::shared_ptr<AST> t) {
        if ( t->isNil() ) {
            visitChildren(t);
        } else {
            switch ( t->getNodeType() ) {
                case VCalcParser::EXPR_TOKEN:
                    numExprAncestors++;
                    visitEXPR_TOKEN(t);
                    numExprAncestors--;
                    break;
                case VCalcParser::ADD:
                case VCalcParser::SUB:
                case VCalcParser::MUL:
                case VCalcParser::DIV:
                case VCalcParser::GREATERTHAN:
                case VCalcParser::LESSTHAN:
                case VCalcParser::ISEQUAL:
                case VCalcParser::ISNOTEQUAL:
                    visitBinaryOperationToken(t);
                    break;
                case VCalcParser::RANGE:
                    visitRANGE(t);
                    break;
                case VCalcParser::INDEX_TOKEN:
                    visitINDEX_TOKEN(t);
                    break;
                case VCalcParser::PARENTHESIS_TOKEN:
                    visitPARENTHESIS_TOKEN(t);
                    break;
                case VCalcParser::INTEGER:
                    visitINTEGER(t);
                    break;
                case VCalcParser::ID:
                    visitID(t);
                    break;
                default: // The other nodes we don't care about just have their children visited
                    visitChildren(t);
            }
        }
    }

    void ExpressionTypeComputation::visitChildren(std::shared_ptr<AST> t) {
        for ( auto child : t->children ) visit(child);
    }

    void ExpressionTypeComputation::visitEXPR_TOKEN(std::shared_ptr<AST> t) {
        visitChildren(t);  // Compute the type of subexpression
        t->evalType = t->children[0]->evalType;
        t->promoteToType = nullptr;
    }

    void ExpressionTypeComputation::visitINTEGER(std::shared_ptr<AST> t) {
        t->evalType = std::dynamic_pointer_cast<Type>(symtab->globals->resolve("int"));
        t->promoteToType = nullptr;
    }

    void ExpressionTypeComputation::visitBinaryOperationToken(std::shared_ptr<AST> t) {
        // This method run only when this AST node is: "+", "-", "*", "/", "<", ">", "==", "!="
        visitChildren(t);  // Compute the type of subexpression

        // Type promotion
        if (t->children[0]->evalType->getName() == "vector" && t->children[1]->evalType->getName() == "vector") {
            t->evalType = std::dynamic_pointer_cast<Type>(symtab->globals->resolve("vector"));
            t->promoteToType = nullptr;
            t->children[0]->promoteToType = nullptr;
            t->children[1]->promoteToType = nullptr;
        } else if (t->children[0]->evalType->getName() == "int" && t->children[1]->evalType->getName() == "vector") {
            t->evalType = std::dynamic_pointer_cast<Type>(symtab->globals->resolve("vector"));
            t->promoteToType = nullptr;
            t->children[0]->promoteToType = std::dynamic_pointer_cast<Type>(symtab->globals->resolve("vector"));
            t->children[1]->promoteToType = nullptr;
        } else if (t->children[0]->evalType->getName() == "vector" && t->children[1]->evalType->getName() == "int") {
            t->evalType = std::dynamic_pointer_cast<Type>(symtab->globals->resolve("vector"));
            t->promoteToType = nullptr;
            t->children[0]->promoteToType = nullptr;
            t->children[1]->promoteToType = std::dynamic_pointer_cast<Type>(symtab->globals->resolve("vector"));
        }
        else {
            t->evalType = std::dynamic_pointer_cast<Type>(symtab->globals->resolve("int"));
            t->promoteToType = nullptr;
            t->children[0]->promoteToType = nullptr;
            t->children[1]->promoteToType = nullptr;
        }
    }

    void ExpressionTypeComputation::visitPARENTHESIS_TOKEN(std::shared_ptr<AST> t) {
        visitChildren(t);  // Compute the type of subexpression
        t->evalType = t->children[0]->evalType;
        t->promoteToType = nullptr;
    }

    void ExpressionTypeComputation::visitRANGE(std::shared_ptr<AST> t) {
        visitChildren(t);  // Compute the type of subexpression
        t->evalType = std::dynamic_pointer_cast<Type>(symtab->globals->resolve("vector"));
        t->promoteToType = nullptr;
    }

    void ExpressionTypeComputation::visitINDEX_TOKEN(std::shared_ptr<AST> t) {
        visitChildren(t);
        t->evalType = std::dynamic_pointer_cast<Type>(symtab->globals->resolve("int"));
        t->promoteToType = nullptr;
    }

    void ExpressionTypeComputation::visitID(std::shared_ptr<AST> t) {
        if ( numExprAncestors > 0 ) { // If an ID occurs within an expression, we have an ID reference
            t->evalType = t->symbol->type;
            t->promoteToType = nullptr;
        }
    }
}