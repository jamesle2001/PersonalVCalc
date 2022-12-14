#include "DefRef.h"

#include <iostream>

#include "VCalcParser.h"
#include "LocalScope.h"
#include "Symbol.h"
#include "VariableSymbol.h"

namespace vcalc {
    DefRef::DefRef(std::shared_ptr<SymbolTable> symtab) : symtab(symtab), currentScope(symtab->globals), numExprAncestors(0) { }

    void DefRef::visit(std::shared_ptr<AST> t) {
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

    void DefRef::visitChildren(std::shared_ptr<AST> t) {
        for ( auto child : t->children ) visit(child);
    }

    std::shared_ptr<Type> DefRef::resolveType(std::shared_ptr<AST> t) {
        std::shared_ptr<Type> tsym;
        tsym = std::dynamic_pointer_cast<Type>(currentScope->resolve(t->token->getText()));
        return tsym;
    }

    void DefRef::visitBLOCK_TOKEN(std::shared_ptr<AST> t) {
        t->scope = currentScope;
        currentScope = std::make_shared<LocalScope>(currentScope); // push scope
        visitChildren(t);
        currentScope = currentScope->getEnclosingScope(); // pop scope
    }

    /* ^(VAR_DECLARATION_TOKEN type ID .?) */
    void DefRef::visitVAR_DECLARATION_TOKEN(std::shared_ptr<AST> t) {
        t->scope = currentScope;
        std::shared_ptr<AST> typeAST = t->children[0];
        std::shared_ptr<AST> idAST = t->children[1];

        std::shared_ptr<Type> type = resolveType(typeAST);
        std::shared_ptr<VariableSymbol> vs = std::make_shared<VariableSymbol>(idAST->token->getText(), type);
        currentScope->define(vs);
        t->symbol = vs;
        visitChildren(t);
    }

    /* ^(ASSIGNMENT_TOKEN ID .) */
    void DefRef::visitASSIGNMENT_TOKEN(std::shared_ptr<AST> t) {
        t->scope = currentScope;
        visitChildren(t);
        std::shared_ptr<AST> idAST = t->children[0];
        std::shared_ptr<VariableSymbol> vs = std::dynamic_pointer_cast<VariableSymbol>(currentScope->resolve(idAST->token->getText()));
        t->symbol = vs;
    }

    void DefRef::visitGENERATOR_TOKEN(std::shared_ptr<AST> t) {
        t->scope = currentScope;
        currentScope = std::make_shared<LocalScope>(currentScope); // push scope
        std::shared_ptr<AST> domainVariableAST = t->children[0];

        // Declare Domain Variable (Similar to normal variable declaration)
        std::shared_ptr<Type> intTypeSymbol = std::dynamic_pointer_cast<Type>(symtab->globals->resolve("int"));  // int is declared in global scope
        std::shared_ptr<VariableSymbol> vs = std::make_shared<VariableSymbol>(domainVariableAST->token->getText(), intTypeSymbol);
        currentScope->define(vs);
        visitChildren(t);
        currentScope = currentScope->getEnclosingScope(); // pop scope
    }

    void DefRef::visitFILTER_TOKEN(std::shared_ptr<AST> t) {
        t->scope = currentScope;
        currentScope = std::make_shared<LocalScope>(currentScope); // push scope
        std::shared_ptr<AST> domainVariableAST = t->children[0];

        // Declare Domain Variable (Similar to normal variable declaration)
        std::shared_ptr<Type> intTypeSymbol = std::dynamic_pointer_cast<Type>(symtab->globals->resolve("int"));  // int is declared in global scope
        std::shared_ptr<VariableSymbol> vs = std::make_shared<VariableSymbol>(domainVariableAST->token->getText(), intTypeSymbol);
        currentScope->define(vs);
        visitChildren(t);
        currentScope = currentScope->getEnclosingScope(); // pop scope
    }

    /* {$start.hasAncestor(EXPR)}? ID */
    void DefRef::visitID(std::shared_ptr<AST> t) {
        if ( numExprAncestors > 0 ) { // If an ID occurs within an expression, we have an ID reference
            std::shared_ptr<Symbol> s = currentScope->resolve(t->token->getText());
            t->scope = currentScope;
            t->symbol = s;
            if ( !s ) {
                std::cout << "line " << t->token->getLine() << ": ref null\n"; // variable not defined
            }
        }
    }
}